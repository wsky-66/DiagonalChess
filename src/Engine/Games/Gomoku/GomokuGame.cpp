#include "Engine/Games/Gomoku/GomokuGame.h"
#include <sstream>

GomokuGame::GomokuGame()
    : window(sf::VideoMode(WIN_W, WIN_H), L"\u4E94\u5B50\u68CB",
             sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize)
    , currentTurn(1)
    , gameOver(false), winner(0), isDraw(false), surrendered(false)
    , hoverR(-1), hoverC(-1), mouseOnBoard(false)
    , gameOverTimer(0.f)
{
    window.setFramerateLimit(60);

    if (font.loadFromFile("C:/Windows/Fonts/simhei.ttf") ||
        font.loadFromFile("C:/Windows/Fonts/msyh.ttc") ||
        font.loadFromFile("C:/Windows/Fonts/simsun.ttc")) {
        fontLoaded = true;
    }

    // 初始化按钮位置
    restartBtn.bounds    = sf::FloatRect(745, 160, 160, 44);
    restartBtn.label     = L"\u91CD\u65B0\u5F00\u59CB";
    restartBtn.hovered   = false;

    undoBtn.bounds       = sf::FloatRect(745, 220, 160, 44);
    undoBtn.label        = L"\u6094\u68CB";
    undoBtn.hovered      = false;

    surrenderBtn.bounds  = sf::FloatRect(745, 280, 160, 44);
    surrenderBtn.label   = L"\u8BA4\u8F93";
    surrenderBtn.hovered = false;

    gameOverRestartBtn.bounds  = sf::FloatRect(0, 0, 200, 50);
    gameOverRestartBtn.label   = L"\u91CD\u65B0\u5F00\u59CB";
    gameOverRestartBtn.hovered = false;
}

GomokuGame::~GomokuGame() {}

void GomokuGame::Update(float dt) {
    ProcessEvents();
    if (gameOver)
        gameOverTimer += dt;
}

void GomokuGame::Render() {
    window.clear(sf::Color(40, 32, 22));

    // 左侧面板（棋盘区域）
    sf::RectangleShape lp(sf::Vector2f(700.f, WIN_H));
    lp.setPosition(0, 0);
    lp.setFillColor(sf::Color(55, 42, 28));
    window.draw(lp);

    // 右侧面板
    sf::RectangleShape rp(sf::Vector2f(450.f, WIN_H));
    rp.setPosition(700, 0);
    rp.setFillColor(sf::Color(45, 35, 24));
    window.draw(rp);

    // 分隔线
    sf::RectangleShape dv(sf::Vector2f(2, WIN_H));
    dv.setPosition(699, 0);
    dv.setFillColor(sf::Color(80, 65, 45));
    window.draw(dv);

    DrawBoard();
    DrawStones();
    DrawUI();

    if (gameOver)
        DrawGameOver();

    window.display();
}

// ==================== 事件处理 ====================

void GomokuGame::ProcessEvents() {
    sf::Event e;
    while (window.pollEvent(e)) {
        if (e.type == sf::Event::Closed) {
            window.close();
            return;
        }

        if (e.type == sf::Event::MouseButtonPressed &&
            e.mouseButton.button == sf::Mouse::Left) {
            float mx = (float)e.mouseButton.x;
            float my = (float)e.mouseButton.y;

            // 游戏结束按钮
            if (gameOver && gameOverTimer > 0.5f &&
                gameOverRestartBtn.bounds.contains(mx, my)) {
                RestartGame();
            }
            // 右侧按钮
            else if (mx >= 700) {
                HandleButtonClick(mx, my);
            }
            // 棋盘区域
            else {
                auto g = board.ScreenToGrid(mx, my);
                if (g.x >= 0 && g.x < GomokuBoard::SIZE &&
                    g.y >= 0 && g.y < GomokuBoard::SIZE)
                    HandleBoardClick(g.x, g.y);
            }
        }

        if (e.type == sf::Event::MouseMoved) {
            float mx = (float)e.mouseMove.x;
            float my = (float)e.mouseMove.y;

            // 更新按钮悬停
            restartBtn.hovered = restartBtn.bounds.contains(mx, my);
            undoBtn.hovered = undoBtn.bounds.contains(mx, my);
            surrenderBtn.hovered = surrenderBtn.bounds.contains(mx, my);
            if (gameOver)
                gameOverRestartBtn.hovered = gameOverRestartBtn.bounds.contains(mx, my);

            // 更新棋盘悬停
            if (mx < 700) {
                auto g = board.ScreenToGrid(mx, my);
                if (g.x >= 0 && g.x < GomokuBoard::SIZE &&
                    g.y >= 0 && g.y < GomokuBoard::SIZE) {
                    hoverR = g.x;
                    hoverC = g.y;
                    mouseOnBoard = true;
                } else {
                    mouseOnBoard = false;
                }
            } else {
                mouseOnBoard = false;
            }
        }
    }
}

