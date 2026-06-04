// ===================================================================
// DiagonalChessGame.cpp — 对角象棋游戏的实现
// 这是整个对角象棋游戏的核心代码，负责所有游戏逻辑
// 包含：事件处理、走棋执行、悔棋、重开、认输、和棋、AI调度、网络联机
// ===================================================================

#include "Engine/Games/DiagonalChess/DiagonalChessGame.h"
#include "Engine/Piece/DiagonalChessPiece.h"
#include <cmath>
#include <sstream>
#include <algorithm>

// ==================== 构造函数 ====================
// 创建窗口、加载字体、初始化 UI、设置网络回调
DiagonalChessGame::DiagonalChessGame()
    : window(sf::VideoMode(WIN_W, WIN_H), L"\u5BF9\u89D2\u8C61\u68CB",  // 窗口标题"对角象棋"
             sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize)  // 标题栏+关闭+可缩放
    , currentTurn(0), selectedR(-1), selectedC(-1), pieceSelected(false)  // 初始红方回合，无选中
    , gameOver(false), winner(Side::RED), isDrawGame(false)
    , surrendered(false), agreedDraw(false)
    , gameOverTimer(0.f), movesWithoutCapture(0), fontLoaded(false)
    , aiMode(false), aiSide(1), aiThinking(false)        // AI默认控制黑方
    , aiDelayTimer(0.f), receivingMove(false)
{
    window.setFramerateLimit(60);                        // 限制 60 FPS
    view = window.getDefaultView();                       // 保存默认视图（支持缩放）

    // 加载中文字体（按优先级：黑体 > 微软雅黑 > 宋体）
    if (font.loadFromFile("C:/Windows/Fonts/simhei.ttf") ||
        font.loadFromFile("C:/Windows/Fonts/msyh.ttc") ||
        font.loadFromFile("C:/Windows/Fonts/simsun.ttc")) {
        fontLoaded = true;
    }
    ui.Init(window, font, fontLoaded);                   // 初始化 UI 管理器
    SetupNetworkCallbacks();                             // 设置网络事件回调
}

// 析构函数：断开网络连接
DiagonalChessGame::~DiagonalChessGame() { network.Disconnect(); }

// ==================== 设置网络事件回调 ====================
// 将 NetworkManager 的事件回调与游戏逻辑绑定
void DiagonalChessGame::SetupNetworkCallbacks() {
    // 收到对方走棋 → 执行走棋
    network.onMoveReceived = [this](int fr, int fc, int tr, int tc) {
        if (gameOver) return;
        if (network.undoRequestSent || network.undoRequestReceived ||
            network.restartRequestSent || network.restartRequestReceived ||
            network.surrenderRequestSent || network.surrenderRequestReceived ||
            network.drawRequestSent || network.drawRequestReceived) return;
        receivingMove = true;                            // 标记为接收中，防止循环发送
        ExecuteMove(fr, fc, tr, tc);
        receivingMove = false;
    };

    // 悔棋同意 → 执行悔棋并告知对方悔了几步
    network.onUndoAccepted = [this]() {
        if (gameOver || moveHistory.empty()) return;
        int steps = (moveHistory.top().side == static_cast<Side>(network.GetNetSide())) ? 1 : 2;
        ApplyUndoSteps(steps);
        network.SendUndoAck(steps);
    };

    // 收到悔棋确认 → 也执行相同步数的悔棋
    network.onUndoAckReceived = [this](int steps) {
        if (gameOver) return;
        ApplyUndoSteps(steps);
    };

    // 重开/认输/和棋 同意
    network.onRestartAccepted = [this]() { if (!gameOver) RestartGame(); };
    network.onSurrenderAccepted = [this]() { if (!gameOver) DoSurrender(network.GetNetSide()); };
    network.onDrawAccepted = [this]() { if (!gameOver) DoDraw(); };

    // 被拒绝 → 显示通知
    network.onRejected = [this](const std::wstring& msg) { ui.SetNotification(msg, 3.f); };

    // 对方断开 → 重置网络状态
    network.onDisconnected = [this]() {
        network.Disconnect();
        ui.SetLogDividerY(440.f);
        ui.onlineBtn.label = L"\u8054\u673A: \u5173";    // "联机: 关"
    };
}

