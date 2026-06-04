// ===================================================================
// UIManager.cpp — UI 管理器的实现
// 负责对角象棋游戏中的所有视觉元素绘制
// 包括：棋盘、棋子、按钮、走棋日志、教程面板、通知、网络状态等
// ===================================================================

#include "Engine/UI/UIManager.h"
#include "Engine/Piece/DiagonalChessPiece.h"
#include <sstream>
#include <algorithm>
#include <cstring>

// 构造函数：初始化工作推迟到 Init() 中进行
UIManager::UIManager() {}

// ==================== 初始化 ====================
// 设置窗口和字体引用，初始化所有按钮的位置和文字
void UIManager::Init(sf::RenderWindow& w, sf::Font& f, bool ok) {
    window = &w; font = &f; fontLoaded = ok;

    // 左侧按钮列（操作区）
    undoBtn = {sf::FloatRect(730, 255, 95, 44), L"\u6094\u68CB", false, false};        // "悔棋"
    restartBtn = {sf::FloatRect(835, 255, 95, 44), L"\u91CD\u5F00", false, false};     // "重开"
    aiBtn = {sf::FloatRect(730, 305, 95, 44), L"AI: \u5173", false, false};            // "AI: 关"
    difficultyBtn = {sf::FloatRect(835, 305, 95, 44), L"\u4E2D\u7B49", false, false};   // "中等"
    onlineBtn = {sf::FloatRect(730, 355, 95, 44), L"\u8054\u673A: \u5173", false, false}; // "联机: 关"
    tutorialBtn = {sf::FloatRect(835, 355, 95, 44), L"\u6559\u7A0B", false, false};    // "教程"
    surrenderBtn = {sf::FloatRect(730, 405, 95, 44), L"\u8BA4\u8F93", false, false};    // "认输"
    drawOfferBtn = {sf::FloatRect(835, 405, 95, 44), L"\u548C\u68CB", false, false};    // "和棋"

    // 网络按钮
    hostBtn = {sf::FloatRect(730, 455, 95, 44), L"\u521B\u5EFA\u623F\u95F4", false, false};   // "创建房间"
    joinBtn = {sf::FloatRect(835, 455, 95, 44), L"\u52A0\u5165\u623F\u95F4", false, false};   // "加入房间"
    connectBtn = {sf::FloatRect(730, 545, 200, 44), L"\u8FDE\u63A5", false, false};            // "连接"
    disconnectBtn = {sf::FloatRect(730, 455, 200, 44), L"\u65AD\u5F00\u8FDE\u63A5", false, false}; // "断开连接"

    // 弹窗中的按钮
    gameOverRestartBtn = {sf::FloatRect(0, 0, 200, 50), L"\u91CD\u65B0\u5F00\u59CB", false, false}; // "重新开始"
    undoAcceptBtn = {sf::FloatRect(730, 255, 95, 44), L"\u540C\u610F", false, false};     // "同意"
    undoRejectBtn = {sf::FloatRect(835, 255, 95, 44), L"\u62D2\u7EDD", false, false};     // "拒绝"
    restartAcceptBtn = {sf::FloatRect(730, 305, 95, 44), L"\u540C\u610F", false, false};
    restartRejectBtn = {sf::FloatRect(835, 305, 95, 44), L"\u62D2\u7EDD", false, false};

    // 认输弹窗按钮
    surrenderRedBtn = {sf::FloatRect(0, 0, 180, 50), L"\u7EA2\u65B9\u8BA4\u8F93", false, false};   // "红方认输"
    surrenderBlackBtn = {sf::FloatRect(0, 0, 180, 50), L"\u9ED1\u65B9\u8BA4\u8F93", false, false}; // "黑方认输"
    surrenderAcceptBtn = {sf::FloatRect(730, 255, 95, 44), L"\u540C\u610F", false, false};
    surrenderRejectBtn = {sf::FloatRect(835, 255, 95, 44), L"\u62D2\u7EDD", false, false};
    drawAcceptBtn = {sf::FloatRect(730, 255, 95, 44), L"\u540C\u610F", false, false};
    drawRejectBtn = {sf::FloatRect(835, 255, 95, 44), L"\u62D2\u7EDD", false, false};

    // 关闭按钮（窗口右上角）
    closeBtn = {sf::FloatRect(1102, 23, 28, 18), L"\u2716", false, false};                // "✖"
}

// ==================== 文本绘制辅助 ====================

// 绘制文本（不带阴影）
void UIManager::DrawText(const std::wstring& text, float x, float y,
                          unsigned sz, sf::Color c, bool center) {
    sf::Text t;
    t.setFont(*font);
    t.setString(text);
    t.setCharacterSize(sz);
    t.setFillColor(c);
    if (center) {                                        // 居中：将原点设为文本中心
        auto b = t.getLocalBounds();
        t.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
    }
    t.setPosition(x, y);
    window->draw(t);
}

// 绘制带阴影的文本（先画深色偏移版，再画原色版）
void UIManager::DrawTextWithShadow(const std::wstring& text, float x, float y,
                                    unsigned sz, sf::Color c, bool center) {
    DrawText(text, x + 2, y + 2, sz, sf::Color(0, 0, 0, c.a / 2), center);  // 阴影（半透明黑色）
    DrawText(text, x, y, sz, c, center);                                      // 原文本
}