void GomokuGame::HandleBoardClick(int r, int c) {
    if (gameOver) return;
    if (!board.IsEmpty(r, c)) return; // 已有棋子，不可覆盖
    PlaceStone(r, c);
}

void GomokuGame::HandleButtonClick(float mx, float my) {
    if (restartBtn.bounds.contains(mx, my)) {
        RestartGame();
    } else if (undoBtn.bounds.contains(mx, my)) {
        UndoMove();
    } else if (surrenderBtn.bounds.contains(mx, my)) {
        DoSurrender();
    }
}

// ==================== 落子 ====================

void GomokuGame::PlaceStone(int r, int c) {
    board.PlaceStone(r, c, currentTurn);

    // 记录悔棋信息
    moveHistory.push({r, c, currentTurn});

    // 检测游戏结束
    CheckGameEnd(r, c);

    // 切换回合
    if (!gameOver)
        currentTurn = (currentTurn == 1) ? 2 : 1;
}

void GomokuGame::CheckGameEnd(int r, int c) {
    // 检测落子方是否五连获胜
    if (rule.CheckWin(board, r, c, currentTurn)) {
        gameOver = true;
        winner = currentTurn;
        isDraw = false;
        gameOverTimer = 0.f;
        return;
    }

    // 检测平局（棋盘已满）
    if (rule.IsBoardFull(board)) {
        gameOver = true;
        isDraw = true;
        winner = 0;
        gameOverTimer = 0.f;
    }
}

// ==================== 悔棋 ====================

void GomokuGame::UndoMove() {
    if (moveHistory.empty() || gameOver) return;

    auto rec = moveHistory.top();
    moveHistory.pop();
    board.RemoveStone(rec.r, rec.c);

    // 如果游戏已结束，撤回可以恢复游戏
    if (moveHistory.empty()) {
        currentTurn = 1;
    } else {
        currentTurn = moveHistory.top().side;
    }
    gameOver = false;
    isDraw = false;
    winner = 0;
}

// ==================== 认输 ====================

void GomokuGame::DoSurrender() {
    if (gameOver) return;
    gameOver = true;
    surrendered = true;
    isDraw = false;
    winner = (currentTurn == 1) ? 2 : 1; // 当前方认输，对方获胜
    gameOverTimer = 0.f;
}

// ==================== 重新开始 ====================

void GomokuGame::RestartGame() {
    board.Reset();
    currentTurn = 1;
    gameOver = false;
    isDraw = false;
    surrendered = false;
    winner = 0;
    gameOverTimer = 0.f;
    while (!moveHistory.empty()) moveHistory.pop();
    hoverR = -1;
    hoverC = -1;
}

void GomokuGame::Reset() {
    RestartGame();
}

// ==================== 渲染：棋盘 ====================

