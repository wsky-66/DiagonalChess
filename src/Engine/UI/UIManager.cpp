#include "Engine/UI/UIManager.h"
#include "Engine/Piece.h"
#include <sstream>
#include <algorithm>
#include <cstring>

UIManager::UIManager() {
    // init done in Init()
}

void UIManager::Init(sf::RenderWindow& w, sf::Font& f, bool ok) {
    window = &w; font = &f; fontLoaded = ok;
    undoBtn = {sf::FloatRect(730, 240, 95, 44), L"\u6094\u68CB", false, false};
    restartBtn = {sf::FloatRect(835, 240, 95, 44), L"\u91CD\u5F00", false, false};
    aiBtn = {sf::FloatRect(730, 290, 95, 44), L"AI: \u5173", false, false};
    difficultyBtn = {sf::FloatRect(835, 290, 95, 44), L"\u4E2D\u7B49", false, false};
    onlineBtn = {sf::FloatRect(730, 340, 95, 44), L"\u8054\u673A: \u5173", false, false};
    tutorialBtn = {sf::FloatRect(835, 340, 95, 44), L"\u6559\u7A0B", false, false};
    surrenderBtn = {sf::FloatRect(730, 390, 95, 44), L"\u8BA4\u8F93", false, false};
    drawOfferBtn = {sf::FloatRect(835, 390, 95, 44), L"\u548C\u68CB", false, false};
    hostBtn = {sf::FloatRect(730, 440, 95, 44), L"\u521B\u5EFA\u623F\u95F4", false, false};
    joinBtn = {sf::FloatRect(835, 440, 95, 44), L"\u52A0\u5165\u623F\u95F4", false, false};
    connectBtn = {sf::FloatRect(730, 530, 200, 44), L"\u8FDE\u63A5", false, false};
    disconnectBtn = {sf::FloatRect(730, 440, 200, 44), L"\u65AD\u5F00\u8FDE\u63A5", false, false};
    gameOverRestartBtn = {sf::FloatRect(0, 0, 200, 50), L"\u91CD\u65B0\u5F00\u59CB", false, false};
    undoAcceptBtn = {sf::FloatRect(730, 240, 95, 44), L"\u540C\u610F", false, false};
    undoRejectBtn = {sf::FloatRect(835, 240, 95, 44), L"\u62D2\u7EDD", false, false};
    restartAcceptBtn = {sf::FloatRect(730, 290, 95, 44), L"\u540C\u610F", false, false};
    restartRejectBtn = {sf::FloatRect(835, 290, 95, 44), L"\u62D2\u7EDD", false, false};
    surrenderRedBtn = {sf::FloatRect(0, 0, 180, 50), L"\u7EA2\u65B9\u8BA4\u8F93", false, false};
    surrenderBlackBtn = {sf::FloatRect(0, 0, 180, 50), L"\u9ED1\u65B9\u8BA4\u8F93", false, false};
    surrenderAcceptBtn = {sf::FloatRect(730, 240, 95, 44), L"\u540C\u610F", false, false};
    surrenderRejectBtn = {sf::FloatRect(835, 240, 95, 44), L"\u62D2\u7EDD", false, false};
    drawAcceptBtn = {sf::FloatRect(730, 240, 95, 44), L"\u540C\u610F", false, false};
    drawRejectBtn = {sf::FloatRect(835, 240, 95, 44), L"\u62D2\u7EDD", false, false};
}

void UIManager::DrawText(const std::wstring& text, float x, float y, unsigned sz, sf::Color c, bool center) {
    sf::Text t;
    t.setFont(*font);
    t.setString(text);
    t.setCharacterSize(sz);
    t.setFillColor(c);
    if (center) { auto b = t.getLocalBounds(); t.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f); }
    t.setPosition(x, y);
    window->draw(t);
}

void UIManager::DrawTextWithShadow(const std::wstring& text, float x, float y, unsigned sz, sf::Color c, bool center) {
    DrawText(text, x + 2, y + 2, sz, sf::Color(0, 0, 0, c.a / 2), center);
    DrawText(text, x, y, sz, c, center);
}

