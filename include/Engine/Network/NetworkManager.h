#pragma once
#include "Engine/Common.h"
#include <SFML/Network.hpp>
#include <functional>

class NetworkManager {
public:
    NetworkManager();

    void StartHost();
    void StartClient(const std::wstring& ip);
    void Disconnect();

    void SendMove(int fromR, int fromC, int toR, int toC);
    void SendUndoRequest();
    void SendUndoAccept();
    void SendUndoReject();
    void SendRestartRequest();
    void SendRestartAccept();
    void SendRestartReject();
    void SendSurrenderRequest();
    void SendSurrenderResponse(bool accept);
    void SendDrawRequest();
    void SendDrawResponse(bool accept);
    void SendUndoAck(int steps);
    void SendRestartAck();

    void PollNetwork();

    NetState GetState() const { return netState; }
    Side GetNetSide() const { return netSide; }
    bool IsNetMode() const { return netMode; }
    std::wstring GetLocalIP() const;
    void SetNetMode(bool mode) { netMode = mode; }

    void ResetRequests();

    using MoveCallback = std::function<void(int,int,int,int)>;
    using UndoRequestCallback = std::function<void()>;
    using RestartRequestCallback = std::function<void()>;
    using SurrenderRequestCallback = std::function<void()>;
    using DrawRequestCallback = std::function<void()>;
    using UndoAcceptCallback = std::function<void(int)>;
    using RestartAcceptCallback = std::function<void()>;
    using SurrenderAcceptCallback = std::function<void()>;
    using DrawAcceptCallback = std::function<void()>;
    using RejectCallback = std::function<void(const std::wstring&)>;
    using DisconnectCallback = std::function<void()>;

    MoveCallback onMoveReceived;
    UndoRequestCallback onUndoRequestReceived;
    RestartRequestCallback onRestartRequestReceived;
    SurrenderRequestCallback onSurrenderRequestReceived;
    DrawRequestCallback onDrawRequestReceived;
    UndoAcceptCallback onUndoAccepted;
    RestartAcceptCallback onRestartAccepted;
    SurrenderAcceptCallback onSurrenderAccepted;
    DrawAcceptCallback onDrawAccepted;
    RejectCallback onRejected;
    DisconnectCallback onDisconnected;

    bool undoRequestSent = false;
    bool undoRequestReceived = false;
    bool restartRequestSent = false;
    bool restartRequestReceived = false;

    bool surrenderRequestSent = false;
    bool surrenderRequestReceived = false;
    bool drawRequestSent = false;
    bool drawRequestReceived = false;
    Side undoRequesterSide = Side::RED;
    Side surrenderRequesterSide = Side::RED;

private:
    NetState netState = NetState::OFFLINE;
    Side netSide = Side::RED;
    bool netMode = false;
    sf::TcpListener listener;
    sf::TcpSocket socket;
    sf::SocketSelector selector;
};
