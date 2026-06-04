// ===================================================================
// NetworkManager.cpp — 网络联机管理器的实现
// 使用 SFML 的 TCP 通信实现局域网联机对战
// 通信协议：每个数据包第一个 int 是消息类型，后面跟参数
// ===================================================================
//
// 协议号定义：
//   0  = 走棋消息（附带 fromR, fromC, toR, toC）
//   1  = 悔棋请求
//   2  = 同意悔棋
//   3  = 拒绝悔棋
//   4  = 重开请求
//   5  = 同意重开
//   6  = 拒绝重开
//   7  = 悔棋确认（附带 steps=悔了几步）
//   8  = 重开确认
//   9  = 认输请求
//   10 = 同意认输
//   11 = 拒绝认输
//   12 = 和棋请求
//   13 = 同意和棋
//   14 = 拒绝和棋
// ===================================================================

#include "Engine/Network/NetworkManager.h"
#include <SFML/Network.hpp>
#include <iostream>

// 构造函数：初始化为离线状态
NetworkManager::NetworkManager() {
    netState = NetState::OFFLINE;
    netSide = Side::RED;
    netMode = false;
}

// ==================== 创建房间（房主）====================
// 房主监听端口 55001，等待客户端连接
// 房主永远执红方先行
void NetworkManager::StartHost() {
    listener.setBlocking(false);                         // 设置非阻塞模式
    if (listener.listen(55001) == sf::Socket::Done) {    // 开始监听 55001 端口
        netState = NetState::HOST_WAITING;               // 进入"等待连接"状态
        netSide = Side::RED;                             // 房主执红方
        ResetRequests();                                 // 清空之前的请求状态
    }
}

// ==================== 加入房间（客户端）====================
// 客户端连接到房主的 IP 地址，端口 55001
// 客户端永远执黑方后手
void NetworkManager::StartClient(const std::wstring& ip) {
    socket.setBlocking(true);                            // 连接时使用阻塞模式
    std::string ipStr(ip.begin(), ip.end());             // wstring → string
    sf::Socket::Status status = socket.connect(sf::IpAddress(ipStr), 55001, sf::seconds(3));
    if (status == sf::Socket::Done) {
        socket.setBlocking(false);                       // 连接成功后切换为非阻塞
        netState = NetState::CONNECTED;
        netSide = Side::BLACK;                           // 客户端执黑方
        ResetRequests();
    }
}

// 断开连接
void NetworkManager::Disconnect() {
    if (netState != NetState::OFFLINE) {
        socket.disconnect();
        listener.close();
        netState = NetState::OFFLINE;
        netMode = false;
        ResetRequests();
    }
}

// 重置所有请求状态（用于新游戏开始时清除残留）
void NetworkManager::ResetRequests() {
    undoRequestSent = false;
    undoRequestReceived = false;
    restartRequestSent = false;
    restartRequestReceived = false;
    surrenderRequestSent = false;
    surrenderRequestReceived = false;
    drawRequestSent = false;
    drawRequestReceived = false;
}

// ==================== 发送消息 ====================
// 每条消息都先创建一个 sf::Packet，然后逐个写入数据
// 消息格式：第一个 int 是协议号，后面跟具体参数

void NetworkManager::SendMove(int fromR, int fromC, int toR, int toC) {
    sf::Packet packet;
    packet << 0 << fromR << fromC << toR << toC;        // 协议号 0 + 4个坐标
    socket.send(packet);
}

void NetworkManager::SendUndoRequest() {
    sf::Packet packet;
    packet << 1;                                         // 协议号 1
    socket.send(packet);
}

void NetworkManager::SendUndoAccept() {
    sf::Packet packet;
    packet << 2;                                         // 协议号 2
    socket.send(packet);
}

void NetworkManager::SendUndoReject() {
    sf::Packet packet;
    packet << 3;                                         // 协议号 3
    socket.send(packet);
}

void NetworkManager::SendRestartRequest() {
    sf::Packet packet;
    packet << 4;                                         // 协议号 4
    socket.send(packet);
}

void NetworkManager::SendRestartAccept() {
    sf::Packet packet;
    packet << 5;                                         // 协议号 5
    socket.send(packet);
}

void NetworkManager::SendRestartReject() {
    sf::Packet packet;
    packet << 6;                                         // 协议号 6
    socket.send(packet);
}

void NetworkManager::SendSurrenderRequest() {
    sf::Packet packet;
    packet << 9;                                         // 协议号 9
    socket.send(packet);
}

void NetworkManager::SendSurrenderResponse(bool accept) {
    sf::Packet packet;
    packet << (accept ? 10 : 11);                        // 协议号 10(同意) 或 11(拒绝)
    socket.send(packet);
}

void NetworkManager::SendDrawRequest() {
    sf::Packet packet;
    packet << 12;                                        // 协议号 12
    socket.send(packet);
}

void NetworkManager::SendDrawResponse(bool accept) {
    sf::Packet packet;
    packet << (accept ? 13 : 14);                        // 协议号 13(同意) 或 14(拒绝)
    socket.send(packet);
}

void NetworkManager::SendUndoAck(int steps) {
    sf::Packet packet;
    packet << 7 << steps;                                // 协议号 7 + 悔棋步数
    socket.send(packet);
}