void GomokuGame::DrawBoard() {
    const float ox = GomokuBoard::ORIGIN_X;
    const float oy = GomokuBoard::ORIGIN_Y;
    const float cell = GomokuBoard::CELL;
    const int sz = GomokuBoard::SIZE;
    const float boardW = (sz - 1) * cell;
    const float boardH = (sz - 1) * cell;

    // 棋盘背景
    sf::RectangleShape bg(sf::Vector2f(boardW + 40, boardH + 40));
    bg.setPosition(ox - 20, oy - 20);
    bg.setFillColor(sf::Color(210, 180, 140));
    bg.setOutlineColor(sf::Color(80, 50, 20));
    bg.setOutlineThickness(3);
    window.draw(bg);

    // 网格线
    sf::Color lc(60, 40, 20);
    for (int i = 0; i < sz; i++) {
        // 横线
        sf::Vertex hline[] = {
            sf::Vertex(sf::Vector2f(ox, oy + i * cell), lc),
            sf::Vertex(sf::Vector2f(ox + boardW, oy + i * cell), lc)
        };
        window.draw(hline, 2, sf::Lines);
        // 竖线
        sf::Vertex vline[] = {
            sf::Vertex(sf::Vector2f(ox + i * cell, oy), lc),
            sf::Vertex(sf::Vector2f(ox + i * cell, oy + boardH), lc)
        };
        window.draw(vline, 2, sf::Lines);
    }

    // 星位标记（天元和四星）
    auto drawStar = [&](int r, int c) {
        sf::CircleShape dot(4);
        dot.setOrigin(4, 4);
        dot.setPosition(ox + c * cell, oy + r * cell);
        dot.setFillColor(sf::Color(60, 40, 20));
        window.draw(dot);
    };

    // 传统五子棋盘星位：四角星 + 天元
    const int starPositions[5][2] = {
        {3, 3}, {3, 11}, {11, 3}, {11, 11},  // 四角星
        {7, 7}                                 // 天元
    };
    for (auto& sp : starPositions)
        drawStar(sp[0], sp[1]);
}

// ==================== 渲染：棋子 ====================

void GomokuGame::DrawStones() {
    const float ox = GomokuBoard::ORIGIN_X;
    const float oy = GomokuBoard::ORIGIN_Y;
    const float cell = GomokuBoard::CELL;
    const float sr = GomokuBoard::STONE_R;

    for (int r = 0; r < GomokuBoard::SIZE; r++) {
        for (int c = 0; c < GomokuBoard::SIZE; c++) {
            int side = board.GetCell(r, c);
            if (side == 0) continue;

            sf::Vector2f pos(ox + c * cell, oy + r * cell);
            sf::Color col = GetSideColor(side);

            // 阴影
            sf::CircleShape shadow(sr);
            shadow.setOrigin(sr, sr);
            shadow.setPosition(pos.x + 2, pos.y + 2);
            shadow.setFillColor(sf::Color(0, 0, 0, 60));
            window.draw(shadow);

            // 主棋子
            sf::CircleShape stone(sr);
            stone.setOrigin(sr, sr);
            stone.setPosition(pos);
            stone.setFillColor(col);
            stone.setOutlineColor(sf::Color(
                std::max(0, col.r - 30),
                std::max(0, col.g - 30),
                std::max(0, col.b - 30)));
            stone.setOutlineThickness(2);
            window.draw(stone);

            // 高光
            sf::CircleShape hl(sr - 5);
            hl.setOrigin(sr - 5, sr - 5);
            hl.setPosition(pos.x - 3, pos.y - 4);
            hl.setFillColor(sf::Color(255, 255, 255, 25));
            window.draw(hl);
        }
    }

    // 悬停预览（合法落子位置）
    if (!gameOver && mouseOnBoard && hoverR >= 0 && hoverC >= 0 &&
        board.IsEmpty(hoverR, hoverC)) {
        sf::Vector2f pos(ox + hoverC * cell, oy + hoverR * cell);
        sf::Color col = GetSideColor(currentTurn);

        sf::CircleShape preview(sr);
        preview.setOrigin(sr, sr);
        preview.setPosition(pos);
        preview.setFillColor(sf::Color(col.r, col.g, col.b, 120));
        preview.setOutlineColor(sf::Color(col.r, col.g, col.b, 180));
        preview.setOutlineThickness(2);
        window.draw(preview);
    }
}

// ==================== 渲染：右侧 UI ====================