// ==================== 绘制棋盘 ====================
void UIManager::DrawBoard(const DiagonalChessBoard& board) {
    // 棋盘外框（四边形背景）
    sf::ConvexShape bg(4);
    bg.setPoint(0, board.GridToScreen(0, 0));            // 左上角
    bg.setPoint(1, board.GridToScreen(8, 0));            // 右上角
    bg.setPoint(2, board.GridToScreen(8, 8));            // 右下角
    bg.setPoint(3, board.GridToScreen(0, 8));            // 左下角
    bg.setFillColor(sf::Color(210, 180, 140));            // 木色
    bg.setOutlineColor(sf::Color(80, 50, 20));
    bg.setOutlineThickness(4);
    window->draw(bg);

    // 内框（略小的四边形，颜色稍浅）
    sf::ConvexShape inner(4);
    inner.setPoint(0, board.GridToScreen(1, 1));
    inner.setPoint(1, board.GridToScreen(7, 1));
    inner.setPoint(2, board.GridToScreen(7, 7));
    inner.setPoint(3, board.GridToScreen(1, 7));
    inner.setFillColor(sf::Color(220, 190, 150));
    window->draw(inner);

    // 绘制横线（行方向：从第0列到第7列，每行8条线）
    sf::Color lc(60, 40, 20);
    for (int r = 0; r < 9; r++)
        for (int c = 0; c < 8; c++) {
            auto p1 = board.GridToScreen(r, c), p2 = board.GridToScreen(r, c + 1);
            sf::Vertex l[] = {sf::Vertex(p1,lc), sf::Vertex(p2,lc)};
            window->draw(l, 2, sf::Lines);
        }

    // 绘制竖线（列方向：从第0行到第7行，每列8条线）
    for (int c = 0; c < 9; c++)
        for (int r = 0; r < 8; r++) {
            auto p1 = board.GridToScreen(r, c), p2 = board.GridToScreen(r + 1, c);
            sf::Vertex l[] = {sf::Vertex(p1,lc), sf::Vertex(p2,lc)};
            window->draw(l, 2, sf::Lines);
        }

    // 绘制九宫格对角斜线（士的移动范围标记）
    sf::Color pc(100, 60, 30, 180);
    auto drawPalace = [&](int r0, int c0) {
        for (int r = r0; r < r0 + 2; r++)
            for (int c = c0; c < c0 + 2; c++) {
                // 斜线方向1：左上→右下
                auto p1 = board.GridToScreen(r, c), p2 = board.GridToScreen(r + 1, c + 1);
                sf::Vertex d[] = {sf::Vertex(p1,pc), sf::Vertex(p2,pc)};
                window->draw(d, 2, sf::Lines);
                // 斜线方向2：右上→左下
                auto p3 = board.GridToScreen(r + 1, c), p4 = board.GridToScreen(r, c + 1);
                sf::Vertex d2[] = {sf::Vertex(p3,pc), sf::Vertex(p4,pc)};
                window->draw(d2, 2, sf::Lines);
            }
    };
    drawPalace(0, 6);                                    // 红方九宫（行列 0-2, 6-8）
    drawPalace(6, 0);                                    // 黑方九宫（行列 6-8, 0-2）

    // 绘制所有交叉点（小圆点，共 9×9 = 81 个）
    for (int r = 0; r < 9; r++)
        for (int c = 0; c < 9; c++) {
            sf::CircleShape dot(3);
            dot.setOrigin(3, 3);
            dot.setPosition(board.GridToScreen(r, c));
            dot.setFillColor(sf::Color(80, 50, 20));
            window->draw(dot);
        }
}

// ==================== 绘制棋子 ====================
void UIManager::DrawPieces(const DiagonalChessBoard& board, int selR, int selC,
                            const std::vector<sf::Vector2i>& validMoves) {
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (!board.IsOccupied(r, c)) continue;       // 空位跳过
            const auto& p = board.At(r, c);
            sf::Vector2f pos = board.GridToScreen(r, c);

            // 棋子阴影（偏移2像素的黑色半透明圆）
            sf::CircleShape sh(PIECE_R + 2);
            sh.setOrigin(PIECE_R + 2, PIECE_R + 2);
            sh.setPosition(pos.x + 2, pos.y + 2);
            sh.setFillColor(sf::Color(0, 0, 0, 80));
            window->draw(sh);

            // 棋子主体（红方红色，黑方深灰色）
            sf::CircleShape piece(PIECE_R);
            piece.setOrigin(PIECE_R, PIECE_R);
            piece.setPosition(pos);
            if (p.GetSide() == 0) {
                piece.setFillColor(sf::Color(180, 30, 30));    // 红方：暗红色
                piece.setOutlineColor(sf::Color(120, 20, 20));
            } else {
                piece.setFillColor(sf::Color(40, 40, 40));     // 黑方：深灰色
                piece.setOutlineColor(sf::Color(20, 20, 20));
            }
            piece.setOutlineThickness(3);
            window->draw(piece);

            // 高光（左上角半透明白色弧形，增加立体感）
            sf::CircleShape hl(PIECE_R - 4);
            hl.setOrigin(PIECE_R - 4, PIECE_R - 4);
            hl.setPosition(pos.x - 3, pos.y - 3);
            hl.setFillColor(sf::Color(255, 255, 255, 40));
            window->draw(hl);

            // 选中高亮环（金色圆环标记选中的棋子）
            if (r == selR && c == selC) {
                sf::CircleShape ring(PIECE_R + 5);
                ring.setOrigin(PIECE_R + 5, PIECE_R + 5);
                ring.setPosition(pos);
                ring.setFillColor(sf::Color::Transparent);
                ring.setOutlineColor(sf::Color(255, 220, 0, 200));  // 半透明金色
                ring.setOutlineThickness(4);
                window->draw(ring);
            }

            // 棋子上的汉字符号
            if (fontLoaded) {
                auto sym = p.GetSymbol();
                sf::Text t;
                t.setFont(*font);
                t.setString(sym);
                t.setCharacterSize(20);
                t.setFillColor(sf::Color::White);
                auto b = t.getLocalBounds();
                t.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
                t.setPosition(pos);
                window->draw(t);
            }
        }
    }

    // 绘制合法走法标记
    for (const auto& m : validMoves) {
        sf::Vector2f pos = board.GridToScreen(m.x, m.y);
        if (board.IsOccupied(m.x, m.y)) {
            // 有棋子的位置（可吃）→ 红色圆环标记
            sf::CircleShape ring(PIECE_R + 3);
            ring.setOrigin(PIECE_R + 3, PIECE_R + 3);
            ring.setPosition(pos);
            ring.setFillColor(sf::Color::Transparent);
            ring.setOutlineColor(sf::Color(255, 60, 60, 220));
            ring.setOutlineThickness(4);
            window->draw(ring);
        } else {
            // 空位（可走）→ 绿色小圆点标记
            sf::CircleShape dot(10);
            dot.setOrigin(10, 10);
            dot.setPosition(pos);
            dot.setFillColor(sf::Color(80, 220, 80, 180));
            window->draw(dot);
        }
    }
}