// ==================== 每帧更新 ====================
void DiagonalChessGame::Update(float dt) {
    ProcessEvents();                                     // 处理窗口事件
    particles.Update(dt);                                // 更新粒子特效
    ui.UpdateNotification(dt);                           // 更新通知计时器
    if (gameOver) gameOverTimer += dt;                   // 游戏结束后累积计时
    network.PollNetwork();                               // 网络轮询

    // 检查是否有待处理的网络请求（会影响按钮状态）
    bool any = network.undoRequestSent || network.undoRequestReceived ||
               network.restartRequestSent || network.restartRequestReceived ||
               network.surrenderRequestSent || network.surrenderRequestReceived ||
               network.drawRequestSent || network.drawRequestReceived;

    // 根据网络请求状态动态更新按钮文字和禁用状态
    ui.undoBtn.disabled = network.undoRequestSent || network.undoRequestReceived;
    ui.undoBtn.label = network.undoRequestSent ? L"\u7B49\u5F85\u540C\u610F..." : L"\u6094\u68CB";  // "等待同意..." / "悔棋"

    ui.restartBtn.disabled = network.restartRequestSent || network.restartRequestReceived;
    ui.restartBtn.label = network.restartRequestSent ? L"\u7B49\u5F85\u540C\u610F..." : L"\u91CD\u65B0\u5F00\u59CB";

    ui.surrenderBtn.disabled = network.surrenderRequestSent || network.surrenderRequestReceived ||
                                gameOver || network.GetState() == NetState::HOST_WAITING;
    ui.surrenderBtn.label = network.surrenderRequestSent ? L"\u7B49\u5F85\u540C\u610F..." : L"\u8BA4\u8F93";

    ui.drawOfferBtn.disabled = network.drawRequestSent || network.drawRequestReceived ||
                                gameOver || network.GetState() == NetState::HOST_WAITING;
    ui.drawOfferBtn.label = network.drawRequestSent ? L"\u7B49\u5F85\u540C\u610F..." : L"\u548C\u68CB";

    // AI 走棋逻辑（仅在人机模式 + 不在联机模式 + 轮到AI + 游戏未结束时触发）
    if (aiMode && !network.IsNetMode() && currentTurn == aiSide && !gameOver) {
        aiDelayTimer += dt;                              // 累积计时（让人看到 AI 在"思考"）
        aiThinking = true;
        if (aiDelayTimer >= 1.5f) {                      // 等1.5秒后 AI 走棋
            DoAITurn();
            aiThinking = false;
            aiDelayTimer = 0.f;
        }
    } else { aiThinking = false; aiDelayTimer = 0.f; }
}

// ==================== 渲染 ====================
void DiagonalChessGame::Render() {
    window.setView(view);                                // 应用视图（支持缩放）
    window.clear(sf::Color(40, 32, 22));                 // 深色背景

    // 左右分栏背景
    sf::RectangleShape lp(sf::Vector2f(700, WIN_H)); lp.setPosition(0, 0);
    lp.setFillColor(sf::Color(55, 42, 28)); window.draw(lp);
    sf::RectangleShape rp(sf::Vector2f(450, WIN_H)); rp.setPosition(700, 0);
    rp.setFillColor(sf::Color(45, 35, 24)); window.draw(rp);
    sf::RectangleShape dv(sf::Vector2f(2, WIN_H)); dv.setPosition(699, 0);
    dv.setFillColor(sf::Color(80, 65, 45)); window.draw(dv);

    // 绘制棋盘和棋子
    ui.DrawBoard(board);
    ui.DrawPieces(board, selectedR, selectedC, validMoves);

    // 绘制 UI 面板
    bool inCheck = rule.IsInCheckRaw(board, currentTurn);
    ui.DrawUI(currentTurn, gameOver, isDrawGame, surrendered, winner, inCheck, aiThinking);
    ui.DrawButtons();
    ui.DrawRequestPopups(network.undoRequestReceived, network.restartRequestReceived,
                         network.surrenderRequestReceived, network.drawRequestReceived);
    ui.DrawNotification();

    // 走棋日志分隔线（可拖拽）
    float dy = ui.GetLogDividerY();
    sf::RectangleShape ld(sf::Vector2f(420, 6)); ld.setPosition(715, dy - 3);
    ld.setFillColor(ui.IsDraggingLogDivider() ? sf::Color(150, 120, 80) : sf::Color(90, 70, 45));
    window.draw(ld);

    // 各种面板和弹窗
    if (ui.IsShowTutorial()) ui.DrawTutorialPanel();
    ui.DrawNetUI(network.IsNetMode(), network.GetState(), network.GetLocalIP(),
                 ui.GetInputIP(), ui.IsShowIPInput(), (int)network.GetNetSide());
    ui.DrawMoveLog(moveLogStrings);
    if (ui.IsShowSurrenderPopup()) ui.DrawSurrenderPopup();

    // 游戏结束遮罩
    if (gameOver) {
        bool cm = rule.IsCheckmate(board, (winner == Side::RED) ? 1 : 0);
        bool ins = rule.HasInsufficientMaterial(board);
        ui.DrawGameOverEffect(gameOverTimer, isDrawGame, winner, surrendered, cm,
                              movesWithoutCapture, agreedDraw, ins);
    }

    particles.Draw(window);                              // 粒子特效在最上层
    window.display();                                    // 将画面显示到屏幕
}

