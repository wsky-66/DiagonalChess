#pragma once

// ===================================================================
// UIManager.h — UI 管理器
// 负责对角象棋游戏中的所有界面绘制
// 包括：棋盘网格、棋子、按钮、走棋日志、教程面板、通知、网络状态等
// ===================================================================

#include "Engine/Common.h"
#include "Engine/Board.h"
#include "Engine/Board/DiagonalChessBoard.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <functional>

class UIManager {
public:
    UIManager();

    // 初始化：设置窗口指针、字体和加载状态
    void Init(sf::RenderWindow& w, sf::Font& f, bool fontOk);

    // —— 绘制方法 ——
    // 绘制棋盘背景（含九宫格对角线）
    void DrawBoard(const DiagonalChessBoard& board);

    // 绘制棋子 + 选中高亮 + 合法走法标记
    // selR/selC: 当前选中的棋子坐标（-1表示无选中）
    // validMoves: 选中棋子的合法走法列表（绿色圆点/红色圈标记）
    void DrawPieces(const DiagonalChessBoard& board, int selR, int selC,
                    const std::vector<sf::Vector2i>& validMoves);

    // 绘制右侧 UI 面板（回合信息、将军提示、AI 思考提示）
    void DrawUI(int currentTurn, bool gameOver, bool isDrawGame, bool surrendered,
                Side winner, bool inCheck, bool aiThinking);

    // 绘制操作按钮（悔棋、重开、AI开关、难度、联机、教程、认输、和棋）
    void DrawButtons();

    // 绘制网络请求弹窗（悔棋请求/重开请求/认输请求/和棋请求）
    void DrawRequestPopups(bool undoRecv, bool restartRecv, bool surrRecv, bool drawRecv);

    // 绘制走棋日志（右侧可拖拽分隔线下方）
    void DrawMoveLog(const std::vector<std::wstring>& moveLogStrings);

    // 绘制网络联机 UI（房主/客户端状态、IP输入框、连接按钮）
    void DrawNetUI(bool netMode, NetState netState, const std::wstring& localIP,
                   const std::wstring& inputIP, bool showIPInput, int netSide);

    // 绘制新手教程面板（覆盖右侧，显示规则说明）
    void DrawTutorialPanel();

    // 绘制认输弹窗（本地双人对战时选择哪方认输）
    void DrawSurrenderPopup();

    // 绘制游戏结束遮罩（半透明黑底 + "绝杀"/"困毙"/"和棋"文字 + 重新开始按钮）
    void DrawGameOverEffect(float timer, bool isDraw, Side winner, bool surrendered,
                            bool checkmate, int movesWithoutCapture,
                            bool agreedDraw, bool insufficient);

    // —— 文本绘制辅助 ——
    void DrawText(const std::wstring& text, float x, float y,
                  unsigned sz, sf::Color c, bool center = false);
    void DrawTextWithShadow(const std::wstring& text, float x, float y,
                            unsigned sz, sf::Color c, bool center = false);

    // —— 鼠标悬停检测 ——
    void UpdateHover(float mx, float my);  // 检测所有按钮是否被鼠标悬停

    // —— 走棋日志滚动/拖拽 ——
    void SetDraggingLogDivider(bool v) { draggingLogDivider = v; }
    bool IsDraggingLogDivider() const { return draggingLogDivider; }
    void SetLogDividerY(float y) { logDividerY = y; }
    float GetLogDividerY() const { return logDividerY; }
    int GetLogScrollOffset() const { return logScrollOffset; }
    void SetLogScrollOffset(int v) { logScrollOffset = v; }

    // —— 面板/弹窗状态 ——
    bool IsShowTutorial() const { return showTutorial; }
    void SetShowTutorial(bool v) { showTutorial = v; }
    bool IsShowSurrenderPopup() const { return showSurrenderPopup; }
    void SetShowSurrenderPopup(bool v) { showSurrenderPopup = v; }
    bool IsShowIPInput() const { return showIPInput; }
    void SetShowIPInput(bool v) { showIPInput = v; }
    std::wstring GetInputIP() const { return inputIP; }
    void SetInputIP(const std::wstring& s) { inputIP = s; }
    void AppendInputIP(wchar_t ch) { if (inputIP.size() < 15) inputIP += ch; }
    void BackspaceInputIP() { if (!inputIP.empty()) inputIP.pop_back(); }

    // —— 通知系统 ——
    void SetNotification(const std::wstring& text, float dur) { notificationText = text; notificationTimer = dur; }
    void UpdateNotification(float dt);
    void DrawNotification();

    // —— 所有 UI 按钮 ——
    UIButton undoBtn, restartBtn, aiBtn, difficultyBtn, onlineBtn, tutorialBtn;
    UIButton surrenderBtn, drawOfferBtn;
    UIButton hostBtn, joinBtn, connectBtn, disconnectBtn;
    UIButton gameOverRestartBtn;
    UIButton undoAcceptBtn, undoRejectBtn;
    UIButton restartAcceptBtn, restartRejectBtn;
    UIButton surrenderAcceptBtn, surrenderRejectBtn;
    UIButton surrenderRedBtn, surrenderBlackBtn;
    UIButton drawAcceptBtn, drawRejectBtn;
    UIButton closeBtn;

private:
    sf::RenderWindow* window = nullptr;   // 窗口指针（不拥有所有权）
    sf::Font* font = nullptr;             // 字体指针（不拥有所有权）
    bool fontLoaded = false;              // 字体是否可用

    bool showTutorial = false;            // 是否显示教程面板
    int logScrollOffset = 0;              // 走棋日志滚动偏移
    float logDividerY = 440.f;            // 日志分隔线的 Y 坐标
    bool draggingLogDivider = false;      // 是否正在拖拽分隔线
    bool showIPInput = false;             // 是否显示 IP 输入框
    bool showSurrenderPopup = false;      // 是否显示认输弹窗
    std::wstring inputIP;                 // IP 输入框内容
    std::wstring notificationText;        // 通知文本
    float notificationTimer = 0.f;        // 通知剩余显示时间（秒）

    // —— 按钮绘制辅助方法 ——
    void DrawButton(const UIButton& btn);       // 标准按钮（棕色风格）
    void DrawSmallButton(const UIButton& btn, sf::Color fill, sf::Color outline); // 小型确认/取消按钮
    void DrawNetButton(const UIButton& btn);    // 网络按钮（绿色风格）
    void DrawNetRedButton(const UIButton& btn); // 网络断开按钮（红色风格）
};