// ==================== 绘制右侧 UI 面板 ====================
void UIManager::DrawUI(int currentTurn, bool gameOver, bool isDrawGame,
                        bool surrendered, Side winner, bool inCheck, bool aiThinking) {
    if (!fontLoaded) return;

    // 标题栏背景
    sf::RectangleShape tb(sf::Vector2f(420, 55));
    tb.setPosition(715, 20);
    tb.setFillColor(sf::Color(65, 50, 35));
    tb.setOutlineColor(sf::Color(90, 75, 55));
    tb.setOutlineThickness(1);
    window->draw(tb);
    DrawTextWithShadow(L"\u5BF9\u89D2\u8C61\u68CB", 925, 50, 26, sf::Color(220, 200, 170), true);  // "对角象棋"

    // 回合信息背景
    sf::RectangleShape turnBg(sf::Vector2f(420, 70));
    turnBg.setPosition(715, 85);
    turnBg.setFillColor(sf::Color(60, 48, 34));
    turnBg.setOutlineColor(sf::Color(85, 70, 50));
    turnBg.setOutlineThickness(1);
    window->draw(turnBg);

    // 根据游戏状态确定显示的文字和颜色
    std::wstring turnText; sf::Color turnColor, indColor;
    if (gameOver) {
        if (isDrawGame) {
            turnText = L"\u548C\u68CB\uFF01";            // "和棋！"
            turnColor = sf::Color(220, 220, 120);       // 黄色
            indColor = sf::Color(200, 200, 100);
        } else if (surrendered) {
            turnText = L"\u8BA4\u8F93";                 // "认输"
            turnColor = sf::Color(255, 150, 100);
            indColor = sf::Color(255, 150, 100);
        } else if (inCheck) {
            turnText = L"\u7EDD\u6740\uFF01";           // "绝杀！"
            turnColor = sf::Color(255, 215, 0);         // 金色
            indColor = sf::Color(255, 215, 0);
        } else {
            turnText = L"\u56F0\u6BD9\uFF01";           // "困毙！"
            turnColor = sf::Color(255, 180, 50);
            indColor = sf::Color(255, 180, 50);
        }
    } else {
        turnText = (currentTurn == 0) ? L"\u7EA2\u65B9\u8D70\u68CB" : L"\u9ED1\u65B9\u8D70\u68CB";  // "红方走棋"/"黑方走棋"
        turnColor = (currentTurn == 0) ? sf::Color(255, 120, 120) : sf::Color(220, 220, 220);
        indColor = (currentTurn == 0) ? sf::Color(220, 60, 60) : sf::Color(80, 80, 80);
    }

    // 棋子颜色指示器
    sf::CircleShape ind(12);
    ind.setOrigin(12, 12);
    ind.setPosition(745, 120);
    ind.setFillColor(indColor);
    ind.setOutlineColor(sf::Color(255, 255, 255, 80));
    ind.setOutlineThickness(2);
    window->draw(ind);
    DrawTextWithShadow(turnText, 935, 120, 30, turnColor, true);

    // 将军提示（红色背景框）
    if (!gameOver && inCheck) {
        sf::RectangleShape cb(sf::Vector2f(140, 40));
        cb.setPosition(855, 170);
        cb.setFillColor(sf::Color(180, 40, 40, 220));
        cb.setOutlineColor(sf::Color(255, 120, 120));
        cb.setOutlineThickness(2);
        window->draw(cb);
        DrawTextWithShadow(L"\u5C06\u519B\uFF01", 925, 190, 24, sf::Color(255, 255, 150), true);  // "将军！"
    }

    // AI 思考提示
    if (aiThinking)
        DrawText(L"AI\u601D\u8003\u4E2D...", 925, 225, 18, sf::Color(200, 180, 150), true);  // "AI思考中..."

    // 关闭按钮（返回主菜单）
    sf::RectangleShape closeBg(sf::Vector2f(closeBtn.bounds.width, closeBtn.bounds.height));
    closeBg.setPosition(closeBtn.bounds.left, closeBtn.bounds.top);
    closeBg.setFillColor(closeBtn.hovered ? sf::Color(220, 60, 60) : sf::Color(180, 50, 50));
    closeBg.setOutlineColor(sf::Color(240, 90, 90));
    closeBg.setOutlineThickness(1.5f);
    window->draw(closeBg);
    DrawTextWithShadow(L"\u2716", closeBtn.bounds.left + closeBtn.bounds.width / 2.f,
                       closeBtn.bounds.top + closeBtn.bounds.height / 2.f,
                       12, sf::Color::White, true);
}

// ==================== 通知系统 ====================

// 绘制通知横幅（半透明底色 + 文字，逐渐消失）
void UIManager::DrawNotification() {
    if (!notificationText.empty() && notificationTimer > 0.f) {
        float a = std::min(notificationTimer / 3.f, 1.f) * 255.f;
        sf::RectangleShape bg(sf::Vector2f(300, 30));
        bg.setPosition(820, 180);
        bg.setFillColor(sf::Color(180, 60, 30, (sf::Uint8)(a * 0.6f)));
        bg.setOutlineColor(sf::Color(255, 100, 50, (sf::Uint8)(a * 0.8f)));
        bg.setOutlineThickness(2);
        window->draw(bg);
        DrawTextWithShadow(notificationText, 970, 195, 16,
                           sf::Color(255, 100, 60, (sf::Uint8)a), true);
    }
}