void GomokuGame::DrawUI() {
    if (!fontLoaded) return;

    // 标题
    sf::RectangleShape tb(sf::Vector2f(420, 50));
    tb.setPosition(715, 20);
    tb.setFillColor(sf::Color(65, 50, 35));
    tb.setOutlineColor(sf::Color(90, 75, 55));
    tb.setOutlineThickness(1);
    window.draw(tb);
    DrawText(L"\u4E94\u5B50\u68CB", 925, 45, 26, sf::Color(220, 200, 170), true);

    // 回合信息
    sf::RectangleShape turnBg(sf::Vector2f(420, 90));
    turnBg.setPosition(715, 80);
    turnBg.setFillColor(sf::Color(60, 48, 34));
    turnBg.setOutlineColor(sf::Color(85, 70, 50));
    turnBg.setOutlineThickness(1);
    window.draw(turnBg);

    std::wstring turnText;
    sf::Color turnColor, stoneColor;
    if (gameOver) {
        if (isDraw) {
            turnText = L"\u5E73\u5C40\uFF01";
            turnColor = sf::Color(220, 220, 120);
            stoneColor = sf::Color(200, 200, 100);
        } else if (surrendered) {
            turnText = L"\u8BA4\u8F93";
            turnColor = sf::Color(255, 150, 100);
            stoneColor = sf::Color(150, 100, 80);
        } else {
            turnText = (winner == 1) ? L"\u9ED1\u65B9\u80DC\u5229\uFF01" : L"\u767D\u65B9\u80DC\u5229\uFF01";
            turnColor = (winner == 1) ? sf::Color(255, 215, 0) : sf::Color(255, 215, 0);
            stoneColor = (winner == 1) ? sf::Color(30, 30, 30) : sf::Color(240, 240, 240);
        }
    } else {
        turnText = (currentTurn == 1) ? L"\u9ED1\u65B9\u843D\u5B50" : L"\u767D\u65B9\u843D\u5B50";
        turnColor = (currentTurn == 1) ? sf::Color(220, 220, 220) : sf::Color(255, 255, 255);
        stoneColor = (currentTurn == 1) ? sf::Color(30, 30, 30) : sf::Color(240, 240, 240);
    }

    // 棋子颜色指示器
    sf::CircleShape ind(10);
    ind.setOrigin(10, 10);
    ind.setPosition(745, 125);
    ind.setFillColor(stoneColor);
    ind.setOutlineColor(sf::Color::White);
    ind.setOutlineThickness(1);
    window.draw(ind);

    DrawText(turnText, 935, 125, 28, turnColor, true);

    // 按钮
    auto drawBtn = [this](const RectButton& btn) {
        // 阴影
        sf::RectangleShape sh(sf::Vector2f(btn.bounds.width, btn.bounds.height));
        sh.setPosition(btn.bounds.left + 2, btn.bounds.top + 2);
        sh.setFillColor(sf::Color(20, 15, 10, 100));
        window.draw(sh);
        // 主体
        sf::RectangleShape r(sf::Vector2f(btn.bounds.width, btn.bounds.height));
        r.setPosition(btn.bounds.left, btn.bounds.top);
        if (btn.hovered) {
            r.setFillColor(sf::Color(110, 92, 72));
            r.setOutlineColor(sf::Color(160, 140, 110));
        } else {
            r.setFillColor(sf::Color(80, 66, 52));
            r.setOutlineColor(sf::Color(130, 112, 88));
        }
        r.setOutlineThickness(3);
        window.draw(r);
        // 文字
        sf::Text t;
        t.setFont(font);
        t.setString(btn.label);
        t.setCharacterSize(18);
        t.setFillColor(sf::Color(245, 235, 220));
        auto b = t.getLocalBounds();
        t.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
        t.setPosition(btn.bounds.left + btn.bounds.width / 2.f,
                      btn.bounds.top + btn.bounds.height / 2.f);
        window.draw(t);
    };

    drawBtn(restartBtn);
    drawBtn(undoBtn);
    drawBtn(surrenderBtn);

    // 操作提示
    DrawText(L"\u5DE6\u952E\u70B9\u51FB\u843D\u5B50", 925, 360, 14,
             sf::Color(160, 140, 120), true);
    DrawText(L"\u9ED1\u5148\u767D\u540E \u4EA4\u66FF\u843D\u5B50", 925, 385, 14,
             sf::Color(140, 120, 100), true);

    // 底部版本
    DrawText(L"v0.1  C++17 + SFML 2.6.2", 925, WIN_H - 20, 12,
             sf::Color(100, 90, 80), true);
}