// ==================== 事件处理 ====================
void DiagonalChessGame::ProcessEvents() {
    sf::Event e;
    while (window.pollEvent(e)) {
        // 关闭窗口
        if (e.type == sf::Event::Closed) { window.close(); return; }

        // 窗口缩放 → 调整 view 保持画面比例
        if (e.type == sf::Event::Resized) {
            float ratio = (float)e.size.height / e.size.width;
            float base = (float)WIN_H / WIN_W;           // 基准宽高比
            if (ratio > base) {                          // 上下加黑边
                float nh = e.size.width * base;
                view.setViewport(sf::FloatRect(0, (1.f - nh / e.size.height) / 2.f, 1.f, nh / e.size.height));
            } else {                                     // 左右加黑边
                float nw = e.size.height / base;
                view.setViewport(sf::FloatRect((1.f - nw / e.size.width) / 2.f, 0, nw / e.size.width, 1.f));
            }
        }

        // 鼠标左键按下
        if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
            auto wp = window.mapPixelToCoords(sf::Vector2i(e.mouseButton.x, e.mouseButton.y), view);
            float mx = wp.x, my = wp.y, ly = ui.GetLogDividerY();

            // 拖拽走棋日志分隔线
            if (mx >= 715 && mx <= 1135 && my >= ly - 8 && my <= ly + 8) {
                ui.SetDraggingLogDivider(true);
            }
            // 游戏结束后点击"重新开始"按钮
            else if (gameOver && gameOverTimer > 0.5f && ui.gameOverRestartBtn.bounds.contains(mx, my)) {
                RestartGame();
            }
            // 认输弹窗中的操作
            else if (ui.IsShowSurrenderPopup() && !aiMode && network.GetState() != NetState::CONNECTED) {
                if (ui.surrenderRedBtn.bounds.contains(mx, my)) {
                    DoSurrender(Side::RED); ui.SetShowSurrenderPopup(false);
                } else if (ui.surrenderBlackBtn.bounds.contains(mx, my)) {
                    DoSurrender(Side::BLACK); ui.SetShowSurrenderPopup(false);
                } else if (mx < 405 || mx > 745 || my < 310 || my > 510) {
                    ui.SetShowSurrenderPopup(false);     // 点击外部取消
                }
            }
            // 右侧按钮区域
            else if (mx >= 700) {
                HandleButtonClick(mx, my);
            }
            // 棋盘区域
            else {
                auto g = board.ScreenToGrid(mx, my);
                if (g.x >= 0 && g.x < 9 && g.y >= 0 && g.y < 9)
                    HandleBoardClick(g.x, g.y);
            }
        }

        // 鼠标滚轮 → 滚动手棋日志
        if (e.type == sf::Event::MouseWheelScrolled) {
            auto wp = window.mapPixelToCoords(
                sf::Vector2i(e.mouseWheelScroll.x, e.mouseWheelScroll.y), view);
            if ((int)wp.x >= 715 && (int)wp.x <= 1135)
                ui.SetLogScrollOffset(ui.GetLogScrollOffset() - (int)e.mouseWheelScroll.delta);
        }

        // 鼠标释放 → 停止拖拽分隔线
        if (e.type == sf::Event::MouseButtonReleased) { ui.SetDraggingLogDivider(false); }

        // 鼠标移动 → 更新分隔线位置或更新按钮悬停
        if (e.type == sf::Event::MouseMoved) {
            auto wp = window.mapPixelToCoords(sf::Vector2i(e.mouseMove.x, e.mouseMove.y), view);
            if (ui.IsDraggingLogDivider())
                ui.SetLogDividerY(std::max(395.f, std::min(wp.y, 740.f)));  // 限制分隔线范围
            ui.UpdateHover(wp.x, wp.y);
        }

        // 文本输入 → IP 地址输入（联机加入房间时）
        if (e.type == sf::Event::TextEntered && ui.IsShowIPInput()) {
            if (e.text.unicode == 8) ui.BackspaceInputIP();              // 退格键
            else if (e.text.unicode < 128 && e.text.unicode != 13)       // ASCII可打印字符
                ui.AppendInputIP((wchar_t)e.text.unicode);
            else if (e.text.unicode == 13) {                              // 回车键 → 连接
                if (!ui.GetInputIP().empty()) {
                    network.StartClient(ui.GetInputIP()); ui.SetShowIPInput(false);
                }
            }
        }
    }
}