// ==================== 按钮绘制方法 ====================

// 标准按钮（棕色系）
void UIManager::DrawButton(const UIButton& btn) {
    // 阴影
    sf::RectangleShape sh(sf::Vector2f(btn.bounds.width, btn.bounds.height));
    sh.setPosition(btn.bounds.left + 2, btn.bounds.top + 2);
    sh.setFillColor(sf::Color(20, 15, 10, 100));
    window->draw(sh);

    // 主体
    sf::RectangleShape r(sf::Vector2f(btn.bounds.width, btn.bounds.height));
    r.setPosition(btn.bounds.left, btn.bounds.top);
    if (btn.disabled) {                                  // 禁用状态：灰色
        r.setFillColor(sf::Color(70, 60, 48));
        r.setOutlineColor(sf::Color(90, 78, 62));
    } else if (btn.hovered) {                            // 悬停状态：高亮
        r.setFillColor(sf::Color(110, 92, 72));
        r.setOutlineColor(sf::Color(160, 140, 110));
    } else {                                             // 正常状态
        r.setFillColor(sf::Color(80, 66, 52));
        r.setOutlineColor(sf::Color(130, 112, 88));
    }
    r.setOutlineThickness(3);
    window->draw(r);

    // 文字
    if (fontLoaded) {
        sf::Text t;
        t.setFont(*font);
        t.setString(btn.label);
        t.setCharacterSize(18);
        t.setFillColor(btn.disabled ? sf::Color(110, 100, 88) : sf::Color(245, 235, 220));
        auto b = t.getLocalBounds();
        t.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
        t.setPosition(btn.bounds.left + btn.bounds.width / 2.f,
                      btn.bounds.top + btn.bounds.height / 2.f);
        window->draw(t);
    }
}

// 小型确认/取消按钮
void UIManager::DrawSmallButton(const UIButton& btn, sf::Color fill, sf::Color outline) {
    sf::RectangleShape r(sf::Vector2f(btn.bounds.width, btn.bounds.height));
    r.setPosition(btn.bounds.left, btn.bounds.top);
    r.setFillColor(btn.hovered ? sf::Color(fill.r + 40, fill.g + 30, fill.b + 20) : fill);
    r.setOutlineColor(outline);
    r.setOutlineThickness(2);
    window->draw(r);

    sf::Text t;
    t.setFont(*font);
    t.setString(btn.label);
    t.setCharacterSize(14);
    t.setFillColor(sf::Color::White);
    auto b = t.getLocalBounds();
    t.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
    t.setPosition(btn.bounds.left + btn.bounds.width / 2.f,
                  btn.bounds.top + btn.bounds.height / 2.f);
    window->draw(t);
}

// 网络按钮（绿色系）
void UIManager::DrawNetButton(const UIButton& btn) {
    // 阴影
    sf::RectangleShape sh(sf::Vector2f(btn.bounds.width, btn.bounds.height));
    sh.setPosition(btn.bounds.left + 2, btn.bounds.top + 2);
    sh.setFillColor(sf::Color(20, 15, 10, 100));
    window->draw(sh);

    // 主体
    sf::RectangleShape r(sf::Vector2f(btn.bounds.width, btn.bounds.height));
    r.setPosition(btn.bounds.left, btn.bounds.top);
    r.setFillColor(btn.hovered ? sf::Color(80, 120, 80) : sf::Color(60, 90, 60));
    r.setOutlineColor(sf::Color(100, 150, 100));
    r.setOutlineThickness(2);
    window->draw(r);

    // 文字
    sf::Text t;
    t.setFont(*font);
    t.setString(btn.label);
    t.setCharacterSize(18);
    t.setFillColor(sf::Color(230, 255, 230));
    auto b = t.getLocalBounds();
    t.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
    t.setPosition(btn.bounds.left + btn.bounds.width / 2.f,
                  btn.bounds.top + btn.bounds.height / 2.f);
    window->draw(t);
}

// 网络断开按钮（红色系）
void UIManager::DrawNetRedButton(const UIButton& btn) {
    sf::RectangleShape r(sf::Vector2f(btn.bounds.width, btn.bounds.height));
    r.setPosition(btn.bounds.left, btn.bounds.top);
    r.setFillColor(btn.hovered ? sf::Color(150, 60, 60) : sf::Color(120, 40, 40));
    r.setOutlineColor(sf::Color(180, 80, 80));
    r.setOutlineThickness(2);
    window->draw(r);

    sf::Text t;
    t.setFont(*font);
    t.setString(btn.label);
    t.setCharacterSize(18);
    t.setFillColor(sf::Color(255, 220, 220));
    auto b = t.getLocalBounds();
    t.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
    t.setPosition(btn.bounds.left + btn.bounds.width / 2.f,
                  btn.bounds.top + btn.bounds.height / 2.f);
    window->draw(t);
}

// 绘制所有操作按钮
void UIManager::DrawButtons() {
    DrawButton(undoBtn); DrawButton(restartBtn);
    DrawButton(aiBtn); DrawButton(difficultyBtn);
    DrawButton(onlineBtn); DrawButton(tutorialBtn);
    DrawButton(surrenderBtn); DrawButton(drawOfferBtn);
}