// ==================== 渲染：游戏结束 ====================

void GomokuGame::DrawGameOver() {
    float alpha = std::min(gameOverTimer * 200.f, 180.f);
    sf::RectangleShape ov(sf::Vector2f((float)WIN_W, (float)WIN_H));
    ov.setPosition(0, 0);
    ov.setFillColor(sf::Color(0, 0, 0, (sf::Uint8)alpha));
    window.draw(ov);

    if (gameOverTimer <= 0.5f) return;

    float ta = std::min((gameOverTimer - 0.5f) * 400.f, 255.f);
    std::wstring mt, st;
    sf::Color mc;

    if (isDraw) {
        mt = L"\u5E73\u5C40";
        mc = sf::Color(255, 255, 100, (sf::Uint8)ta);
        st = L"\u68CB\u76D8\u5DF2\u6EE1";
    } else if (surrendered) {
        mt = (winner == 1) ? L"\u9ED1\u65B9\u80DC\u5229" : L"\u767D\u65B9\u80DC\u5229";
        mc = (winner == 1) ? sf::Color(200, 200, 200, (sf::Uint8)ta)
                           : sf::Color(255, 255, 200, (sf::Uint8)ta);
        st = (winner == 1) ? L"\u767D\u65B9\u8BA4\u8F93" : L"\u9ED1\u65B9\u8BA4\u8F93";
    } else {
        mt = (winner == 1) ? L"\u9ED1\u65B9\u4E94\u8FDE\u80DC\u5229\uFF01"
                           : L"\u767D\u65B9\u4E94\u8FDE\u80DC\u5229\uFF01";
        mc = sf::Color(255, 215, 0, (sf::Uint8)ta);
        st = (winner == 1) ? L"\u9ED1\u5B50\u4E94\u5B50\u8FDE\u73E0"
                           : L"\u767D\u5B50\u4E94\u5B50\u8FDE\u73E0";
    }

    DrawText(mt, 575, 360, 60, mc, true);
    DrawText(st, 575, 430, 24, sf::Color(200, 200, 200, (sf::Uint8)ta), true);

    // 重新开始按钮
    float bx = 475, by_ = 500;
    gameOverRestartBtn.bounds.left = bx;
    gameOverRestartBtn.bounds.top = by_;

    sf::RectangleShape bs(sf::Vector2f(200, 50));
    bs.setPosition(bx + 3, by_ + 3);
    bs.setFillColor(sf::Color(0, 0, 0, (sf::Uint8)(ta * 0.5f)));
    window.draw(bs);

    sf::RectangleShape br(sf::Vector2f(200, 50));
    br.setPosition(bx, by_);
    br.setFillColor(gameOverRestartBtn.hovered
        ? sf::Color(140, 110, 80, (sf::Uint8)ta)
        : sf::Color(100, 80, 60, (sf::Uint8)ta));
    br.setOutlineColor(sf::Color(180, 150, 110, (sf::Uint8)ta));
    br.setOutlineThickness(2);
    window.draw(br);

    DrawText(L"\u91CD\u65B0\u5F00\u59CB", bx + 100, by_ + 25, 22,
             sf::Color(255, 255, 255, (sf::Uint8)ta), true);
}

// ==================== 辅助方法 ====================

void GomokuGame::DrawText(const std::wstring& text, float x, float y,
                           unsigned sz, sf::Color c, bool center) {
    if (!fontLoaded) return;
    sf::Text t;
    t.setFont(font);
    t.setString(text);
    t.setCharacterSize(sz);
    t.setFillColor(c);
    if (center) {
        auto b = t.getLocalBounds();
        t.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
    }
    t.setPosition(x, y);
    window.draw(t);
}

sf::Color GomokuGame::GetSideColor(int side) const {
    if (side == 1)
        return sf::Color(20, 20, 20);   // 黑子
    else
        return sf::Color(245, 245, 245); // 白子
}