// ==================== 棋盘点击处理 ====================
void DiagonalChessGame::HandleBoardClick(int r, int c) {
    if (gameOver) return;
    // AI 模式：轮到 AI 时不响应点击
    if (aiMode && !network.IsNetMode() && currentTurn == aiSide) return;
    // 联机模式：不是自己的回合时不响应点击
    if (network.GetState() == NetState::CONNECTED && currentTurn != (int)network.GetNetSide()) return;
    // 有待处理的网络请求时不响应点击
    if (network.undoRequestSent || network.undoRequestReceived ||
        network.restartRequestSent || network.restartRequestReceived ||
        network.surrenderRequestSent || network.surrenderRequestReceived ||
        network.drawRequestSent || network.drawRequestReceived) return;

    if (!pieceSelected) {
        // 第一次点击：选择棋子（必须是己方棋子）
        if (board.IsOccupied(r, c) && board.At(r, c).GetSide() == currentTurn) {
            selectedR = r; selectedC = c; pieceSelected = true;
            validMoves = rule.GetValidMoves(board, r, c);  // 计算合法走法
        }
    } else {
        // 第二次点击：执行走棋
        if (r == selectedR && c == selectedC) {           // 点同一个位置 → 取消选择
            pieceSelected = false; validMoves.clear(); return;
        }
        if (board.IsOccupied(r, c) && board.At(r, c).GetSide() == currentTurn) {
            selectedR = r; selectedC = c;                // 点击其他己方棋子 → 切换选择
            validMoves = rule.GetValidMoves(board, r, c); return;
        }
        // 点击合法走法标记的位置 → 执行走棋
        for (const auto& m : validMoves) {
            if (m.x == r && m.y == c) { ExecuteMove(selectedR, selectedC, r, c); break; }
        }
        pieceSelected = false; validMoves.clear();
    }
}

