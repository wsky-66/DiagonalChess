#pragma once
#include "Engine/Core/Game.h"
#include "Engine/Board/DiagonalChessBoard.h"
#include "Engine/Rule/DiagonalChessRule.h"
#include "Engine/AI/DiagonalChessAI.h"
#include "Engine/Network/NetworkManager.h"
#include "Engine/UI/UIManager.h"
#include "Engine/Audio/AudioManager.h"
#include "Engine/Effects/ParticleSystem.h"
#include <stack>
#include <vector>

class DiagonalChessGame : public Game {
public:
    DiagonalChessGame();
    ~DiagonalChessGame() override;

    void Update(float dt) override;
    void Render() override;
    void Reset() override;
    std::string GetName() const override { return "DiagonalChess"; }
    bool IsRunning() const override { return window.isOpen(); }

private:
    void ProcessEvents();
    void HandleBoardClick(int r, int c);
    void HandleButtonClick(float mx, float my);
    void ExecuteMove(int fromR, int fromC, int toR, int toC);
    void UndoMove();
    void RestartGame();
    void DoSurrender(Side side);
    void DoDraw();
    void CheckGameEnd();
    void DoAITurn();
    void ApplyUndoSteps(int steps);

    std::wstring GetMoveString(const MoveRecord& move) const;

    void SetupNetworkCallbacks();

    DiagonalChessBoard board;
    DiagonalChessRule rule;
    DiagonalChessAI ai;
    NetworkManager network;
    UIManager ui;
    AudioManager audio;
    ParticleSystem particles;

    sf::RenderWindow window;
    sf::Font font;
    sf::View view;
    bool running;
    bool fontLoaded;

    Side currentTurn;
    int selectedR, selectedC;
    bool pieceSelected;
    std::vector<sf::Vector2i> validMoves;
    bool gameOver;
    Side winner;
    bool isDrawGame;
    bool surrendered;
    bool agreedDraw;
    float gameOverTimer;
    int movesWithoutCapture;

    std::stack<MoveRecord> moveHistory;
    std::vector<std::wstring> moveLogStrings;

    bool aiMode;
    Side aiSide;
    bool aiThinking;
    float aiDelayTimer;

    bool receivingMove;

    static constexpr int WIN_W = 1150;
    static constexpr int WIN_H = 820;
    static constexpr int MAX_LOG = 20;
    static constexpr int DRAW_LIMIT = 120;
};
