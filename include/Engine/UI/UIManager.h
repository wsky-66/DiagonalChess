#pragma once
#include "Engine/Common.h"
#include "Engine/Board/DiagonalChessBoard.h"
#include <SFML/Graphics.hpp>
#include <vector>

class UIManager {
public:
    UIManager();

    void Init(sf::RenderWindow& w, sf::Font& f, bool fontOk);

    void DrawBoard(const DiagonalChessBoard& board);
    void DrawPieces(const DiagonalChessBoard& board, int selectedR, int selectedC,
                    const std::vector<sf::Vector2i>& validMoves);
    void DrawUI(Side currentTurn, bool gameOver, bool isDrawGame, bool surrendered, Side winner,
                bool inCheck, bool aiThinking);
    void DrawButtons();
    void DrawRequestPopups(bool undoRecv, bool restartRecv, bool surrRecv, bool drawRecv);
    void DrawMoveLog(const std::vector<std::wstring>& moveLogStrings);
    void DrawNetUI(bool netMode, NetState netState, const std::wstring& localIP,
                   const std::wstring& inputIP, bool showIPInput, Side netSide);
    void DrawTutorialPanel();
    void DrawSurrenderPopup();
    void DrawGameOverEffect(float gameOverTimer, bool isDrawGame, Side winner,
                            bool surrendered, bool checkmate, int movesWithoutCapture,
                            bool agreedDraw, bool hasInsufficientMaterial);

    void DrawText(const std::wstring& text, float x, float y,
                  unsigned int size, sf::Color color, bool center = false);
    void DrawTextWithShadow(const std::wstring& text, float x, float y,
                            unsigned int size, sf::Color color, bool center = false);

    void UpdateHover(float mx, float my);
    void SetDraggingLogDivider(bool v) { draggingLogDivider = v; }
    bool IsDraggingLogDivider() const { return draggingLogDivider; }
    void SetLogDividerY(float y) { logDividerY = y; }
    float GetLogDividerY() const { return logDividerY; }
    int GetLogScrollOffset() const { return logScrollOffset; }
    void SetLogScrollOffset(int v) { logScrollOffset = v; }

    bool IsShowTutorial() const { return showTutorial; }
    void SetShowTutorial(bool v) { showTutorial = v; }
    void SetShowSurrenderPopup(bool v) { showSurrenderPopup = v; }
    bool IsShowSurrenderPopup() const { return showSurrenderPopup; }
    void SetShowIPInput(bool v) { showIPInput = v; }
    bool IsShowIPInput() const { return showIPInput; }

    std::wstring GetInputIP() const { return inputIP; }
    void SetInputIP(const std::wstring& s) { inputIP = s; }
    void AppendInputIP(wchar_t ch) { if (inputIP.size() < 15) inputIP += ch; }
    void BackspaceInputIP() { if (!inputIP.empty()) inputIP.pop_back(); }

    void SetNotification(const std::wstring& text, float duration) {
        notificationText = text; notificationTimer = duration;
    }
    void UpdateNotification(float dt) {
        if (notificationTimer > 0.f) {
            notificationTimer -= dt;
            if (notificationTimer <= 0.f) { notificationTimer = 0.f; notificationText.clear(); }
        }
    }

    void DrawNotification();

    UIButton undoBtn, restartBtn, aiBtn, difficultyBtn, onlineBtn, tutorialBtn;
    UIButton surrenderBtn, drawOfferBtn;
    UIButton hostBtn, joinBtn, connectBtn, disconnectBtn;
    UIButton gameOverRestartBtn;
    UIButton undoAcceptBtn, undoRejectBtn;
    UIButton restartAcceptBtn, restartRejectBtn;
    UIButton surrenderAcceptBtn, surrenderRejectBtn;
    UIButton surrenderRedBtn, surrenderBlackBtn;
    UIButton drawAcceptBtn, drawRejectBtn;

private:
    sf::RenderWindow* window;
    sf::Font* font;
    bool fontLoaded;

    bool showTutorial;
    int logScrollOffset;
    float logDividerY;
    bool draggingLogDivider;
    bool showIPInput;
    bool showSurrenderPopup;

    std::wstring inputIP;
    std::wstring notificationText;
    float notificationTimer;

    void DrawButton(const UIButton& btn);
    void DrawSmallButton(const UIButton& btn, sf::Color fill, sf::Color outline);
    void DrawNetButton(const UIButton& btn);
    void DrawNetRedButton(const UIButton& btn);
};