void UIManager::DrawBoard(const DiagonalChessBoard& board) {
    sf::ConvexShape bg(4);
    bg.setPoint(0, board.GridToScreen(0, 0)); bg.setPoint(1, board.GridToScreen(8, 0));
    bg.setPoint(2, board.GridToScreen(8, 8)); bg.setPoint(3, board.GridToScreen(0, 8));
    bg.setFillColor(sf::Color(210, 180, 140)); bg.setOutlineColor(sf::Color(80, 50, 20)); bg.setOutlineThickness(4);
    window->draw(bg);

    sf::ConvexShape inner(4);
    inner.setPoint(0, board.GridToScreen(1, 1)); inner.setPoint(1, board.GridToScreen(7, 1));
    inner.setPoint(2, board.GridToScreen(7, 7)); inner.setPoint(3, board.GridToScreen(1, 7));
    inner.setFillColor(sf::Color(220, 190, 150)); window->draw(inner);

    sf::Color lc(60, 40, 20);
    for (int r = 0; r < 9; r++)
        for (int c = 0; c < 8; c++) {
            auto p1 = board.GridToScreen(r, c), p2 = board.GridToScreen(r, c + 1);
            sf::Vertex l[] = {sf::Vertex(p1,lc), sf::Vertex(p2,lc)}; window->draw(l, 2, sf::Lines);
        }
    for (int c = 0; c < 9; c++)
        for (int r = 0; r < 8; r++) {
            auto p1 = board.GridToScreen(r, c), p2 = board.GridToScreen(r + 1, c);
            sf::Vertex l[] = {sf::Vertex(p1,lc), sf::Vertex(p2,lc)}; window->draw(l, 2, sf::Lines);
        }

    sf::Color pc(100, 60, 30, 180);
    auto drawPalace = [&](int r0, int c0) {
        for (int r = r0; r < r0 + 2; r++)
            for (int c = c0; c < c0 + 2; c++) {
                auto p1 = board.GridToScreen(r, c), p2 = board.GridToScreen(r + 1, c + 1);
                sf::Vertex d[] = {sf::Vertex(p1,pc), sf::Vertex(p2,pc)}; window->draw(d, 2, sf::Lines);
                auto p3 = board.GridToScreen(r + 1, c), p4 = board.GridToScreen(r, c + 1);
                sf::Vertex d2[] = {sf::Vertex(p3,pc), sf::Vertex(p4,pc)}; window->draw(d2, 2, sf::Lines);
            }
    };
    drawPalace(0, 6); drawPalace(6, 0);

    for (int r = 0; r < 9; r++)
        for (int c = 0; c < 9; c++) {
            sf::CircleShape dot(3); dot.setOrigin(3, 3);
            dot.setPosition(board.GridToScreen(r, c)); dot.setFillColor(sf::Color(80, 50, 20));
            window->draw(dot);
        }
}

void UIManager::DrawPieces(const DiagonalChessBoard& board, int selR, int selC,
                            const std::vector<sf::Vector2i>& validMoves) {
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (!board.IsOccupied(r, c)) continue;
            const auto& p = board.At(r, c);
            sf::Vector2f pos = board.GridToScreen(r, c);

            sf::CircleShape sh(PIECE_R + 2); sh.setOrigin(PIECE_R + 2, PIECE_R + 2);
            sh.setPosition(pos.x + 2, pos.y + 2); sh.setFillColor(sf::Color(0, 0, 0, 80));
            window->draw(sh);

            sf::CircleShape piece(PIECE_R); piece.setOrigin(PIECE_R, PIECE_R); piece.setPosition(pos);
            if (p.GetSide() == 0) { piece.setFillColor(sf::Color(180, 30, 30)); piece.setOutlineColor(sf::Color(120, 20, 20)); }
            else { piece.setFillColor(sf::Color(40, 40, 40)); piece.setOutlineColor(sf::Color(20, 20, 20)); }
            piece.setOutlineThickness(3); window->draw(piece);

            sf::CircleShape hl(PIECE_R - 4); hl.setOrigin(PIECE_R - 4, PIECE_R - 4);
            hl.setPosition(pos.x - 3, pos.y - 3); hl.setFillColor(sf::Color(255, 255, 255, 40));
            window->draw(hl);

            if (r == selR && c == selC) {
                sf::CircleShape ring(PIECE_R + 5); ring.setOrigin(PIECE_R + 5, PIECE_R + 5);
                ring.setPosition(pos); ring.setFillColor(sf::Color::Transparent);
                ring.setOutlineColor(sf::Color(255, 220, 0, 200)); ring.setOutlineThickness(4);
                window->draw(ring);
            }
            if (fontLoaded) {
                auto sym = p.GetSymbol();
                sf::Text t; t.setFont(*font); t.setString(sym); t.setCharacterSize(20);
                t.setFillColor(sf::Color::White);
                auto b = t.getLocalBounds(); t.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
                t.setPosition(pos); window->draw(t);
            }
        }
    }
    for (const auto& m : validMoves) {
        sf::Vector2f pos = board.GridToScreen(m.x, m.y);
        if (board.IsOccupied(m.x, m.y)) {
            sf::CircleShape ring(PIECE_R + 3); ring.setOrigin(PIECE_R + 3, PIECE_R + 3);
            ring.setPosition(pos); ring.setFillColor(sf::Color::Transparent);
            ring.setOutlineColor(sf::Color(255, 60, 60, 220)); ring.setOutlineThickness(4);
            window->draw(ring);
        } else {
            sf::CircleShape dot(10); dot.setOrigin(10, 10); dot.setPosition(pos);
            dot.setFillColor(sf::Color(80, 220, 80, 180)); window->draw(dot);
        }
    }
}