// ==================== 按钮点击处理 ====================
void DiagonalChessGame::HandleButtonClick(float mx, float my) {
    // 关闭按钮 → 返回主菜单
    if (ui.closeBtn.bounds.contains(mx, my)) {
        audio.PlayClickSound(); window.close(); return;
    }

    // 教程面板关闭按钮
    if (ui.IsShowTutorial() && mx >= 1090 && mx <= 1140 && my >= 4 && my <= 32) {
        ui.SetShowTutorial(false); audio.PlayClickSound(); return;
    }

    bool uR = network.undoRequestReceived, rR = network.restartRequestReceived;
    bool sR = network.surrenderRequestReceived, dR = network.drawRequestReceived;

    // 网络请求弹窗的"同意/拒绝"按钮
    if (ui.undoAcceptBtn.bounds.contains(mx, my) && uR) {
        if (gameOver) { network.undoRequestReceived = false; return; }
        network.SendUndoAccept(); network.undoRequestReceived = false; audio.PlayClickSound();
    } else if (ui.undoRejectBtn.bounds.contains(mx, my) && uR) {
        network.SendUndoReject(); network.undoRequestReceived = false; audio.PlayClickSound();
    } else if (ui.surrenderAcceptBtn.bounds.contains(mx, my) && sR) {
        if (gameOver) { network.surrenderRequestReceived = false; return; }
        network.SendSurrenderResponse(true); network.surrenderRequestReceived = false;
        DoSurrender(network.surrenderRequesterSide); audio.PlayClickSound();
    } else if (ui.surrenderRejectBtn.bounds.contains(mx, my) && sR) {
        network.SendSurrenderResponse(false); network.surrenderRequestReceived = false; audio.PlayClickSound();
    } else if (ui.drawAcceptBtn.bounds.contains(mx, my) && dR) {
        if (gameOver) { network.drawRequestReceived = false; return; }
        network.SendDrawResponse(true); network.drawRequestReceived = false;
        DoDraw(); audio.PlayClickSound();
    } else if (ui.drawRejectBtn.bounds.contains(mx, my) && dR) {
        network.SendDrawResponse(false); network.drawRequestReceived = false; audio.PlayClickSound();
    } else if (ui.restartAcceptBtn.bounds.contains(mx, my) && rR) {
        if (gameOver) { network.restartRequestReceived = false; return; }
        network.SendRestartAccept(); network.restartRequestReceived = false;
        RestartGame(); network.SendRestartAck(); audio.PlayClickSound();
    } else if (ui.restartRejectBtn.bounds.contains(mx, my) && rR) {
        network.SendRestartReject(); network.restartRequestReceived = false; audio.PlayClickSound();

    // 悔棋按钮
    } else if (ui.undoBtn.bounds.contains(mx, my) && !ui.undoBtn.disabled) {
        if (network.GetState() == NetState::CONNECTED) {
            if (!network.undoRequestSent && moveHistory.size() >= 1) {
                network.SendUndoRequest(); network.undoRequestSent = true;
            }
        } else { UndoMove(); }
        audio.PlayClickSound();

    // 重开按钮
    } else if (ui.restartBtn.bounds.contains(mx, my) && !ui.restartBtn.disabled) {
        if (network.GetState() == NetState::CONNECTED) {
            if (!network.restartRequestSent) {
                network.SendRestartRequest(); network.restartRequestSent = true;
            }
        } else { RestartGame(); }
        audio.PlayClickSound();

    // AI 开关
    } else if (ui.aiBtn.bounds.contains(mx, my) && !ui.aiBtn.disabled) {
        aiMode = !aiMode;
        ui.aiBtn.label = aiMode ? L"AI: \u5F00" : L"AI: \u5173";  // "AI: 开" / "AI: 关"
        audio.PlayClickSound();

    // 难度切换（简单→中等→困难→简单...）
    } else if (ui.difficultyBtn.bounds.contains(mx, my) && !ui.difficultyBtn.disabled) {
        auto d = ai.GetDifficulty();
        if (d == AIDifficulty::EASY) {
            ai.SetDifficulty(AIDifficulty::MEDIUM); ui.difficultyBtn.label = L"\u4E2D\u7B49";  // "中等"
        } else if (d == AIDifficulty::MEDIUM) {
            ai.SetDifficulty(AIDifficulty::HARD); ui.difficultyBtn.label = L"\u56F0\u96BE";    // "困难"
        } else {
            ai.SetDifficulty(AIDifficulty::EASY); ui.difficultyBtn.label = L"\u7B80\u5355";    // "简单"
        }
        audio.PlayClickSound();

    // 联机开关
    } else if (ui.onlineBtn.bounds.contains(mx, my) && !ui.onlineBtn.disabled) {
        bool nm = !network.IsNetMode(); network.SetNetMode(nm);
        ui.onlineBtn.label = nm ? L"\u8054\u673A: \u5F00" : L"\u8054\u673A: \u5173";  // "联机: 开/关"
        if (!nm) { network.Disconnect(); ui.SetShowIPInput(false); ui.SetLogDividerY(440.f); }
        else ui.SetLogDividerY(550.f);
        audio.PlayClickSound();

    // 创建房间（房主）
    } else if (ui.hostBtn.bounds.contains(mx, my) && network.IsNetMode()
               && network.GetState() == NetState::OFFLINE) {
        network.StartHost(); RestartGame(); audio.PlayClickSound();

    // 加入房间 → 显示 IP 输入框
    } else if (ui.joinBtn.bounds.contains(mx, my) && network.IsNetMode()
               && network.GetState() == NetState::OFFLINE) {
        ui.SetShowIPInput(!ui.IsShowIPInput()); audio.PlayClickSound();

    // 连接按钮（IP 输入后的确认）
    } else if (ui.connectBtn.bounds.contains(mx, my) && ui.IsShowIPInput()
               && !ui.GetInputIP().empty()) {
        network.StartClient(ui.GetInputIP());
        if (network.GetState() == NetState::CONNECTED) {
            aiMode = false; ui.aiBtn.label = L"AI: \u5173\u95ED"; Reset();  // "AI: 关闭"
        }
        ui.SetShowIPInput(false); audio.PlayClickSound();

    // 断开连接
    } else if (ui.disconnectBtn.bounds.contains(mx, my)
               && network.GetState() != NetState::OFFLINE) {
        network.Disconnect(); ui.SetLogDividerY(440.f); audio.PlayClickSound();

    // 教程面板
    } else if (ui.tutorialBtn.bounds.contains(mx, my) && !ui.tutorialBtn.disabled) {
        ui.SetShowTutorial(!ui.IsShowTutorial()); audio.PlayClickSound();

    // 认输按钮
    } else if (ui.surrenderBtn.bounds.contains(mx, my) && !ui.surrenderBtn.disabled) {
        if (aiMode) DoSurrender(Side::RED);
        else if (network.GetState() == NetState::CONNECTED) {
            if (!network.surrenderRequestSent) {
                network.SendSurrenderRequest();
                network.surrenderRequestSent = true;
                network.surrenderRequesterSide = network.GetNetSide();
            }
        } else ui.SetShowSurrenderPopup(true);           // 本地模式：弹出"谁认输"选择框
        audio.PlayClickSound();

    // 和棋按钮
    } else if (ui.drawOfferBtn.bounds.contains(mx, my) && !ui.drawOfferBtn.disabled) {
        if (network.GetState() == NetState::CONNECTED) {
            if (network.drawRequestReceived) {           // 对方已请求和棋 → 直接同意
                network.SendDrawResponse(true);
                network.drawRequestReceived = false;
                DoDraw();
            } else if (!network.drawRequestSent) {
                network.SendDrawRequest(); network.drawRequestSent = true;
            }
        } else DoDraw();                                 // 本地模式直接和棋
        audio.PlayClickSound();
    }
}