// ==================== 网络请求弹窗 ====================
void UIManager::DrawRequestPopups(bool undoRecv, bool restartRecv,
                                   bool surrRecv, bool drawRecv) {
    // 悔棋请求弹窗（橙色框）
    if (undoRecv) {
        sf::RectangleShape bg(sf::Vector2f(200, 70));
        bg.setPosition(940, 255);
        bg.setFillColor(sf::Color(180, 130, 30, 220));
        bg.setOutlineColor(sf::Color(255, 180, 50));
        bg.setOutlineThickness(2);
        window->draw(bg);
        DrawText(L"\u5BF9\u65B9\u8BF7\u6C42\u6094\u68CB", 1040, 275, 16,       // "对方请求悔棋"
                 sf::Color(255, 255, 200), true);
        undoAcceptBtn.bounds = sf::FloatRect(940, 290, 95, 44);
        undoRejectBtn.bounds = sf::FloatRect(1045, 290, 95, 44);
        DrawSmallButton(undoAcceptBtn, sf::Color(60, 140, 60), sf::Color(80, 180, 80));
        DrawSmallButton(undoRejectBtn, sf::Color(160, 50, 50), sf::Color(200, 80, 80));
    }

    // 重开请求弹窗
    if (restartRecv) {
        sf::RectangleShape bg(sf::Vector2f(200, 70));
        bg.setPosition(940, 305);
        bg.setFillColor(sf::Color(180, 130, 30, 220));
        bg.setOutlineColor(sf::Color(255, 180, 50));
        bg.setOutlineThickness(2);
        window->draw(bg);
        DrawText(L"\u5BF9\u65B9\u8BF7\u6C42\u91CD\u5F00", 1040, 325, 16,       // "对方请求重开"
                 sf::Color(255, 255, 200), true);
        restartAcceptBtn.bounds = sf::FloatRect(940, 340, 95, 44);
        restartRejectBtn.bounds = sf::FloatRect(1045, 340, 95, 44);
        DrawSmallButton(restartAcceptBtn, sf::Color(60, 140, 60), sf::Color(80, 180, 80));
        DrawSmallButton(restartRejectBtn, sf::Color(160, 50, 50), sf::Color(200, 80, 80));
    }

    // 认输请求弹窗（红色框）
    if (surrRecv) {
        sf::RectangleShape bg(sf::Vector2f(200, 70));
        bg.setPosition(940, 355);
        bg.setFillColor(sf::Color(180, 40, 40, 220));
        bg.setOutlineColor(sf::Color(255, 80, 80));
        bg.setOutlineThickness(2);
        window->draw(bg);
        DrawText(L"\u5BF9\u65B9\u8BA4\u8F93\u4E86", 1040, 375, 16,              // "对方认输了"
                 sf::Color(255, 220, 200), true);
        surrenderAcceptBtn.bounds = sf::FloatRect(940, 390, 95, 44);
        surrenderRejectBtn.bounds = sf::FloatRect(1045, 390, 95, 44);
        DrawSmallButton(surrenderAcceptBtn, sf::Color(60, 140, 60), sf::Color(80, 180, 80));
        DrawSmallButton(surrenderRejectBtn, sf::Color(160, 50, 50), sf::Color(200, 80, 80));
    }

    // 和棋请求弹窗（黄色框）
    if (drawRecv) {
        sf::RectangleShape bg(sf::Vector2f(200, 70));
        bg.setPosition(940, 405);
        bg.setFillColor(sf::Color(180, 180, 40, 220));
        bg.setOutlineColor(sf::Color(255, 255, 80));
        bg.setOutlineThickness(2);
        window->draw(bg);
        DrawText(L"\u5BF9\u65B9\u8BF7\u6C42\u548C\u68CB", 1040, 425, 16,       // "对方请求和棋"
                 sf::Color(255, 255, 200), true);
        drawAcceptBtn.bounds = sf::FloatRect(940, 440, 95, 44);
        drawRejectBtn.bounds = sf::FloatRect(1045, 440, 95, 44);
        DrawSmallButton(drawAcceptBtn, sf::Color(60, 140, 60), sf::Color(80, 180, 80));
        DrawSmallButton(drawRejectBtn, sf::Color(160, 50, 50), sf::Color(200, 80, 80));
    }
}

// ==================== 走棋日志 ====================
void UIManager::DrawMoveLog(const std::vector<std::wstring>& logs) {
    if (!fontLoaded) return;

    // 日志区域（分隔线下方到窗口底部的区域）
    float logY = logDividerY, logH = 810.f - logDividerY;

    // 背景
    sf::RectangleShape bg(sf::Vector2f(420, logH));
    bg.setPosition(715, logY);
    bg.setFillColor(sf::Color(55, 44, 32));
    bg.setOutlineColor(sf::Color(85, 70, 52));
    bg.setOutlineThickness(1);
    window->draw(bg);

    // 标题栏
    sf::RectangleShape hd(sf::Vector2f(420, 30));
    hd.setPosition(715, logY);
    hd.setFillColor(sf::Color(70, 56, 40));
    window->draw(hd);
    DrawTextWithShadow(L"\u8D70\u68CB\u8BB0\u5F55", 925, logY + 15, 16,         // "走棋记录"
                       sf::Color(230, 210, 180), true);

    // 计算可见行数（每行高度 22 像素）
    int total = (int)logs.size();
    int vis = (int)((logH - 40) / 22);                  // 可见的最大行数
    int mx = std::max(0, total - vis);                  // 最大滚动偏移
    if (logScrollOffset > mx) logScrollOffset = mx;
    if (logScrollOffset < 0) logScrollOffset = 0;
    int start = std::max(0, total - vis - logScrollOffset);

    // 逐行绘制日志
    float y = logY + 40;
    for (int i = start; i < total; i++) {
        // 偶数行画浅色背景（斑马条纹）
        if (i % 2 == 0) {
            sf::RectangleShape rb(sf::Vector2f(410, 20));
            rb.setPosition(720, y - 2);
            rb.setFillColor(sf::Color(65, 52, 38, 100));
            window->draw(rb);
        }
        std::wostringstream oss;
        oss << (i + 1) << L". " << logs[i];             // 行号 + 日志内容
        DrawText(oss.str(), 730, y, 13,
                 (i % 2 == 0) ? sf::Color(255, 140, 140) : sf::Color(220, 220, 220));
        y += 22;
    }
}