void UIManager::DrawUI(int currentTurn, bool gameOver, bool isDrawGame, bool surrendered, Side winner,
                        bool inCheck, bool aiThinking) {
    if (!fontLoaded) return;
    sf::RectangleShape tb(sf::Vector2f(420, 50)); tb.setPosition(715, 20);
    tb.setFillColor(sf::Color(65, 50, 35)); tb.setOutlineColor(sf::Color(90, 75, 55)); tb.setOutlineThickness(1);
    window->draw(tb);
    DrawTextWithShadow(L"\u5BF9\u89D2\u8C61\u68CB", 925, 45, 26, sf::Color(220, 200, 170), true);

    sf::RectangleShape turnBg(sf::Vector2f(420, 70)); turnBg.setPosition(715, 80);
    turnBg.setFillColor(sf::Color(60, 48, 34)); turnBg.setOutlineColor(sf::Color(85, 70, 50)); turnBg.setOutlineThickness(1);
    window->draw(turnBg);

    std::wstring turnText; sf::Color turnColor, indColor;
    if (gameOver) {
        if (isDrawGame) { turnText = L"\u548C\u68CB\uFF01"; turnColor = sf::Color(220, 220, 120); indColor = sf::Color(200, 200, 100); }
        else if (surrendered) { turnText = L"\u8BA4\u8F93"; turnColor = sf::Color(255, 150, 100); indColor = sf::Color(255, 150, 100); }
        else if (inCheck) { turnText = L"\u7EDD\u6740\uFF01"; turnColor = sf::Color(255, 215, 0); indColor = sf::Color(255, 215, 0); }
        else { turnText = L"\u56F0\u6BD9\uFF01"; turnColor = sf::Color(255, 180, 50); indColor = sf::Color(255, 180, 50); }
    } else {
        turnText = (currentTurn == 0) ? L"\u7EA2\u65B9\u8D70\u68CB" : L"\u9ED1\u65B9\u8D70\u68CB";
        turnColor = (currentTurn == 0) ? sf::Color(255, 120, 120) : sf::Color(220, 220, 220);
        indColor = (currentTurn == 0) ? sf::Color(220, 60, 60) : sf::Color(80, 80, 80);
    }
    sf::CircleShape ind(12); ind.setOrigin(12, 12); ind.setPosition(745, 115);
    ind.setFillColor(indColor); ind.setOutlineColor(sf::Color(255, 255, 255, 80)); ind.setOutlineThickness(2);
    window->draw(ind);
    DrawTextWithShadow(turnText, 935, 115, 30, turnColor, true);

    if (!gameOver && inCheck) {
        sf::RectangleShape cb(sf::Vector2f(140, 40)); cb.setPosition(855, 160);
        cb.setFillColor(sf::Color(180, 40, 40, 220)); cb.setOutlineColor(sf::Color(255, 120, 120)); cb.setOutlineThickness(2);
        window->draw(cb);
        DrawTextWithShadow(L"\u5C06\u519B\uFF01", 925, 180, 24, sf::Color(255, 255, 150), true);
    }
    if (aiThinking) DrawText(L"AI\u601D\u8003\u4E2D...", 925, 210, 18, sf::Color(200, 180, 150), true);
}

void UIManager::DrawNotification() {
    if (!notificationText.empty() && notificationTimer > 0.f) {
        float a = std::min(notificationTimer / 3.f, 1.f) * 255.f;
        sf::RectangleShape bg(sf::Vector2f(300, 30)); bg.setPosition(820, 170);
        bg.setFillColor(sf::Color(180, 60, 30, (sf::Uint8)(a * 0.6f)));
        bg.setOutlineColor(sf::Color(255, 100, 50, (sf::Uint8)(a * 0.8f))); bg.setOutlineThickness(2);
        window->draw(bg);
        DrawTextWithShadow(notificationText, 970, 185, 16, sf::Color(255, 100, 60, (sf::Uint8)a), true);
    }
}