// ==================== 执行走棋 ====================
void DiagonalChessGame::ExecuteMove(int fr, int fc, int tr, int tc) {
    // 创建走棋记录
    MoveRecord rec;
    rec.fromR = fr; rec.fromC = fc; rec.toR = tr; rec.toC = tc;
    rec.movedType = (int)board.At(fr, fc).GetDType();
    rec.movedSide = board.At(fr, fc).GetSide();
    rec.capturedType = board.IsOccupied(tr, tc) ? (int)board.At(tr, tc).GetDType() : 0;
    rec.capturedSide = board.IsOccupied(tr, tc) ? board.At(tr, tc).GetSide() : -1;
    rec.side = static_cast<Side>(currentTurn);
    rec.prevMovesWithoutCapture = movesWithoutCapture;

    bool captured = board.IsOccupied(tr, tc);

    // 播放音效
    if (captured) audio.PlayCaptureSound(); else audio.PlayMoveSound();

    // 如果吃掉了对方的将/帅 → 直接获胜
    if (captured && board.At(tr, tc).GetDType() == DChessPieceType::GENERAL) {
        gameOver = true; winner = static_cast<Side>(currentTurn); gameOverTimer = 0.f;
        particles.CreateWinParticles(static_cast<Side>(currentTurn));
        if (currentTurn == 0) audio.PlayWinSound(); else audio.PlayLoseSound();
    }

    // 在棋盘上移动棋子
    board.MovePieceInternal(fr, fc, tr, tc);

    // 更新无吃子计数器
    if (captured) movesWithoutCapture = 0;
    else movesWithoutCapture++;

    // 记录走棋历史
    moveHistory.push(rec);
    std::wstring moveStr = GetMoveString(rec);
    moveLogStrings.push_back(moveStr);
    // 限制日志条目数（避免无限增长）
    if ((int)moveLogStrings.size() > MAX_LOG * 2) {
        moveLogStrings.erase(moveLogStrings.begin());
    }

    // 联机时发送走棋给对方
    if (network.GetState() == NetState::CONNECTED && !receivingMove) {
        network.SendMove(fr, fc, tr, tc);
    }

    // 切换回合
    currentTurn = (currentTurn == 0) ? 1 : 0;

    // 检查游戏是否因困毙/和棋而结束
    if (!gameOver) CheckGameEnd();
}

