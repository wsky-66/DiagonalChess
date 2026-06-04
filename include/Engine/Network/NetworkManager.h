#pragma once

// ===================================================================
// NetworkManager.h — 网络联机管理器
// 基于 SFML 的 TCP 通信实现联机对战
// 支持：创建房间（房主）、加入房间（客户端）、收发走棋和请求
// ===================================================================

#include "Engine/Common.h"
#include <SFML/Network.hpp>          // SFML 网络功能（TcpSocket, Packet 等）
#include <functional>                // std::function（回调函数）

class NetworkManager {
public:
    NetworkManager();

    // —— 网络连接控制 ——
    void StartHost();                // 创建房间（作为房主，监听端口55001）
    void StartClient(const std::wstring& ip);  // 加入房间（连接到指定 IP）
    void Disconnect();               // 断开连接

    // —— 发送各种网络消息 ——
    // 走棋消息：协议号 0，附带四个坐标参数
    void SendMove(int fromR, int fromC, int toR, int toC);

    // 悔棋请求/同意/拒绝：协议号 1/2/3
    void SendUndoRequest();
    void SendUndoAccept();
    void SendUndoReject();

    // 重开请求/同意/拒绝：协议号 4/5/6
    void SendRestartRequest();
    void SendRestartAccept();
    void SendRestartReject();

    // 悔棋确认（告知对方悔了几步）：协议号 7
    void SendUndoAck(int steps);
    // 重开确认：协议号 8
    void SendRestartAck();

    // 认输请求/响应：协议号 9/10/11
    void SendSurrenderRequest();
    void SendSurrenderResponse(bool accept);

    // 和棋请求/响应：协议号 12/13/14
    void SendDrawRequest();
    void SendDrawResponse(bool accept);

    // —— 网络轮询 ——
    // 每帧调用一次，检查是否有新连接或新消息到达
    void PollNetwork();

    // —— 状态查询 ——
    NetState GetState() const { return netState; }      // 当前网络状态
    Side GetNetSide() const { return netSide; }          // 我方在网络对局中的阵营
    bool IsNetMode() const { return netMode; }           // 是否开启了联机模式
    std::wstring GetLocalIP() const;                     // 获取本机局域网 IP
    void SetNetMode(bool mode) { netMode = mode; }       // 设置联机模式开关

    void ResetRequests();            // 重置所有请求状态

    // —— 回调函数类型定义（供 Game 层注册事件处理）——
    using MoveCallback = std::function<void(int,int,int,int)>;    // 收到对方走棋
    using UndoRequestCallback = std::function<void()>;            // 收到悔棋请求
    using RestartRequestCallback = std::function<void()>;         // 收到重开请求
    using SurrenderRequestCallback = std::function<void()>;       // 收到认输请求
    using DrawRequestCallback = std::function<void()>;            // 收到和棋请求
    using UndoAcceptCallback = std::function<void()>;             // 对方同意悔棋
    using UndoAckCallback = std::function<void(int steps)>;       // 收到悔棋确认
    using RestartAcceptCallback = std::function<void()>;          // 对方同意重开
    using SurrenderAcceptCallback = std::function<void()>;        // 对方同意认输
    using DrawAcceptCallback = std::function<void()>;             // 对方同意和棋
    using RejectCallback = std::function<void(const std::wstring&)>;  // 对方拒绝
    using DisconnectCallback = std::function<void()>;             // 对方断开

    // —— 公开的回调函数指针（Game 层设置它们以处理网络事件）——
    MoveCallback onMoveReceived;
    UndoRequestCallback onUndoRequestReceived;
    RestartRequestCallback onRestartRequestReceived;
    SurrenderRequestCallback onSurrenderRequestReceived;
    DrawRequestCallback onDrawRequestReceived;
    UndoAcceptCallback onUndoAccepted;
    UndoAckCallback onUndoAckReceived;
    RestartAcceptCallback onRestartAccepted;
    SurrenderAcceptCallback onSurrenderAccepted;
    DrawAcceptCallback onDrawAccepted;
    RejectCallback onRejected;
    DisconnectCallback onDisconnected;

    // —— 请求状态标志（用于 UI 判断是否正在等待对方回应）——
    bool undoRequestSent = false;          // 我是否已发出悔棋请求
    bool undoRequestReceived = false;      // 是否收到了对方的悔棋请求
    bool restartRequestSent = false;
    bool restartRequestReceived = false;
    bool surrenderRequestSent = false;
    bool surrenderRequestReceived = false;
    bool drawRequestSent = false;
    bool drawRequestReceived = false;
    Side undoRequesterSide = Side::RED;    // 悔棋请求方的阵营
    Side surrenderRequesterSide = Side::RED; // 认输请求方的阵营

private:
    NetState netState = NetState::OFFLINE;  // 当前网络状态
    Side netSide = Side::RED;               // 我方阵营（房主=红, 客户端=黑）
    bool netMode = false;                   // 是否开启联机模式

    // —— SFML 网络对象 ——
    sf::TcpListener listener;              // TCP 监听器（房主用）
    sf::TcpSocket socket;                  // TCP 套接字（通信用）
    sf::SocketSelector selector;           // 套接字选择器（用于非阻塞轮询）
};