void UIManager::DrawButton(const UIButton& btn) {
    sf::RectangleShape sh(sf::Vector2f(btn.bounds.width, btn.bounds.height));
    sh.setPosition(btn.bounds.left + 2, btn.bounds.top + 2); sh.setFillColor(sf::Color(20, 15, 10, 100));
    window->draw(sh);
    sf::RectangleShape r(sf::Vector2f(btn.bounds.width, btn.bounds.height));
    r.setPosition(btn.bounds.left, btn.bounds.top);
    if (btn.disabled) { r.setFillColor(sf::Color(70, 60, 48)); r.setOutlineColor(sf::Color(90, 78, 62)); }
    else if (btn.hovered) { r.setFillColor(sf::Color(110, 92, 72)); r.setOutlineColor(sf::Color(160, 140, 110)); }
    else { r.setFillColor(sf::Color(80, 66, 52)); r.setOutlineColor(sf::Color(130, 112, 88)); }
    r.setOutlineThickness(3); window->draw(r);
    if (fontLoaded) {
        sf::Text t; t.setFont(*font); t.setString(btn.label); t.setCharacterSize(18);
        t.setFillColor(btn.disabled ? sf::Color(110, 100, 88) : sf::Color(245, 235, 220));
        auto b = t.getLocalBounds(); t.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
        t.setPosition(btn.bounds.left + btn.bounds.width / 2.f, btn.bounds.top + btn.bounds.height / 2.f);
        window->draw(t);
    }
}

void UIManager::DrawSmallButton(const UIButton& btn, sf::Color fill, sf::Color outline) {
    sf::RectangleShape r(sf::Vector2f(btn.bounds.width, btn.bounds.height));
    r.setPosition(btn.bounds.left, btn.bounds.top);
    r.setFillColor(btn.hovered ? sf::Color(fill.r + 40, fill.g + 30, fill.b + 20) : fill);
    r.setOutlineColor(outline); r.setOutlineThickness(2); window->draw(r);
    sf::Text t; t.setFont(*font); t.setString(btn.label); t.setCharacterSize(14);
    t.setFillColor(sf::Color::White);
    auto b = t.getLocalBounds(); t.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
    t.setPosition(btn.bounds.left + btn.bounds.width / 2.f, btn.bounds.top + btn.bounds.height / 2.f);
    window->draw(t);
}

void UIManager::DrawNetButton(const UIButton& btn) {
    sf::RectangleShape sh(sf::Vector2f(btn.bounds.width, btn.bounds.height));
    sh.setPosition(btn.bounds.left + 2, btn.bounds.top + 2); sh.setFillColor(sf::Color(20, 15, 10, 100));
    window->draw(sh);
    sf::RectangleShape r(sf::Vector2f(btn.bounds.width, btn.bounds.height));
    r.setPosition(btn.bounds.left, btn.bounds.top);
    r.setFillColor(btn.hovered ? sf::Color(80, 120, 80) : sf::Color(60, 90, 60));
    r.setOutlineColor(sf::Color(100, 150, 100)); r.setOutlineThickness(2); window->draw(r);
    sf::Text t; t.setFont(*font); t.setString(btn.label); t.setCharacterSize(18);
    t.setFillColor(sf::Color(230, 255, 230));
    auto b = t.getLocalBounds(); t.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
    t.setPosition(btn.bounds.left + btn.bounds.width / 2.f, btn.bounds.top + btn.bounds.height / 2.f);
    window->draw(t);
}

void UIManager::DrawNetRedButton(const UIButton& btn) {
    sf::RectangleShape r(sf::Vector2f(btn.bounds.width, btn.bounds.height));
    r.setPosition(btn.bounds.left, btn.bounds.top);
    r.setFillColor(btn.hovered ? sf::Color(150, 60, 60) : sf::Color(120, 40, 40));
    r.setOutlineColor(sf::Color(180, 80, 80)); r.setOutlineThickness(2); window->draw(r);
    sf::Text t; t.setFont(*font); t.setString(btn.label); t.setCharacterSize(18);
    t.setFillColor(sf::Color(255, 220, 220));
    auto b = t.getLocalBounds(); t.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
    t.setPosition(btn.bounds.left + btn.bounds.width / 2.f, btn.bounds.top + btn.bounds.height / 2.f);
    window->draw(t);
}

void UIManager::DrawButtons() {
    DrawButton(undoBtn); DrawButton(restartBtn); DrawButton(aiBtn); DrawButton(difficultyBtn);
    DrawButton(onlineBtn); DrawButton(tutorialBtn); DrawButton(surrenderBtn); DrawButton(drawOfferBtn);
}