// ==================== 悔棋 ====================
void DiagonalChessGame::UndoMove() {
    if (moveHistory.empty()) return;
    if (gameOver) {                                      // 游戏结束后悔棋 → 恢复游戏
        gameOver = false; isDrawGame = false; gameOverTimer = 0.f; particles.Clear();
    }

    // 弹出一步走棋记录的 lambda
    auto popOne = [this]() {
        auto rec = moveHistory.top(); moveHistory.pop();
        // 恢复初始位置的棋子
        board.At(rec.fromR, rec.fromC) = DiagonalChessPiece(
            (DChessPieceType)rec.movedType, rec.movedSide);
        board.OccupiedCell(rec.fromR, rec.fromC);
        // 如果吃掉了棋子，恢复被吃的棋子
        if (rec.capturedSide >= 0) {
            board.At(rec.toR, rec.toC) = DiagonalChessPiece(
                (DChessPieceType)rec.capturedType, rec.capturedSide);
            board.OccupiedCell(rec.toR, rec.toC);
        } else {
            board.ClearCell(rec.toR, rec.toC);           // 否则清空目标位置
        }
        currentTurn = (int)rec.side;                      // 恢复回合
        if (!moveLogStrings.empty()) moveLogStrings.pop_back();
    };

    // AI 模式：悔棋需要撤回两步（AI的一步 + 玩家的一步）
    if (aiMode) {
        if (moveHistory.size() < 2) return;
        popOne(); popOne();
        movesWithoutCapture = moveHistory.empty() ? 0 : moveHistory.top().prevMovesWithoutCapture;
        moveLogStrings.push_back(L"\u6094\u68CB");       // 记录"悔棋"
    } else {
        auto rec = moveHistory.top();
        popOne();
        movesWithoutCapture = rec.prevMovesWithoutCapture;
        moveLogStrings.push_back(L"\u6094\u68CB");
    }
    pieceSelected = false; validMoves.clear();
    ui.SetShowSurrenderPopup(false);
}

// ==================== 重新开始 ====================
void DiagonalChessGame::RestartGame() {
    board.Reset();                                       // 棋盘恢复初始布局
    currentTurn = 0; pieceSelected = false; validMoves.clear();
    gameOver = false; isDrawGame = false; surrendered = false; agreedDraw = false;
    gameOverTimer = 0.f; movesWithoutCapture = 0;
    moveLogStrings.clear();
    moveLogStrings.push_back(L"\u91CD\u65B0\u5F00\u59CB");  // 记录"重新开始"
    while (!moveHistory.empty()) moveHistory.pop();
    particles.Clear(); network.ResetRequests();
    ui.SetShowSurrenderPopup(false);
}

// ==================== 认输 ====================
void DiagonalChessGame::DoSurrender(Side side) {
    ui.SetShowSurrenderPopup(false);
    gameOver = true;
    winner = (side == Side::RED) ? Side::BLACK : Side::RED;  // 对方获胜
    isDrawGame = false; surrendered = true; gameOverTimer = 0.f;
    moveLogStrings.push_back((side == Side::RED) ? L"\u7EA2\u65B9\u8BA4\u8F93" : L"\u9ED1\u65B9\u8BA4\u8F93");
    particles.CreateWinParticles(winner);
    if (winner == Side::RED) audio.PlayWinSound(); else audio.PlayLoseSound();
}

// ==================== 和棋 ====================
void DiagonalChessGame::DoDraw() {
    gameOver = true; isDrawGame = true; surrendered = false; agreedDraw = true; gameOverTimer = 0.f;
    moveLogStrings.push_back(L"\u548C\u68CB");
    particles.CreateDrawParticles(); audio.PlayDrawSound();
}

