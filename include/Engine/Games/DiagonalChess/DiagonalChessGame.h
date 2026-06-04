#pragma once

// ===================================================================
// DiagonalChessGame.h — 对角象棋游戏主类
// 继承自 Game（即 Scene），是整个对角象棋游戏的核心控制器
// 组合了：棋盘、规则、AI、网络、UI、音效、粒子特效等模块
// 负责：事件处理、回合切换、走棋执行、悔棋、重开、认输、和棋
// ===================================================================

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

    void Update(float dt) override;        // 每帧更新：事件、AI、网络、粒子
    void Render() override;                // 每帧渲染：棋盘、棋子、UI、特效
    void Reset() override;                 // 重置游戏
    std::string GetName() const override { return "DiagonalChess"; }
    bool IsRunning() const override { return window.isOpen(); }

private:
    // —— 事件处理 ——
    void ProcessEvents();                  // 处理所有窗口事件
    void HandleBoardClick(int r, int c);   // 处理棋盘点击（选子/走子）
    void HandleButtonClick(float mx, float my); // 处理按钮点击

    // —— 走棋相关 ——
    void ExecuteMove(int fromR, int fromC, int toR, int toC); // 执行一步走棋
    void UndoMove();                       // 悔棋操作
    std::wstring GetMoveString(const MoveRecord& move) const; // 格式化走棋记录

    // —— 游戏控制 ——
    void RestartGame();                    // 重新开始
    void DoSurrender(Side side);           // 认输处理
    void DoDraw();                         // 和棋处理
    void CheckGameEnd();                   // 检查游戏是否结束（绝杀/困毙/和棋）
    void DoAITurn();                       // AI 走一步棋
    void ApplyUndoSteps(int steps);        // 应用悔棋步数（网络联机用）
    void SetupNetworkCallbacks();          // 设置网络事件回调

    // —— 游戏模块（组合模式）——
    DiagonalChessBoard board;              // 棋盘
    DiagonalChessRule rule;                // 规则
    DiagonalChessAI ai;                    // AI 引擎
    NetworkManager network;                // 网络管理器
    UIManager ui;                          // UI 管理器
    AudioManager audio;                    // 音效管理器
    ParticleSystem particles;              // 粒子特效系统

    // —— SFML 窗口 ——
    sf::RenderWindow window;
    sf::Font font;
    sf::View view;                        // 视图（支持窗口缩放）
    bool fontLoaded;                       // 字体是否加载成功

    // —— 游戏状态 ——
    int currentTurn;                       // 当前回合方：0=红方, 1=黑方
    int selectedR, selectedC;              // 当前选中的棋子坐标（-1=无选中）
    bool pieceSelected;                    // 是否已选中棋子
    std::vector<sf::Vector2i> validMoves;  // 选中棋子的合法走法列表
    bool gameOver;                         // 游戏是否结束
    Side winner;                           // 胜者
    bool isDrawGame;                       // 是否平局
    bool surrendered;                      // 是否通过认输结束
    bool agreedDraw;                       // 是否双方同意和棋
    float gameOverTimer;                   // 游戏结束后的计时器（用于渐显动画）
    int movesWithoutCapture;               // 无吃子步数计数器（超过 DRAW_LIMIT 则和棋）

    // —— 悔棋/走棋记录 ——
    std::stack<MoveRecord> moveHistory;    // 走棋历史栈（栈顶是最近一步）
    std::vector<std::wstring> moveLogStrings; // 走棋日志文本列表

    // —— AI 状态 ——
    bool aiMode;                           // AI 模式是否开启（本地人机对战）
    int aiSide;                            // AI 操控的阵营（0=红, 1=黑）
    bool aiThinking;                       // AI 是否正在思考中
    float aiDelayTimer;                    // AI 思考延迟计时器（给人看的等待时间）
    bool receivingMove;                    // 正在接收网络传来的走棋（防止循环发送）

    // —— 窗口常量 ——
    static constexpr int WIN_W = 1150;
    static constexpr int WIN_H = 820;
    static constexpr int MAX_LOG = 20;
    static constexpr int DRAW_LIMIT = 120;
};