void UIManager::DrawRequestPopups(bool undoRecv, bool restartRecv, bool surrRecv, bool drawRecv) {
    if (undoRecv) {
        sf::RectangleShape bg(sf::Vector2f(200, 70)); bg.setPosition(940, 240);
        bg.setFillColor(sf::Color(180, 130, 30, 220)); bg.setOutlineColor(sf::Color(255, 180, 50)); bg.setOutlineThickness(2);
        window->draw(bg);
        DrawText(L"\u5BF9\u65B9\u8BF7\u6C42\u6094\u68CB", 1040, 260, 16, sf::Color(255, 255, 200), true);
        undoAcceptBtn.bounds = sf::FloatRect(940, 275, 95, 44);
        undoRejectBtn.bounds = sf::FloatRect(1045, 275, 95, 44);
        DrawSmallButton(undoAcceptBtn, sf::Color(60, 140, 60), sf::Color(80, 180, 80));
        DrawSmallButton(undoRejectBtn, sf::Color(160, 50, 50), sf::Color(200, 80, 80));
    }
    if (restartRecv) {
        sf::RectangleShape bg(sf::Vector2f(200, 70)); bg.setPosition(940, 290);
        bg.setFillColor(sf::Color(180, 130, 30, 220)); bg.setOutlineColor(sf::Color(255, 180, 50)); bg.setOutlineThickness(2);
        window->draw(bg);
        DrawText(L"\u5BF9\u65B9\u8BF7\u6C42\u91CD\u5F00", 1040, 310, 16, sf::Color(255, 255, 200), true);
        restartAcceptBtn.bounds = sf::FloatRect(940, 325, 95, 44);
        restartRejectBtn.bounds = sf::FloatRect(1045, 325, 95, 44);
        DrawSmallButton(restartAcceptBtn, sf::Color(60, 140, 60), sf::Color(80, 180, 80));
        DrawSmallButton(restartRejectBtn, sf::Color(160, 50, 50), sf::Color(200, 80, 80));
    }
    if (surrRecv) {
        sf::RectangleShape bg(sf::Vector2f(200, 70)); bg.setPosition(940, 340);
        bg.setFillColor(sf::Color(180, 40, 40, 220)); bg.setOutlineColor(sf::Color(255, 80, 80)); bg.setOutlineThickness(2);
        window->draw(bg);
        DrawText(L"\u5BF9\u65B9\u8BA4\u8F93\u4E86", 1040, 360, 16, sf::Color(255, 220, 200), true);
        surrenderAcceptBtn.bounds = sf::FloatRect(940, 375, 95, 44);
        surrenderRejectBtn.bounds = sf::FloatRect(1045, 375, 95, 44);
        DrawSmallButton(surrenderAcceptBtn, sf::Color(60, 140, 60), sf::Color(80, 180, 80));
        DrawSmallButton(surrenderRejectBtn, sf::Color(160, 50, 50), sf::Color(200, 80, 80));
    }
    if (drawRecv) {
        sf::RectangleShape bg(sf::Vector2f(200, 70)); bg.setPosition(940, 390);
        bg.setFillColor(sf::Color(180, 180, 40, 220)); bg.setOutlineColor(sf::Color(255, 255, 80)); bg.setOutlineThickness(2);
        window->draw(bg);
        DrawText(L"\u5BF9\u65B9\u8BF7\u6C42\u548C\u68CB", 1040, 410, 16, sf::Color(255, 255, 200), true);
        drawAcceptBtn.bounds = sf::FloatRect(940, 425, 95, 44);
        drawRejectBtn.bounds = sf::FloatRect(1045, 425, 95, 44);
        DrawSmallButton(drawAcceptBtn, sf::Color(60, 140, 60), sf::Color(80, 180, 80));
        DrawSmallButton(drawRejectBtn, sf::Color(160, 50, 50), sf::Color(200, 80, 80));
    }
}

void UIManager::DrawMoveLog(const std::vector<std::wstring>& logs) {
    if (!fontLoaded) return;
    float logY = logDividerY, logH = 810.f - logDividerY;
    sf::RectangleShape bg(sf::Vector2f(420, logH)); bg.setPosition(715, logY);
    bg.setFillColor(sf::Color(55, 44, 32)); bg.setOutlineColor(sf::Color(85, 70, 52)); bg.setOutlineThickness(1);
    window->draw(bg);
    sf::RectangleShape hd(sf::Vector2f(420, 30)); hd.setPosition(715, logY); hd.setFillColor(sf::Color(70, 56, 40));
    window->draw(hd);
    DrawTextWithShadow(L"\u8D70\u68CB\u8BB0\u5F55", 925, logY + 15, 16, sf::Color(230, 210, 180), true);

    int total = (int)logs.size(), vis = (int)((logH - 40) / 22);
    int mx = std::max(0, total - vis);
    if (logScrollOffset > mx) logScrollOffset = mx;
    if (logScrollOffset < 0) logScrollOffset = 0;
    int start = std::max(0, total - vis - logScrollOffset);
    float y = logY + 40;
    for (int i = start; i < total; i++) {
        if (i % 2 == 0) {
            sf::RectangleShape rb(sf::Vector2f(410, 20)); rb.setPosition(720, y - 2);
            rb.setFillColor(sf::Color(65, 52, 38, 100)); window->draw(rb);
        }
        std::wostringstream oss; oss << (i + 1) << L". " << logs[i];
        DrawText(oss.str(), 730, y, 13, (i % 2 == 0) ? sf::Color(255, 140, 140) : sf::Color(220, 220, 220));
        y += 22;
    }
}