// ==================== 游戏结束特效 ====================
void UIManager::DrawGameOverEffect(float timer, bool isDraw, Side winner,
                                    bool surrendered, bool checkmate,
                                    int mwc, bool agreedDraw, bool insuf) {
    // 半透明黑色遮罩（逐渐变暗）
    float alpha = std::min(timer * 200.f, 180.f);
    sf::RectangleShape ov(sf::Vector2f(1150, 820));
    ov.setPosition(0, 0);
    ov.setFillColor(sf::Color(0, 0, 0, (sf::Uint8)alpha));
    window->draw(ov);

    if (timer <= 0.5f) return;                           // 0.5 秒后才显示文字

    // 文字渐显
    float ta = std::min((timer - 0.5f) * 400.f, 255.f);
    std::wstring mt, st;                                 // 主标题 and 副标题
    sf::Color mc;

    if (isDraw) {
        mt = L"\u548C\u68CB";                            // "和棋"
        mc = sf::Color(255, 255, 100, (sf::Uint8)ta);
        if (mwc >= DRAW_LIMIT) st = L"\u81EA\u7136\u9650\u7740\u548C\u68CB";    // "自然限着和棋"
        else if (insuf) st = L"\u5B50\u529B\u4E0D\u8DB3\u548C\u68CB";           // "子力不足和棋"
        else if (agreedDraw) st = L"\u53CC\u65B9\u540C\u610F\u548C\u68CB";      // "双方同意和棋"
        else st = L"\u56F0\u6BD9\u548C\u68CB";                                   // "困毙和棋"
    } else {
        Side loser = (winner == Side::RED) ? Side::BLACK : Side::RED;
        if (surrendered) {
            mt = (winner == Side::RED) ? L"\u7EA2\u65B9\u80DC\u5229" : L"\u9ED1\u65B9\u80DC\u5229";
            mc = (winner == Side::RED) ? sf::Color(255, 80, 80, (sf::Uint8)ta)
                                       : sf::Color(220, 220, 220, (sf::Uint8)ta);
            st = (loser == Side::RED) ? L"\u7EA2\u65B9\u8BA4\u8F93" : L"\u9ED1\u65B9\u8BA4\u8F93";
        } else if (checkmate) {
            mt = L"\u7EDD\u6740";                        // "绝杀"
            mc = sf::Color(255, 215, 0, (sf::Uint8)ta);
            st = (winner == Side::RED) ? L"\u7EA2\u65B9\u7EDD\u6740\u9ED1\u65B9" : L"\u9ED1\u65B9\u7EDD\u6740\u7EA2\u65B9";
        } else {
            mt = L"\u56F0\u6BD9";                        // "困毙"
            mc = sf::Color(255, 180, 50, (sf::Uint8)ta);
            st = (loser == Side::RED) ? L"\u7EA2\u65B9\u88AB\u56F0\u6BD9" : L"\u9ED1\u65B9\u88AB\u56F0\u6BD9";
        }
    }
    DrawTextWithShadow(mt, 575, 360, 72, mc, true);
    DrawTextWithShadow(st, 575, 430, 28, sf::Color(200, 200, 200, (sf::Uint8)ta), true);

    // 重新开始按钮
    float bx = 475, by = 500;
    gameOverRestartBtn.bounds.left = bx;
    gameOverRestartBtn.bounds.top = by;

    // 按钮阴影
    sf::RectangleShape bs(sf::Vector2f(200, 50));
    bs.setPosition(bx + 3, by + 3);
    bs.setFillColor(sf::Color(0, 0, 0, (sf::Uint8)(ta * 0.5f)));
    window->draw(bs);

    // 按钮主体
    sf::RectangleShape br(sf::Vector2f(200, 50));
    br.setPosition(bx, by);
    br.setFillColor(gameOverRestartBtn.hovered
        ? sf::Color(140, 110, 80, (sf::Uint8)ta)
        : sf::Color(100, 80, 60, (sf::Uint8)ta));
    br.setOutlineColor(sf::Color(180, 150, 110, (sf::Uint8)ta));
    br.setOutlineThickness(2);
    window->draw(br);
    DrawTextWithShadow(L"\u91CD\u65B0\u5F00\u59CB", bx + 100, by + 25, 22,
                       sf::Color(255, 255, 255, (sf::Uint8)ta), true);
}

// ==================== 网络联机 UI ====================
void UIManager::DrawNetUI(bool netMode, NetState ns, const std::wstring& localIP,
                           const std::wstring& inputIP, bool showIP, int netSide) {
    if (!fontLoaded || !netMode) return;

    // 离线状态：显示创建/加入房间按钮
    if (ns == NetState::OFFLINE) {
        if (showIP) {
            // IP 输入框
            sf::RectangleShape ib(sf::Vector2f(200, 35));
            ib.setPosition(730, 455);
            ib.setFillColor(sf::Color(40, 35, 28));
            ib.setOutlineColor(sf::Color(120, 100, 80));
            ib.setOutlineThickness(2);
            window->draw(ib);
            std::wstring d = inputIP.empty() ? L"\u8F93\u5165IP\u5730\u5740..." : inputIP;  // "输入IP地址..."
            DrawText(d, 740, 462, 15,
                     inputIP.empty() ? sf::Color(120, 110, 100) : sf::Color(240, 230, 220));
            DrawNetButton(connectBtn);                   // 连接按钮
        } else {
            DrawNetButton(hostBtn);                      // 创建房间
            DrawNetButton(joinBtn);                      // 加入房间
        }
    }
    // 等待连接状态（房主）
    else if (ns == NetState::HOST_WAITING) {
        sf::RectangleShape sb(sf::Vector2f(200, 90));
        sb.setPosition(730, 515);
        sb.setFillColor(sf::Color(50, 45, 35));
        sb.setOutlineColor(sf::Color(80, 70, 55));
        sb.setOutlineThickness(1);
        window->draw(sb);
        DrawText(L"\u7B49\u5F85\u8FDE\u63A5...", 830, 535, 16, sf::Color(200, 200, 150), true);  // "等待连接..."
        DrawText(L"\u672C\u673AIP:", 830, 561, 14, sf::Color(180, 180, 180), true);              // "本机IP:"
        DrawText(localIP, 830, 583, 16, sf::Color(100, 255, 100), true);                          // IP地址
        disconnectBtn.bounds.top = 615;
        DrawNetRedButton(disconnectBtn);                 // 红色断开按钮
    }
    // 已连接状态
    else if (ns == NetState::CONNECTED) {
        sf::RectangleShape sb(sf::Vector2f(200, 80));
        sb.setPosition(730, 515);
        sb.setFillColor(sf::Color(40, 60, 40));
        sb.setOutlineColor(sf::Color(80, 120, 80));
        sb.setOutlineThickness(1);
        window->draw(sb);
        DrawText(L"\u5DF2\u8FDE\u63A5", 830, 535, 18, sf::Color(100, 255, 100), true);           // "已连接"
        std::wstring st = (netSide == 0) ? L"\u4F60\u662F\u7EA2\u65B9" : L"\u4F60\u662F\u9ED1\u65B9";
        DrawText(st, 830, 570, 16,
                 (netSide == 0) ? sf::Color(255, 120, 120) : sf::Color(200, 200, 200), true);
        disconnectBtn.bounds.top = 605;
        DrawNetRedButton(disconnectBtn);
    }
}