void NetworkManager::SendRestartAck() {
    sf::Packet packet;
    packet << 8;                                         // 协议号 8
    socket.send(packet);
}

// ==================== 网络轮询（每帧调用）====================
// 检查是否有新连接（房主模式）或新消息到达（已连接模式）
void NetworkManager::PollNetwork() {
    // —— 房主等待连接 —— 接受客户端连接请求
    if (netState == NetState::HOST_WAITING) {
        sf::Socket::Status status = listener.accept(socket);
        if (status == sf::Socket::Done) {
            socket.setBlocking(false);                   // 连接后切换非阻塞
            netState = NetState::CONNECTED;
            netSide = Side::RED;
            ResetRequests();
        }
        return;
    }

    // 非已连接状态不处理消息
    if (netState != NetState::CONNECTED) return;

    // 尝试接收消息
    sf::Packet packet;
    sf::Socket::Status status = socket.receive(packet);
    if (status == sf::Socket::Done) {
        int msgType;
        packet >> msgType;                               // 读取协议号

        switch (msgType) {
            case 0: { // 走棋消息
                int fromR, fromC, toR, toC;
                packet >> fromR >> fromC >> toR >> toC;
                if (onMoveReceived) onMoveReceived(fromR, fromC, toR, toC);
                break;
            }
            case 1: { // 悔棋请求
                // 如果我也发了悔棋请求 → 双方同时请求 → 当作同意处理
                if (undoRequestSent) {
                    undoRequestSent = false;
                    if (onUndoAccepted) onUndoAccepted();
                } else {
                    undoRequestReceived = true;
                    undoRequesterSide = (netSide == Side::RED) ? Side::BLACK : Side::RED;
                    if (onUndoRequestReceived) onUndoRequestReceived();
                }
                break;
            }
            case 2: // 对方同意悔棋
                undoRequestSent = false;
                if (onUndoAccepted) onUndoAccepted();
                break;
            case 3: // 对方拒绝悔棋
                undoRequestSent = false;
                if (onRejected) onRejected(L"\u5bf9\u65b9\u62d2\u7edd\u4e86\u6094\u68cb\u8bf7\u6c42");  // "对方拒绝了悔棋请求"
                break;
            case 4: { // 重开请求
                if (restartRequestSent) {
                    restartRequestSent = false;
                    if (onRestartAccepted) onRestartAccepted();
                } else {
                    restartRequestReceived = true;
                    if (onRestartRequestReceived) onRestartRequestReceived();
                }
                break;
            }
            case 5: // 对方同意重开
                restartRequestSent = false;
                if (onRestartAccepted) onRestartAccepted();
                break;
            case 6: // 对方拒绝重开
                restartRequestSent = false;
                if (onRejected) onRejected(L"\u5bf9\u65b9\u62d2\u7edd\u4e86\u91cd\u5f00\u8bf7\u6c42");  // "对方拒绝了重开请求"
                break;
            case 7: { // 悔棋确认（告知悔了几步）
                int steps;
                packet >> steps;
                if (onUndoAckReceived) onUndoAckReceived(steps);
                break;
            }
            case 8: // 重开确认
                if (onRestartAccepted) onRestartAccepted();
                break;
            case 9: { // 认输请求
                if (surrenderRequestSent) {
                    surrenderRequestSent = false;
                    if (onSurrenderAccepted) onSurrenderAccepted();
                } else {
                    surrenderRequestReceived = true;
                    surrenderRequesterSide = (netSide == Side::RED) ? Side::BLACK : Side::RED;
                    if (onSurrenderRequestReceived) onSurrenderRequestReceived();
                }
                break;
            }
            case 10: // 对方同意认输
                surrenderRequestSent = false;
                if (onSurrenderAccepted) onSurrenderAccepted();
                break;
            case 11: // 对方拒绝认输
                surrenderRequestSent = false;
                if (onRejected) onRejected(L"\u5bf9\u65b9\u62d2\u7edd\u4e86\u8ba4\u8f93\u8bf7\u6c42");  // "对方拒绝了认输请求"
                break;
            case 12: { // 和棋请求
                if (drawRequestSent) {
                    drawRequestSent = false;
                    if (onDrawAccepted) onDrawAccepted();
                } else {
                    drawRequestReceived = true;
                    if (onDrawRequestReceived) onDrawRequestReceived();
                }
                break;
            }
            case 13: // 对方同意和棋
                drawRequestSent = false;
                if (onDrawAccepted) onDrawAccepted();
                break;
            case 14: // 对方拒绝和棋
                drawRequestSent = false;
                if (onRejected) onRejected(L"\u5bf9\u65b9\u62d2\u7edd\u4e86\u548c\u68cb\u8bf7\u6c42");  // "对方拒绝了和棋请求"
                break;
        }
    } else if (status == sf::Socket::Disconnected || status == sf::Socket::Error) {
        // 对方断开连接
        if (onDisconnected) onDisconnected();
    }
}

// 获取本机局域网 IP 地址
std::wstring NetworkManager::GetLocalIP() const {
    sf::IpAddress ip = sf::IpAddress::getLocalAddress();  // SFML 获取本机 IP
    std::string ipStr = ip.toString();
    return std::wstring(ipStr.begin(), ipStr.end());
}