void UIManager::DrawGameOverEffect(float timer, bool isDraw, Side winner, bool surrendered,
                                    bool checkmate, int mwc, bool agreedDraw, bool insuf) {
    float alpha = std::min(timer * 200.f, 180.f);
    sf::RectangleShape ov(sf::Vector2f(1150, 820)); ov.setPosition(0, 0);
    ov.setFillColor(sf::Color(0, 0, 0, (sf::Uint8)alpha)); window->draw(ov);
    if (timer <= 0.5f) return;

    float ta = std::min((timer - 0.5f) * 400.f, 255.f);
    std::wstring mt, st; sf::Color mc;
    if (isDraw) {
        mt = L"\u548C\u68CB"; mc = sf::Color(255, 255, 100, (sf::Uint8)ta);
        if (mwc >= DRAW_LIMIT) st = L"\u81EA\u7136\u9650\u7740\u548C\u68CB";
        else if (insuf) st = L"\u5B50\u529B\u4E0D\u8DB3\u548C\u68CB";
        else if (agreedDraw) st = L"\u53CC\u65B9\u540C\u610F\u548C\u68CB";
        else st = L"\u56F0\u6BD9\u548C\u68CB";
    } else {
        Side loser = (winner == Side::RED) ? Side::BLACK : Side::RED;
        if (surrendered) {
            mt = (winner == Side::RED) ? L"\u7EA2\u65B9\u80DC\u5229" : L"\u9ED1\u65B9\u80DC\u5229";
            mc = (winner == Side::RED) ? sf::Color(255, 80, 80, (sf::Uint8)ta) : sf::Color(220, 220, 220, (sf::Uint8)ta);
            st = (loser == Side::RED) ? L"\u7EA2\u65B9\u8BA4\u8F93" : L"\u9ED1\u65B9\u8BA4\u8F93";
        } else if (checkmate) {
            mt = L"\u7EDD\u6740"; mc = sf::Color(255, 215, 0, (sf::Uint8)ta);
            st = (winner == Side::RED) ? L"\u7EA2\u65B9\u7EDD\u6740\u9ED1\u65B9" : L"\u9ED1\u65B9\u7EDD\u6740\u7EA2\u65B9";
        } else {
            mt = L"\u56F0\u6BD9"; mc = sf::Color(255, 180, 50, (sf::Uint8)ta);
            st = (loser == Side::RED) ? L"\u7EA2\u65B9\u88AB\u56F0\u6BD9" : L"\u9ED1\u65B9\u88AB\u56F0\u6BD9";
        }
    }
    DrawTextWithShadow(mt, 575, 360, 72, mc, true);
    DrawTextWithShadow(st, 575, 430, 28, sf::Color(200, 200, 200, (sf::Uint8)ta), true);

    float bx = 475, by = 500;
    gameOverRestartBtn.bounds.left = bx; gameOverRestartBtn.bounds.top = by;
    sf::RectangleShape bs(sf::Vector2f(200, 50)); bs.setPosition(bx + 3, by + 3);
    bs.setFillColor(sf::Color(0, 0, 0, (sf::Uint8)(ta * 0.5f))); window->draw(bs);
    sf::RectangleShape br(sf::Vector2f(200, 50)); br.setPosition(bx, by);
    br.setFillColor(gameOverRestartBtn.hovered ? sf::Color(140, 110, 80, (sf::Uint8)ta) : sf::Color(100, 80, 60, (sf::Uint8)ta));
    br.setOutlineColor(sf::Color(180, 150, 110, (sf::Uint8)ta)); br.setOutlineThickness(2); window->draw(br);
    DrawTextWithShadow(L"\u91CD\u65B0\u5F00\u59CB", bx + 100, by + 25, 22, sf::Color(255, 255, 255, (sf::Uint8)ta), true);
}