// ==================== 教程面板 ====================
void UIManager::DrawTutorialPanel() {
    if (!fontLoaded) return;

    // 半透明背景（覆盖右侧面板）
    sf::RectangleShape bg(sf::Vector2f(440, 820));
    bg.setPosition(710, 0);
    bg.setFillColor(sf::Color(38, 28, 18, 250));
    bg.setOutlineColor(sf::Color(120, 100, 70));
    bg.setOutlineThickness(2);
    window->draw(bg);

    // 标题栏
    sf::RectangleShape hd(sf::Vector2f(440, 36));
    hd.setPosition(710, 0);
    hd.setFillColor(sf::Color(55, 42, 30));
    window->draw(hd);
    DrawTextWithShadow(L"\u65B0\u624B\u6559\u7A0B", 930, 18, 20, sf::Color(255, 220, 150), true);  // "新手教程"

    // 关闭按钮
    sf::RectangleShape cb(sf::Vector2f(50, 28));
    cb.setPosition(1090, 4);
    cb.setFillColor(sf::Color(180, 50, 50));
    cb.setOutlineColor(sf::Color(220, 80, 80));
    cb.setOutlineThickness(2);
    window->draw(cb);
    DrawTextWithShadow(L"\u2716", 1115, 18, 18, sf::Color::White, true);

    // 教程内容（逐行显示规则说明）
    const wchar_t* lines[] = {
        L"\u3010\u68CB\u76D8\u3011 9x9\u4EA4\u70B9\uFF0C\u68CB\u76D8\u659C\u653E45\u5EA6\uFF0C",
        L"\u53CC\u65B9\u5404\u6709\u4E00\u4E2A\u4E5D\u5BAB\uFF083x3\u533A\u57DF\uFF09\u3002",
        L"",
        L"\u3010\u8D70\u6CD5\u3011",
        L"\u5C06/\u5E05\uFF1A\u6A2A\u7AD6\u4E00\u6B65\uFF0C\u4E0D\u51FA\u4E5D\u5BAB\u3002",
        L"\u58EB/\u4ED5\uFF1A\u516B\u4E2A\u659C\u65B9\u5411\u4E00\u6B65\uFF0C\u4E0D\u51FA\u4E5D\u5BAB\u3002",
        L"\u8C61/\u76F8\uFF1A\u8D70\u7530\u5B57\uFF082\u00D72\u5BF9\u89D2\uFF09\uFF0C\u53EF\u8FC7\u6CB3\u3002",
        L"\u9A6C\uFF1A\u8D70\u65E5\u5B57\uFF0C\u6CE8\u610F\u8E6B\u9A6C\u817F\u3002",
        L"\u8F66\uFF1A\u6A2A\u7AD6\u76F4\u7EBF\u65E0\u9650\uFF0C\u4E0D\u53EF\u8D8A\u5B50\u3002",
        L"\u70AE\uFF1A\u79FB\u52A8\u5982\u8F66\uFF0C\u5403\u5B50\u987B\u9694\u4E00\u5B50\u3002",
        L"\u5175/\u5352\uFF1A\u53EA\u80FD\u659C\u524D\u4E00\u6B65\uFF0C\u4E0D\u53EF\u540E\u9000\u3002",
        L"",
        L"\u3010\u80DC\u8D1F\u3011\u5403\u6389\u5BF9\u65B9\u5C06/\u5E05\u5373\u80DC\u3002",
        L"\u88AB\u5C06\u519B\u4E14\u65E0\u8DEF\u53EF\u8D70\u4E3A\u7EDD\u6740\uFF0C\u5224\u8D1F\u3002",
        L"\u65E0\u5B50\u53EF\u8D70\u4E14\u672A\u88AB\u5C06\u519B\u4E3A\u56F0\u6BD9\uFF0C\u5224\u8D1F\u3002",
        L"",
        L"\u3010\u548C\u68CB\u3011\u53CC\u65B9\u540C\u610F\u548C\u68CB\uFF0C\u6216120\u6B65\u65E0\u5403\u5B50\u3002",
        L"\u3010\u8BA4\u8F93\u3011\u70B9\u51FB\u8BA4\u8F93\u6309\u94AE\uFF0C\u9009\u62E9\u8BA4\u8F93\u65B9\u5373\u53EF\u6295\u964D\u3002",
        L"\u3010\u6094\u68CB\u3011\u70B9\u51FB\u6094\u68CB\u8BF7\u6C42\uFF0C\u8054\u673A\u65F6\u9700\u5BF9\u65B9\u540C\u610F\u3002",
        L"\u3010\u91CD\u5F00\u3011\u70B9\u51FB\u91CD\u5F00\u6309\u94AE\uFF0C\u8054\u673A\u65F6\u9700\u5BF9\u65B9\u540C\u610F\u3002",
        L"",
        L"\u3010\u8054\u673A\u3011\u540CWiFi\u4E0B\uFF0C\u4E00\u4EBA\u521B\u5EFA\u623F\u95F4\uFF0C",
        L"\u53E6\u4E00\u4EBA\u8F93\u5165IP\u52A0\u5165\u3002\u4E3B\u673A\u6267\u7EA2\u5148\u624B\u3002",
        L"\u3010AI\u3011\u4E09\u6863\u96BE\u5EA6\u53EF\u9009\uFF0C\u8054\u673A\u65F6\u81EA\u52A8\u5173\u95ED\u3002",
    };

    float y = 50;
    for (const auto& line : lines) {
        bool hd = (line[0] && wcslen(line) > 0 && line[0] == L'\u3010');  // 以【开头的行是标题
        DrawText(line, 730, y, hd ? 16 : 14,
                 hd ? sf::Color(255, 200, 100) : sf::Color(210, 200, 180));
        y += hd ? 26 : 21;                               // 标题行高一些
    }
}

