#include "Engine/Network/NetworkManager.h"
#include <SFML/Network.hpp>
#include <iostream>

NetworkManager::NetworkManager() {
    netState = NetState::OFFLINE;
    netSide = Side::RED;
    netMode = false;
}

void NetworkManager::StartHost() {
    listener.setBlocking(false);
    if (listener.listen(55001) == sf::Socket::Done) {
        netState = NetState::HOST_WAITING;
        netSide = Side::RED;
        ResetRequests();
    }
}

void NetworkManager::StartClient(const std::wstring& ip) {
    socket.setBlocking(true);
    std::string ipStr(ip.begin(), ip.end());
    sf::Socket::Status status = socket.connect(sf::IpAddress(ipStr), 55001, sf::seconds(3));
    if (status == sf::Socket::Done) {
        socket.setBlocking(false);
        netState = NetState::CONNECTED;
        netSide = Side::BLACK;
        ResetRequests();
    }
}

void NetworkManager::Disconnect() {
    if (netState != NetState::OFFLINE) {
        socket.disconnect();
        listener.close();
        netState = NetState::OFFLINE;
        netMode = false;
        ResetRequests();
    }
}

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

void NetworkManager::SendMove(int fromR, int fromC, int toR, int toC) {
    sf::Packet packet;
    packet << 0 << fromR << fromC << toR << toC;
    socket.send(packet);
}

void NetworkManager::SendUndoRequest() {
    sf::Packet packet;
    packet << 1;
    socket.send(packet);
}

void NetworkManager::SendUndoAccept() {
    sf::Packet packet;
    packet << 2;
    socket.send(packet);
}

void NetworkManager::SendUndoReject() {
    sf::Packet packet;
    packet << 3;
    socket.send(packet);
}

void NetworkManager::SendRestartRequest() {
    sf::Packet packet;
    packet << 4;
    socket.send(packet);
}

void NetworkManager::SendRestartAccept() {
    sf::Packet packet;
    packet << 5;
    socket.send(packet);
}

void NetworkManager::SendRestartReject() {
    sf::Packet packet;
    packet << 6;
    socket.send(packet);
}

void NetworkManager::SendSurrenderRequest() {
    sf::Packet packet;
    packet << 9;
    socket.send(packet);
}

void NetworkManager::SendSurrenderResponse(bool accept) {
    sf::Packet packet;
    packet << (accept ? 10 : 11);
    socket.send(packet);
}

void NetworkManager::SendDrawRequest() {
    sf::Packet packet;
    packet << 12;
    socket.send(packet);
}

void NetworkManager::SendDrawResponse(bool accept) {
    sf::Packet packet;
    packet << (accept ? 13 : 14);
    socket.send(packet);
}

void NetworkManager::SendUndoAck(int steps) {
    sf::Packet packet;
    packet << 7 << steps;
    socket.send(packet);
}

void NetworkManager::SendRestartAck() {
    sf::Packet packet;
    packet << 8;
    socket.send(packet);
}

void NetworkManager::PollNetwork() {
    if (netState == NetState::HOST_WAITING) {
        sf::Socket::Status status = listener.accept(socket);
    if (status == sf::Socket::Done) {
            socket.setBlocking(false);
            netState = NetState::CONNECTED;
            netSide = Side::RED;
            ResetRequests();
        }
        return;
    }

    if (netState != NetState::CONNECTED) return;

    sf::Packet packet;
    sf::Socket::Status status = socket.receive(packet);
    if (status == sf::Socket::Done) {
        int msgType;
        packet >> msgType;
        switch (msgType) {
            case 0: { // Move
                int fromR, fromC, toR, toC;
                packet >> fromR >> fromC >> toR >> toC;
                if (onMoveReceived) onMoveReceived(fromR, fromC, toR, toC);
                break;
            }
            case 1: { // Undo request
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
            case 2: // Undo accept
                undoRequestSent = false;
                if (onUndoAccepted) onUndoAccepted();
                break;
            case 3: // Undo reject
                undoRequestSent = false;
                if (onRejected) onRejected(L"\u5bf9\u65b9\u62d2\u7edd\u4e86\u6094\u68cb\u8bf7\u6c42");
                break;
            case 4: // Restart request
                if (restartRequestSent) {
                    restartRequestSent = false;
                    if (onRestartAccepted) onRestartAccepted();
                } else {
                    restartRequestReceived = true;
                    if (onRestartRequestReceived) onRestartRequestReceived();
                }
                break;
            case 5: // Restart accept
                restartRequestSent = false;
                if (onRestartAccepted) onRestartAccepted();
                break;
            case 6: // Restart reject
                restartRequestSent = false;
                if (onRejected) onRejected(L"\u5bf9\u65b9\u62d2\u7edd\u4e86\u91cd\u5f00\u8bf7\u6c42");
                break;
            case 7: { // Undo ack
                int steps;
                packet >> steps;
                if (onUndoAckReceived) onUndoAckReceived(steps);
                break;
            }
            case 8: // Restart ack
                if (onRestartAccepted) onRestartAccepted();
                break;
            case 9: // Surrender request
                if (surrenderRequestSent) {
                    surrenderRequestSent = false;
                    if (onSurrenderAccepted) onSurrenderAccepted();
                } else {
                    surrenderRequestReceived = true;
                    surrenderRequesterSide = (netSide == Side::RED) ? Side::BLACK : Side::RED;
                    if (onSurrenderRequestReceived) onSurrenderRequestReceived();
                }
                break;
            case 10: // Surrender accept
                surrenderRequestSent = false;
                if (onSurrenderAccepted) onSurrenderAccepted();
                break;
            case 11: // Surrender reject
                surrenderRequestSent = false;
                if (onRejected) onRejected(L"\u5bf9\u65b9\u62d2\u7edd\u4e86\u8ba4\u8f93\u8bf7\u6c42");
                break;
            case 12: // Draw request
                if (drawRequestSent) {
                    drawRequestSent = false;

                    if (onDrawAccepted) onDrawAccepted();
                } else {
                    drawRequestReceived = true;
                    if (onDrawRequestReceived) onDrawRequestReceived();
                }
                break;
            case 13: // Draw accept
                drawRequestSent = false;
                if (onDrawAccepted) onDrawAccepted();
                break;
            case 14: // Draw reject
                drawRequestSent = false;
                if (onRejected) onRejected(L"\u5bf9\u65b9\u62d2\u7edd\u4e86\u548c\u68cb\u8bf7\u6c42");
                break;
        }
    } else if (status == sf::Socket::Disconnected || status == sf::Socket::Error) {
        if (onDisconnected) onDisconnected();
    }
}

std::wstring NetworkManager::GetLocalIP() const {
    sf::IpAddress ip = sf::IpAddress::getLocalAddress();
    std::string ipStr = ip.toString();
    return std::wstring(ipStr.begin(), ipStr.end());
}
