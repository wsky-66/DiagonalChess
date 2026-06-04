#pragma once
#include "Engine/Core/Game.h"
#include "Engine/Board/GomokuBoard.h"
#include "Engine/Rule/GomokuRule.h"
#include <SFML/Graphics.hpp>
#include <stack>
#include <vector>

// 五子棋游戏：事件处理、渲染、回合切换、按钮逻辑
class GomokuGame : public Game {
public:
    GomokuGame();
    ~GomokuGame() override;

    void Update(float dt) override;
    void Render() override;
    void Reset() override;
    std::string GetName() const override { return "Gomoku"; }
    bool IsRunning() const override { return window.isOpen(); }

private:
    // —— 事件 ——
    void ProcessEvents();

    // —— 棋盘点击 ——
    void HandleBoardClick(int r, int c);

    // —— 按钮点击 ——
    void HandleButtonClick(float mx, float my);

    // —— 落子执行 ——
    void PlaceStone(int r, int c);

    // —— 悔棋 ——
    void UndoMove();

    // —— 游戏结束检测 ——
    void CheckGameEnd(int r, int c);

    // —— 认输 ——
    void DoSurrender();

    // —— 重新开始 ——
    void RestartGame();

    // —— 渲染 ——
    void DrawBoard();
    void DrawStones();
    void DrawUI();
    void DrawGameOver();
    void DrawText(const std::wstring& text, float x, float y, unsigned sz,
                  sf::Color c, bool center = false);

    // —— 辅助 ——
    sf::Color GetSideColor(int side) const;

    // 棋盘 & 规则
    GomokuBoard board;
    GomokuRule rule;

    // 窗口 & 字体
    sf::RenderWindow window;
    sf::Font font;
    bool fontLoaded;

    // 游戏状态
    int currentTurn;        // 1=黑先, 2=白后
    bool gameOver;
    int  winner;            // 1=黑胜, 2=白胜
    bool isDraw;
    bool surrendered;

    // 鼠标悬停
    int hoverR, hoverC;     // 当前悬停的交叉点坐标，-1 表示无
    bool mouseOnBoard;      // 鼠标是否在棋盘区域内

    // 悔棋记录
    struct GomokuRecord {
        int r, c;
        int side;
    };
    std::stack<GomokuRecord> moveHistory;

    // 按钮
    struct RectButton {
        sf::FloatRect bounds;
        std::wstring label;
        bool hovered;
    };
    RectButton restartBtn;
    RectButton undoBtn;
    RectButton surrenderBtn;
    RectButton gameOverRestartBtn;
    RectButton closeBtn;

    // 游戏结束动画计时
    float gameOverTimer;

    // 窗口尺寸
    static constexpr int WIN_W = 1150;
    static constexpr int WIN_H = 820;
};