// ==================== 认输弹窗 ====================
// 本地双人对战时显示，让玩家选择哪方认输
void UIManager::DrawSurrenderPopup() {
    // 全屏半透明遮罩
    sf::RectangleShape ov(sf::Vector2f(1150, 820));
    ov.setPosition(0, 0);
    ov.setFillColor(sf::Color(0, 0, 0, 160));
    window->draw(ov);

    // 弹窗主体
    float pw = 340, ph = 200, px = (1150 - pw) / 2, py = 310;
    sf::RectangleShape p(sf::Vector2f(pw, ph));
    p.setPosition(px, py);
    p.setFillColor(sf::Color(50, 40, 30));
    p.setOutlineColor(sf::Color(120, 100, 70));
    p.setOutlineThickness(3);
    window->draw(p);

    float cx = px + pw / 2;
    DrawTextWithShadow(L"\u8BA4\u8F93", cx, py + 35, 26, sf::Color(255, 220, 150), true);       // "认输"
    DrawText(L"\u8BF7\u9009\u62E9\u8BA4\u8F93\u65B9", cx, py + 70, 14,                          // "请选择认输方"
             sf::Color(200, 190, 170), true);

    // 两个按钮：红方认输 / 黑方认输
    float bw = 130, bh = 44, by_ = py + 95, gap = 20, blx = cx - bw - gap / 2;
    surrenderRedBtn.bounds = sf::FloatRect(blx, by_, bw, bh);
    surrenderBlackBtn.bounds = sf::FloatRect(cx + gap / 2, by_, bw, bh);
    DrawSmallButton(surrenderRedBtn, sf::Color(180, 40, 40), sf::Color(240, 80, 80));
    DrawSmallButton(surrenderBlackBtn, sf::Color(40, 40, 40), sf::Color(100, 100, 100));

    DrawText(L"\u70B9\u51FB\u5916\u90E8\u53D6\u6D88", cx, py + ph + 18, 13,                     // "点击外部取消"
             sf::Color(150, 140, 130), true);
}

// ==================== 鼠标悬停更新 ====================
// 检测所有按钮是否被鼠标悬停，更新 hovered 状态
void UIManager::UpdateHover(float mx, float my) {
    undoBtn.hovered = undoBtn.bounds.contains(mx, my);
    restartBtn.hovered = restartBtn.bounds.contains(mx, my);
    aiBtn.hovered = aiBtn.bounds.contains(mx, my);
    difficultyBtn.hovered = difficultyBtn.bounds.contains(mx, my);
    onlineBtn.hovered = onlineBtn.bounds.contains(mx, my);
    hostBtn.hovered = hostBtn.bounds.contains(mx, my);
    joinBtn.hovered = joinBtn.bounds.contains(mx, my);
    connectBtn.hovered = connectBtn.bounds.contains(mx, my);
    disconnectBtn.hovered = disconnectBtn.bounds.contains(mx, my);
    undoAcceptBtn.hovered = undoAcceptBtn.bounds.contains(mx, my);
    undoRejectBtn.hovered = undoRejectBtn.bounds.contains(mx, my);
    restartAcceptBtn.hovered = restartAcceptBtn.bounds.contains(mx, my);
    restartRejectBtn.hovered = restartRejectBtn.bounds.contains(mx, my);
    tutorialBtn.hovered = tutorialBtn.bounds.contains(mx, my);
    surrenderBtn.hovered = surrenderBtn.bounds.contains(mx, my);
    surrenderRedBtn.hovered = surrenderRedBtn.bounds.contains(mx, my);
    surrenderBlackBtn.hovered = surrenderBlackBtn.bounds.contains(mx, my);
    surrenderAcceptBtn.hovered = surrenderAcceptBtn.bounds.contains(mx, my);
    surrenderRejectBtn.hovered = surrenderRejectBtn.bounds.contains(mx, my);
    drawOfferBtn.hovered = drawOfferBtn.bounds.contains(mx, my);
    drawAcceptBtn.hovered = drawAcceptBtn.bounds.contains(mx, my);
    drawRejectBtn.hovered = drawRejectBtn.bounds.contains(mx, my);
    gameOverRestartBtn.hovered = gameOverRestartBtn.bounds.contains(mx, my);
    closeBtn.hovered = closeBtn.bounds.contains(mx, my);
}

// 更新通知计时器（每秒衰减，到0后清空文字）
void UIManager::UpdateNotification(float dt) {
    if (notificationTimer > 0.f) {
        notificationTimer -= dt;
        if (notificationTimer <= 0.f) {
            notificationTimer = 0.f;
            notificationText.clear();
        }
    }
}