// ==================== 检查游戏是否结束 ====================
// 在每步走棋后调用，检查触发条件
void DiagonalChessGame::CheckGameEnd() {
    ui.SetShowSurrenderPopup(false);

    // 120 步无吃子 → 自然限着和棋
    if (movesWithoutCapture >= DRAW_LIMIT) {
        gameOver = true; isDrawGame = true; gameOverTimer = 0.f;
        particles.CreateDrawParticles(); audio.PlayDrawSound(); return;
    }

    // 子力不足 → 和棋
    if (rule.HasInsufficientMaterial(board)) {
        gameOver = true; isDrawGame = true; gameOverTimer = 0.f;
        particles.CreateDrawParticles(); audio.PlayDrawSound(); return;
    }

    // 当前回合方无攻击力 → 困毙（对方获胜）
    if (rule.SideHasNoAttack(board, currentTurn)) {
        gameOver = true; isDrawGame = false; gameOverTimer = 0.f;
        winner = (currentTurn == 0) ? Side::BLACK : Side::RED;
        particles.CreateWinParticles(winner);
        if (winner == Side::RED) audio.PlayWinSound(); else audio.PlayLoseSound();
        return;
    }

    // 当前回合方无合法走法 → 困毙或绝杀（对方获胜）
    if (!rule.HasLegalMovesRaw(board, currentTurn)) {
        gameOver = true; isDrawGame = false; gameOverTimer = 0.f;
        winner = (currentTurn == 0) ? Side::BLACK : Side::RED;
        particles.CreateWinParticles(winner);
        if (winner == Side::RED) audio.PlayWinSound(); else audio.PlayLoseSound();
    }
}

// ==================== AI 走棋 ====================
void DiagonalChessGame::DoAITurn() {
    auto best = ai.Think(board, aiSide, rule);           // AI 思考（Minimax搜索）
    if (best.fromR < 0) { CheckGameEnd(); return; }      // 无合法走法 → 检查游戏结束
    ExecuteMove(best.fromR, best.fromC, best.toR, best.toC);
}

// ==================== 应用悔棋步数（网络联机用）====================
void DiagonalChessGame::ApplyUndoSteps(int steps) {
    if ((int)moveHistory.size() < steps) return;
    for (int i = 0; i < steps; i++) {
        auto rec = moveHistory.top(); moveHistory.pop();
        board.At(rec.fromR, rec.fromC) = DiagonalChessPiece(
            (DChessPieceType)rec.movedType, rec.movedSide);
        board.OccupiedCell(rec.fromR, rec.fromC);
        if (rec.capturedSide >= 0) {
            board.At(rec.toR, rec.toC) = DiagonalChessPiece(
                (DChessPieceType)rec.capturedType, rec.capturedSide);
            board.OccupiedCell(rec.toR, rec.toC);
        } else { board.ClearCell(rec.toR, rec.toC); }
        currentTurn = (int)rec.side;
        if (!moveLogStrings.empty()) moveLogStrings.pop_back();
    }
    if (!moveHistory.empty()) movesWithoutCapture = moveHistory.top().prevMovesWithoutCapture;
    else movesWithoutCapture = 0;
    pieceSelected = false; validMoves.clear();
    if (gameOver) { gameOver = false; isDrawGame = false; gameOverTimer = 0.f; particles.Clear(); }
}

// ==================== 格式化走棋记录字符串 ====================
// 例如：红车 (0,4)→(3,4)  或  黑马 (8,3)→(6,2) 吃兵
std::wstring DiagonalChessGame::GetMoveString(const MoveRecord& rec) const {
    std::wostringstream oss;
    std::wstring sideStr = (rec.side == Side::RED) ? L"\u7EA2" : L"\u9ED1";  // "红" / "黑"
    auto sym = DChessGetSymbol((DChessPieceType)rec.movedType, rec.movedSide);
    oss << sideStr << sym << L" (" << rec.fromR << L"," << rec.fromC << L")\u2192("
        << rec.toR << L"," << rec.toC << L")";           // "→"
    if (rec.capturedSide >= 0) {
        auto cs = DChessGetSymbol((DChessPieceType)rec.capturedType, rec.capturedSide);
        oss << L" \u5403" << cs;                         // "吃"
    }
    return oss.str();
}

// 重置游戏（委托给 RestartGame）
void DiagonalChessGame::Reset() { RestartGame(); }