void UIManager::DrawNetUI(bool netMode, NetState ns, const std::wstring& localIP,
                           const std::wstring& inputIP, bool showIP, int netSide) {
    if (!fontLoaded || !netMode) return;
    if (ns == NetState::OFFLINE) {
        if (showIP) {
            sf::RectangleShape ib(sf::Vector2f(200, 35)); ib.setPosition(730, 440);
            ib.setFillColor(sf::Color(40, 35, 28)); ib.setOutlineColor(sf::Color(120, 100, 80)); ib.setOutlineThickness(2);
            window->draw(ib);
            std::wstring d = inputIP.empty() ? L"\u8F93\u5165IP\u5730\u5740..." : inputIP;
            DrawText(d, 740, 447, 15, inputIP.empty() ? sf::Color(120, 110, 100) : sf::Color(240, 230, 220));
            DrawNetButton(connectBtn);
        } else { DrawNetButton(hostBtn); DrawNetButton(joinBtn); }
    } else if (ns == NetState::HOST_WAITING) {
        sf::RectangleShape sb(sf::Vector2f(200, 90)); sb.setPosition(730, 500);
        sb.setFillColor(sf::Color(50, 45, 35)); sb.setOutlineColor(sf::Color(80, 70, 55)); sb.setOutlineThickness(1);
        window->draw(sb);
        DrawText(L"\u7B49\u5F85\u8FDE\u63A5...", 830, 520, 16, sf::Color(200, 200, 150), true);
        DrawText(L"\u672C\u673AIP:", 830, 546, 14, sf::Color(180, 180, 180), true);
        DrawText(localIP, 830, 568, 16, sf::Color(100, 255, 100), true);
        disconnectBtn.bounds.top = 600; DrawNetRedButton(disconnectBtn);
    } else if (ns == NetState::CONNECTED) {
        sf::RectangleShape sb(sf::Vector2f(200, 80)); sb.setPosition(730, 500);
        sb.setFillColor(sf::Color(40, 60, 40)); sb.setOutlineColor(sf::Color(80, 120, 80)); sb.setOutlineThickness(1);
        window->draw(sb);
        DrawText(L"\u5DF2\u8FDE\u63A5", 830, 520, 18, sf::Color(100, 255, 100), true);
        std::wstring st = (netSide == 0) ? L"\u4F60\u662F\u7EA2\u65B9" : L"\u4F60\u662F\u9ED1\u65B9";
        DrawText(st, 830, 555, 16, (netSide == 0) ? sf::Color(255, 120, 120) : sf::Color(200, 200, 200), true);
        disconnectBtn.bounds.top = 590; DrawNetRedButton(disconnectBtn);
    }
}

void UIManager::DrawTutorialPanel() {
    if (!fontLoaded) return;
    sf::RectangleShape bg(sf::Vector2f(440, 820)); bg.setPosition(710, 0);
    bg.setFillColor(sf::Color(38, 28, 18, 250)); bg.setOutlineColor(sf::Color(120, 100, 70)); bg.setOutlineThickness(2);
    window->draw(bg);
    sf::RectangleShape hd(sf::Vector2f(440, 36)); hd.setPosition(710, 0); hd.setFillColor(sf::Color(55, 42, 30));
    window->draw(hd);
    DrawTextWithShadow(L"\u65B0\u624B\u6559\u7A0B", 930, 18, 20, sf::Color(255, 220, 150), true);
    sf::RectangleShape cb(sf::Vector2f(50, 28)); cb.setPosition(1090, 4);
    cb.setFillColor(sf::Color(180, 50, 50)); cb.setOutlineColor(sf::Color(220, 80, 80)); cb.setOutlineThickness(2);
    window->draw(cb);
    DrawTextWithShadow(L"\u2716", 1115, 18, 18, sf::Color::White, true);

    const wchar_t* lines[] = {
        L"\u3010\u68CB\u76D8\u3011 9x9\u4EA4\u70B9\uFF0C\u68CB\u76D8\u659C\u653E45\u5EA6\uFF0C",
        L"\u53CC\u65B9\u5404\u6709\u4E00\u4E2A\u4E5D\u5BAB\uFF083x3\u533A\u57DF\uFF09\u3002",
        L"", L"\u3010\u8D70\u6CD5\u3011",
        L"\u5C06/\u5E05\uFF1A\u6A2A\u7AD6\u4E00\u6B65\uFF0C\u4E0D\u51FA\u4E5D\u5BAB\u3002",
        L"\u58EB/\u4ED5\uFF1A\u516B\u4E2A\u659C\u65B9\u5411\u4E00\u6B65\uFF0C\u4E0D\u51FA\u4E5D\u5BAB\u3002",
        L"\u8C61/\u76F8\uFF1A\u8D70\u7530\u5B57\uFF082\u00D72\u5BF9\u89D2\uFF09\uFF0C\u53EF\u8FC7\u6CB3\u3002",
        L"\u9A6C\uFF1A\u8D70\u65E5\u5B57\uFF0C\u6CE8\u610F\u8E6B\u9A6C\u817F\u3002",
        L"\u8F66\uFF1A\u6A2A\u7AD6\u76F4\u7EBF\u65E0\u9650\uFF0C\u4E0D\u53EF\u8D8A\u5B50\u3002",
        L"\u70AE\uFF1A\u79FB\u52A8\u5982\u8F66\uFF0C\u5403\u5B50\u987B\u9694\u4E00\u5B50\u3002",
        L"\u5175/\u5352\uFF1A\u53EA\u80FD\u659C\u524D\u4E00\u6B65\uFF0C\u4E0D\u53EF\u540E\u9000\u3002",
        L"", L"\u3010\u80DC\u8D1F\u3011\u5403\u6389\u5BF9\u65B9\u5C06/\u5E05\u5373\u80DC\u3002",
        L"\u88AB\u5C06\u519B\u4E14\u65E0\u8DEF\u53EF\u8D70\u4E3A\u7EDD\u6740\uFF0C\u5224\u8D1F\u3002",
        L"\u65E0\u5B50\u53EF\u8D70\u4E14\u672A\u88AB\u5C06\u519B\u4E3A\u56F0\u6BD9\uFF0C\u5224\u8D1F\u3002",
        L"", L"\u3010\u548C\u68CB\u3011\u53CC\u65B9\u540C\u610F\u548C\u68CB\uFF0C\u6216120\u6B65\u65E0\u5403\u5B50\u3002\u8054\u673A\u65F6\u9700\u53CC\u65B9\u540C\u610F\u3002",
        L"\u3010\u8BA4\u8F93\u3011\u70B9\u51FB\u8BA4\u8F93\u6309\u94AE\uFF0C\u9009\u62E9\u8BA4\u8F93\u65B9\u5373\u53EF\u6295\u964D\u3002",
        L"\u3010\u6094\u68CB\u3011\u70B9\u51FB\u6094\u68CB\u8BF7\u6C42\uFF0C\u8054\u673A\u65F6\u9700\u5BF9\u65B9\u540C\u610F\u3002",
        L"\u3010\u91CD\u5F00\u3011\u70B9\u51FB\u91CD\u5F00\u6309\u94AE\uFF0C\u8054\u673A\u65F6\u9700\u5BF9\u65B9\u540C\u610F\u3002",
        L"", L"\u3010\u8054\u673A\u3011\u540CWiFi\u4E0B\uFF0C\u4E00\u4EBA\u521B\u5EFA\u623F\u95F4\uFF0C",
        L"\u53E6\u4E00\u4EBA\u8F93\u5165IP\u52A0\u5165\u3002\u4E3B\u673A\u6267\u7EA2\u5148\u624B\u3002",
        L"\u3010AI\u3011\u4E09\u6863\u96BE\u5EA6\u53EF\u9009\uFF0C\u8054\u673A\u65F6\u81EA\u52A8\u5173\u95ED\u3002",
    };
    float y = 50;
    for (const auto& line : lines) {
        bool hd = (line[0] && wcslen(line) > 0 && line[0] == L'\u3010');
        DrawText(line, 730, y, hd ? 16 : 14, hd ? sf::Color(255, 200, 100) : sf::Color(210, 200, 180));
        y += hd ? 26 : 21;
    }
}

