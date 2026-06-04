#pragma once

// ===================================================================
// GomokuGame.h — 五子棋游戏主类
// 继承自 Game（即 Scene），是整个五子棋游戏的核心控制器
// 包含：棋盘、规则、事件处理、渲染、回合切换、悔棋、认输
// ===================================================================

#include "Engine/Core/Game.h"
#include "Engine/Board/GomokuBoard.h"
#include "Engine/Rule/GomokuRule.h"
#include <SFML/Graphics.hpp>
#include <stack>
#include <vector>

class GomokuGame : public Game {
public:
    GomokuGame();
    ~GomokuGame() override;

    // —— 实现 Game 接口 ——
    void Update(float dt) override;           // 每帧更新：处理事件
    void Render() override;                   // 每帧渲染：棋盘、棋子、UI
    void Reset() override;                    // 重置游戏
    std::string GetName() const override { return "Gomoku"; }
    bool IsRunning() const override { return window.isOpen(); }

private:
    void ProcessEvents();                     // 处理窗口事件（鼠标点击、移动、关闭）
    void HandleBoardClick(int r, int c);      // 处理棋盘点击（落子）
    void HandleButtonClick(float mx, float my); // 处理按钮点击

    void PlaceStone(int r, int c);            // 执行落子
    void UndoMove();                          // 悔棋（撤回上一步）
    void CheckGameEnd(int r, int c);          // 检查游戏是否结束（五连/棋盘满）
    void DoSurrender();                       // 认输
    void RestartGame();                       // 重新开始

    // —— 渲染 ——
    void DrawBoard();                         // 绘制棋盘背景和网格线
    void DrawStones();                        // 绘制棋子（含悬停预览）
    void DrawUI();                            // 绘制右侧 UI 面板
    void DrawGameOver();                      // 绘制游戏结束遮罩
    void DrawText(const std::wstring& text, float x, float y,
                  unsigned sz, sf::Color c, bool center = false);

    // —— 辅助 ——
    sf::Color GetSideColor(int side) const;   // 获取阵营对应颜色（黑=深色, 白=浅色）

    // —— 游戏模块 ——
    GomokuBoard board;                        // 15×15 棋盘
    GomokuRule rule;                          // 五子棋规则

    // —— SFML 窗口 ——
    sf::RenderWindow window;
    sf::Font font;
    bool fontLoaded;

    // —— 游戏状态 ——
    int currentTurn;                          // 当前回合：1=黑方先行, 2=白方后手
    bool gameOver;                            // 游戏是否结束
    int  winner;                              // 胜者：1=黑胜, 2=白胜
    bool isDraw;                              // 是否平局
    bool surrendered;                         // 是否认输

    // —— 鼠标交互 ——
    int hoverR, hoverC;                       // 鼠标悬停的交叉点坐标（-1=无）
    bool mouseOnBoard;                        // 鼠标是否在棋盘区域

    // —— 悔棋记录 ——
    struct GomokuRecord {
        int r, c;                             // 落子位置
        int side;                             // 落子方
    };
    std::stack<GomokuRecord> moveHistory;     // 走棋历史栈

    // —— 按钮 ——
    struct RectButton {
        sf::FloatRect bounds;                 // 按钮矩形区域
        std::wstring label;                   // 按钮文字
        bool hovered;                         // 鼠标是否悬停
    };
    RectButton restartBtn;                    // 重新开始
    RectButton undoBtn;                       // 悔棋
    RectButton surrenderBtn;                  // 认输
    RectButton gameOverRestartBtn;            // 游戏结束后的重新开始按钮
    RectButton closeBtn;                      // 关闭按钮（返回主菜单）

    float gameOverTimer;                      // 游戏结束后的计时器（渐显动画）

    static constexpr int WIN_W = 1150;
    static constexpr int WIN_H = 820;
};