void UIManager::DrawSurrenderPopup() {
    sf::RectangleShape ov(sf::Vector2f(1150, 820)); ov.setPosition(0, 0);
    ov.setFillColor(sf::Color(0, 0, 0, 160)); window->draw(ov);
    float pw = 340, ph = 200, px = (1150 - pw) / 2, py = 310;
    sf::RectangleShape p(sf::Vector2f(pw, ph)); p.setPosition(px, py);
    p.setFillColor(sf::Color(50, 40, 30)); p.setOutlineColor(sf::Color(120, 100, 70)); p.setOutlineThickness(3);
    window->draw(p);
    float cx = px + pw / 2;
    DrawTextWithShadow(L"\u8BA4\u8F93", cx, py + 35, 26, sf::Color(255, 220, 150), true);
    DrawText(L"\u8BF7\u9009\u62E9\u8BA4\u8F93\u65B9", cx, py + 70, 14, sf::Color(200, 190, 170), true);
    float bw = 130, bh = 44, by_ = py + 95, gap = 20, blx = cx - bw - gap / 2;
    surrenderRedBtn.bounds = sf::FloatRect(blx, by_, bw, bh);
    surrenderBlackBtn.bounds = sf::FloatRect(cx + gap / 2, by_, bw, bh);
    DrawSmallButton(surrenderRedBtn, sf::Color(180, 40, 40), sf::Color(240, 80, 80));
    DrawSmallButton(surrenderBlackBtn, sf::Color(40, 40, 40), sf::Color(100, 100, 100));
    DrawText(L"\u70B9\u51FB\u5916\u90E8\u53D6\u6D88", cx, py + ph + 18, 13, sf::Color(150, 140, 130), true);
}

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
}

void UIManager::UpdateNotification(float dt) {
    if (notificationTimer > 0.f) { notificationTimer -= dt; if (notificationTimer <= 0.f) { notificationTimer = 0.f; notificationText.clear(); } }
}
