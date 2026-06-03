#include "Engine/UI/UIManager.h"
#include <sstream>
#include <algorithm>

UIManager::UIManager()
    : window(nullptr), font(nullptr), fontLoaded(false), showTutorial(false),
      logScrollOffset(0), logDividerY(440.f), draggingLogDivider(false),
      showIPInput(false), showSurrenderPopup(false), notificationTimer(0.f) {}

void UIManager::Init(sf::RenderWindow& w, sf::Font& f, bool fontOk) {
    window = &w;
    font = &f;
    fontLoaded = fontOk;

    undoBtn = {sf::FloatRect(730, 240, 95, 44), L"\u6094\u68cb", false, false};
    restartBtn = {sf::FloatRect(835, 240, 95, 44), L"\u91cd\u5f00", false, false};
    aiBtn = {sf::FloatRect(730, 290, 95, 44), L"AI: \u5173", false, false};
    difficultyBtn = {sf::FloatRect(835, 290, 95, 44), L"\u4e2d\u7b49", false, false};
    onlineBtn = {sf::FloatRect(730, 340, 95, 44), L"\u8054\u673a: \u5173", false, false};
    tutorialBtn = {sf::FloatRect(835, 340, 95, 44), L"\u6559\u7a0b", false, false};
    surrenderBtn = {sf::FloatRect(730, 390, 95, 44), L"\u8ba4\u8f93", false, false};
    drawOfferBtn = {sf::FloatRect(835, 390, 95, 44), L"\u548c\u68cb", false, false};
    hostBtn = {sf::FloatRect(730, 440, 95, 44), L"\u521b\u5efa\u623f\u95f4", false, false};
    joinBtn = {sf::FloatRect(835, 440, 95, 44), L"\u52a0\u5165\u623f\u95f4", false, false};
    connectBtn = {sf::FloatRect(730, 530, 200, 44), L"\u8fde\u63a5", false, false};
    disconnectBtn = {sf::FloatRect(730, 440, 200, 44), L"\u65ad\u5f00\u8fde\u63a5", false, false};
    gameOverRestartBtn = {sf::FloatRect(0, 0, 200, 50), L"\u91cd\u65b0\u5f00\u59cb", false, false};
    undoAcceptBtn = {sf::FloatRect(730, 240, 95, 44), L"\u540c\u610f", false, false};
    undoRejectBtn = {sf::FloatRect(835, 240, 95, 44), L"\u62d2\u7edd", false, false};
    restartAcceptBtn = {sf::FloatRect(730, 290, 95, 44), L"\u540c\u610f", false, false};
    restartRejectBtn = {sf::FloatRect(835, 290, 95, 44), L"\u62d2\u7edd", false, false};
    surrenderRedBtn = {sf::FloatRect(0, 0, 180, 50), L"\u7ea2\u65b9\u8ba4\u8f93", false, false};
    surrenderBlackBtn = {sf::FloatRect(0, 0, 180, 50), L"\u9ed1\u65b9\u8ba4\u8f93", false, false};
    surrenderAcceptBtn = {sf::FloatRect(730, 240, 95, 44), L"\u540c\u610f", false, false};
    surrenderRejectBtn = {sf::FloatRect(835, 240, 95, 44), L"\u62d2\u7edd", false, false};
    drawAcceptBtn = {sf::FloatRect(730, 240, 95, 44), L"\u540c\u610f", false, false};
    drawRejectBtn = {sf::FloatRect(835, 240, 95, 44), L"\u62d2\u7edd", false, false};
}

void UIManager::DrawText(const std::wstring& text, float x, float y,
                          unsigned int charSize, sf::Color color, bool center) {
    sf::Text t;
    t.setFont(*font);
    t.setString(text);
    t.setCharacterSize(charSize);
    t.setFillColor(color);
    if (center) {
        sf::FloatRect bounds = t.getLocalBounds();
        t.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
    }
    t.setPosition(x, y);
    window->draw(t);
}

void UIManager::DrawTextWithShadow(const std::wstring& text, float x, float y,
                                    unsigned int charSize, sf::Color color, bool center) {
    sf::Color shadowColor(0, 0, 0, color.a / 2);
    DrawText(text, x + 2, y + 2, charSize, shadowColor, center);
    DrawText(text, x, y, charSize, color, center);
}

void UIManager::DrawBoard(const DiagonalChessBoard& board) {
    sf::ConvexShape bg(4);
    bg.setPoint(0, board.GridToScreen(0, 0));
    bg.setPoint(1, board.GridToScreen(8, 0));
    bg.setPoint(2, board.GridToScreen(8, 8));
    bg.setPoint(3, board.GridToScreen(0, 8));
    bg.setFillColor(sf::Color(210, 180, 140));
    bg.setOutlineColor(sf::Color(80, 50, 20));
    bg.setOutlineThickness(4);
    window->draw(bg);

    sf::ConvexShape innerBg(4);
    innerBg.setPoint(0, board.GridToScreen(1, 1));
    innerBg.setPoint(1, board.GridToScreen(7, 1));
    innerBg.setPoint(2, board.GridToScreen(7, 7));
    innerBg.setPoint(3, board.GridToScreen(1, 7));
    innerBg.setFillColor(sf::Color(220, 190, 150));

    window->draw(innerBg);

    sf::Color lineColor(60, 40, 20);
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 8; c++) {
            sf::Vector2f p1 = board.GridToScreen(r, c);
            sf::Vector2f p2 = board.GridToScreen(r, c + 1);
            sf::Vertex line[] = {sf::Vertex(p1, lineColor), sf::Vertex(p2, lineColor)};
            window->draw(line, 2, sf::Lines);
        }
    }
    for (int c = 0; c < 9; c++) {
        for (int r = 0; r < 8; r++) {
            sf::Vector2f p1 = board.GridToScreen(r, c);
            sf::Vector2f p2 = board.GridToScreen(r + 1, c);
            sf::Vertex line[] = {sf::Vertex(p1, lineColor), sf::Vertex(p2, lineColor)};
            window->draw(line, 2, sf::Lines);
        }
    }

    sf::Color palaceColor(100, 60, 30, 180);
    for (int r = 0; r < 2; r++) {
        for (int c = 6; c < 8; c++) {
            sf::Vector2f p1 = board.GridToScreen(r, c);
            sf::Vector2f p2 = board.GridToScreen(r + 1, c + 1);
            sf::Vertex d1[] = {sf::Vertex(p1, palaceColor), sf::Vertex(p2, palaceColor)};
            window->draw(d1, 2, sf::Lines);
            sf::Vector2f p3 = board.GridToScreen(r + 1, c);
            sf::Vector2f p4 = board.GridToScreen(r, c + 1);
            sf::Vertex d2[] = {sf::Vertex(p3, palaceColor), sf::Vertex(p4, palaceColor)};
            window->draw(d2, 2, sf::Lines);
        }
    }
    for (int r = 6; r < 8; r++) {
        for (int c = 0; c < 2; c++) {
            sf::Vector2f p1 = board.GridToScreen(r, c);
            sf::Vector2f p2 = board.GridToScreen(r + 1, c + 1);
            sf::Vertex d1[] = {sf::Vertex(p1, palaceColor), sf::Vertex(p2, palaceColor)};
            window->draw(d1, 2, sf::Lines);
            sf::Vector2f p3 = board.GridToScreen(r + 1, c);
            sf::Vector2f p4 = board.GridToScreen(r, c + 1);
            sf::Vertex d2[] = {sf::Vertex(p3, palaceColor), sf::Vertex(p4, palaceColor)};
            window->draw(d2, 2, sf::Lines);
        }
    }

    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            sf::Vector2f pos = board.GridToScreen(r, c);
            sf::CircleShape dot(3);
            dot.setOrigin(3, 3);
            dot.setPosition(pos);
            dot.setFillColor(sf::Color(80, 50, 20));
            window->draw(dot);
        }
    }
}

void UIManager::DrawPieces(const DiagonalChessBoard& board, int selectedR, int selectedC,
                            const std::vector<sf::Vector2i>& validMoves) {
    const auto& b = board.GetBoard();
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (!b[r][c].alive) continue;
            sf::Vector2f pos = board.GridToScreen(r, c);

            sf::CircleShape shadow(PIECE_R + 2);
            shadow.setOrigin(PIECE_R + 2, PIECE_R + 2);
            shadow.setPosition(pos.x + 2, pos.y + 2);
            shadow.setFillColor(sf::Color(0, 0, 0, 80));
            window->draw(shadow);

            sf::CircleShape piece(PIECE_R);
            piece.setOrigin(PIECE_R, PIECE_R);
            piece.setPosition(pos);
            if (b[r][c].side == Side::RED) {
                piece.setFillColor(sf::Color(180, 30, 30));
                piece.setOutlineColor(sf::Color(120, 20, 20));
            } else {
                piece.setFillColor(sf::Color(40, 40, 40));
                piece.setOutlineColor(sf::Color(20, 20, 20));
            }
            piece.setOutlineThickness(3);
            window->draw(piece);

            sf::CircleShape highlight(PIECE_R - 4);
            highlight.setOrigin(PIECE_R - 4, PIECE_R - 4);
            highlight.setPosition(pos.x - 3, pos.y - 3);
            highlight.setFillColor(sf::Color(255, 255, 255, 40));
            window->draw(highlight);

            if (r == selectedR && c == selectedC) {
                sf::CircleShape hl(PIECE_R + 5);
                hl.setOrigin(PIECE_R + 5, PIECE_R + 5);
                hl.setPosition(pos);
                hl.setFillColor(sf::Color::Transparent);
                hl.setOutlineColor(sf::Color(255, 220, 0, 200));
                hl.setOutlineThickness(4);
                window->draw(hl);
            }

            if (fontLoaded) {
                std::wstring text = GetPieceName(b[r][c].type, b[r][c].side);
                sf::Text t;
                t.setFont(*font);
                t.setString(text);
                t.setCharacterSize(20);
                t.setFillColor(sf::Color::White);
                sf::FloatRect bounds = t.getLocalBounds();
                t.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
                t.setPosition(pos);
                window->draw(t);
            }
        }
    }

    for (const auto& move : validMoves) {
        sf::Vector2f pos = board.GridToScreen(move.x, move.y);
        if (b[move.x][move.y].alive) {
            sf::CircleShape ring(PIECE_R + 3);
            ring.setOrigin(PIECE_R + 3, PIECE_R + 3);
            ring.setPosition(pos);
            ring.setFillColor(sf::Color::Transparent);
            ring.setOutlineColor(sf::Color(255, 60, 60, 220));
            ring.setOutlineThickness(4);
            window->draw(ring);
        } else {
            sf::CircleShape dot(10);
            dot.setOrigin(10, 10);
            dot.setPosition(pos);
            dot.setFillColor(sf::Color(80, 220, 80, 180));
            window->draw(dot);
        }
    }
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

void UIManager::DrawButton(const UIButton& btn) {
    sf::RectangleShape shadow(sf::Vector2f(btn.bounds.width, btn.bounds.height));
    shadow.setPosition(btn.bounds.left + 2, btn.bounds.top + 2);
    shadow.setFillColor(sf::Color(20, 15, 10, 100));
    window->draw(shadow);

    sf::RectangleShape rect(sf::Vector2f(btn.bounds.width, btn.bounds.height));
    rect.setPosition(btn.bounds.left, btn.bounds.top);
    sf::Color fillColor, outlineColor;
    if (btn.disabled) {
        fillColor = sf::Color(70, 60, 48);
        outlineColor = sf::Color(90, 78, 62);
    } else if (btn.hovered) {
        fillColor = sf::Color(110, 92, 72);
        outlineColor = sf::Color(160, 140, 110);
    } else {
        fillColor = sf::Color(80, 66, 52);
        outlineColor = sf::Color(130, 112, 88);
    }
    rect.setFillColor(fillColor);
    rect.setOutlineColor(outlineColor);
    rect.setOutlineThickness(3);
    window->draw(rect);

    if (fontLoaded) {
        sf::Text t;
        t.setFont(*font);
        t.setString(btn.label);
        t.setCharacterSize(18);
        sf::Color textColor = btn.disabled ? sf::Color(110, 100, 88) : sf::Color(245, 235, 220);
        t.setFillColor(textColor);
        sf::FloatRect bounds = t.getLocalBounds();
        t.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
        t.setPosition(btn.bounds.left + btn.bounds.width / 2.f,
                     btn.bounds.top + btn.bounds.height / 2.f);
        window->draw(t);
    }
}

void UIManager::DrawSmallButton(const UIButton& btn, sf::Color fill, sf::Color outline) {
    sf::RectangleShape rect(sf::Vector2f(btn.bounds.width, btn.bounds.height));
    rect.setPosition(btn.bounds.left, btn.bounds.top);
    rect.setFillColor(btn.hovered ? sf::Color(fill.r + 40, fill.g + 30, fill.b + 20) : fill);
    rect.setOutlineColor(outline);
    rect.setOutlineThickness(2);
    window->draw(rect);
    sf::Text t;
    t.setFont(*font);
    t.setString(btn.label);
    t.setCharacterSize(14);
    t.setFillColor(sf::Color(255, 255, 255));
    sf::FloatRect bounds = t.getLocalBounds();
    t.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
    t.setPosition(btn.bounds.left + btn.bounds.width / 2.f,
                 btn.bounds.top + btn.bounds.height / 2.f);
    window->draw(t);
}

void UIManager::DrawNetButton(const UIButton& btn) {
    sf::RectangleShape shadow(sf::Vector2f(btn.bounds.width, btn.bounds.height));
    shadow.setPosition(btn.bounds.left + 2, btn.bounds.top + 2);
    shadow.setFillColor(sf::Color(20, 15, 10, 100));
    window->draw(shadow);
    sf::RectangleShape rect(sf::Vector2f(btn.bounds.width, btn.bounds.height));
    rect.setPosition(btn.bounds.left, btn.bounds.top);
    rect.setFillColor(btn.hovered ? sf::Color(80, 120, 80) : sf::Color(60, 90, 60));
    rect.setOutlineColor(sf::Color(100, 150, 100));
    rect.setOutlineThickness(2);
    window->draw(rect);
    sf::Text t;
    t.setFont(*font);
    t.setString(btn.label);
    t.setCharacterSize(18);
    t.setFillColor(sf::Color(230, 255, 230));
    sf::FloatRect bounds = t.getLocalBounds();
    t.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
    t.setPosition(btn.bounds.left + btn.bounds.width / 2.f,
                 btn.bounds.top + btn.bounds.height / 2.f);
    window->draw(t);
}

void UIManager::DrawNetRedButton(const UIButton& btn) {
    sf::RectangleShape rect(sf::Vector2f(btn.bounds.width, btn.bounds.height));
    rect.setPosition(btn.bounds.left, btn.bounds.top);
    rect.setFillColor(btn.hovered ? sf::Color(150, 60, 60) : sf::Color(120, 40, 40));
    rect.setOutlineColor(sf::Color(180, 80, 80));
    rect.setOutlineThickness(2);
    window->draw(rect);
    sf::Text t;
    t.setFont(*font);
    t.setString(btn.label);
    t.setCharacterSize(18);
    t.setFillColor(sf::Color(255, 220, 220));
    sf::FloatRect bounds = t.getLocalBounds();
    t.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
    t.setPosition(btn.bounds.left + btn.bounds.width / 2.f,
                 btn.bounds.top + btn.bounds.height / 2.f);
    window->draw(t);
}

void UIManager::DrawButtons() {
    DrawButton(undoBtn);
    DrawButton(restartBtn);
    DrawButton(aiBtn);
    DrawButton(difficultyBtn);
    DrawButton(onlineBtn);
    DrawButton(tutorialBtn);
    DrawButton(surrenderBtn);
    DrawButton(drawOfferBtn);
}

void UIManager::DrawRequestPopups(bool undoRecv, bool restartRecv, bool surrRecv, bool drawRecv) {
    if (undoRecv) {
        sf::RectangleShape bg(sf::Vector2f(200, 70));
        bg.setPosition(940, 240);
        bg.setFillColor(sf::Color(180, 130, 30, 220));
        bg.setOutlineColor(sf::Color(255, 180, 50));
        bg.setOutlineThickness(2);
        window->draw(bg);
        DrawText(L"\u5bf9\u65b9\u8bf7\u6c42\u6094\u68cb", 1040, 260, 16, sf::Color(255, 255, 200), true);
        undoAcceptBtn.bounds = sf::FloatRect(940, 275, 95, 44);
        undoRejectBtn.bounds = sf::FloatRect(1045, 275, 95, 44);
        DrawSmallButton(undoAcceptBtn, sf::Color(60, 140, 60), sf::Color(80, 180, 80));
        DrawSmallButton(undoRejectBtn, sf::Color(160, 50, 50), sf::Color(200, 80, 80));
    }

    if (restartRecv) {
        sf::RectangleShape bg(sf::Vector2f(200, 70));
        bg.setPosition(940, 290);
        bg.setFillColor(sf::Color(180, 130, 30, 220));
        bg.setOutlineColor(sf::Color(255, 180, 50));
        bg.setOutlineThickness(2);
        window->draw(bg);
        DrawText(L"\u5bf9\u65b9\u8bf7\u6c42\u91cd\u5f00", 1040, 310, 16, sf::Color(255, 255, 200), true);
        restartAcceptBtn.bounds = sf::FloatRect(940, 325, 95, 44);
        restartRejectBtn.bounds = sf::FloatRect(1045, 325, 95, 44);
        DrawSmallButton(restartAcceptBtn, sf::Color(60, 140, 60), sf::Color(80, 180, 80));
        DrawSmallButton(restartRejectBtn, sf::Color(160, 50, 50), sf::Color(200, 80, 80));
    }

    if (surrRecv) {
        sf::RectangleShape bg(sf::Vector2f(200, 70));
        bg.setPosition(940, 340);
        bg.setFillColor(sf::Color(180, 40, 40, 220));
        bg.setOutlineColor(sf::Color(255, 80, 80));
        bg.setOutlineThickness(2);
        window->draw(bg);
        DrawText(L"\u5bf9\u65b9\u8ba4\u8f93\u4e86", 1040, 360, 16, sf::Color(255, 220, 200), true);
        surrenderAcceptBtn.bounds = sf::FloatRect(940, 375, 95, 44);
        surrenderRejectBtn.bounds = sf::FloatRect(1045, 375, 95, 44);
        DrawSmallButton(surrenderAcceptBtn, sf::Color(60, 140, 60), sf::Color(80, 180, 80));
        DrawSmallButton(surrenderRejectBtn, sf::Color(160, 50, 50), sf::Color(200, 80, 80));
    }

    if (drawRecv) {
        sf::RectangleShape bg(sf::Vector2f(200, 70));
        bg.setPosition(940, 390);
        bg.setFillColor(sf::Color(180, 180, 40, 220));
        bg.setOutlineColor(sf::Color(255, 255, 80));
        bg.setOutlineThickness(2);
        window->draw(bg);
        DrawText(L"\u5bf9\u65b9\u8bf7\u6c42\u548c\u68cb", 1040, 410, 16, sf::Color(255, 255, 200), true);
        drawAcceptBtn.bounds = sf::FloatRect(940, 425, 95, 44);
        drawRejectBtn.bounds = sf::FloatRect(1045, 425, 95, 44);
        DrawSmallButton(drawAcceptBtn, sf::Color(60, 140, 60), sf::Color(80, 180, 80));
        DrawSmallButton(drawRejectBtn, sf::Color(160, 50, 50), sf::Color(200, 80, 80));
    }
}

void UIManager::DrawUI(Side currentTurn, bool gameOver, bool isDrawGame, bool surrendered, Side winner,
                        bool inCheck, bool aiThinking) {
    if (!fontLoaded) return;

    sf::RectangleShape titleBg(sf::Vector2f(420, 50));
    titleBg.setPosition(715, 20);
    titleBg.setFillColor(sf::Color(65, 50, 35));
    titleBg.setOutlineColor(sf::Color(90, 75, 55));
    titleBg.setOutlineThickness(1);
    window->draw(titleBg);
    DrawTextWithShadow(L"\u5bf9\u89d2\u8c61\u68cb", 925, 45, 26, sf::Color(220, 200, 170), true);

    sf::RectangleShape turnBg(sf::Vector2f(420, 70));
    turnBg.setPosition(715, 80);
    turnBg.setFillColor(sf::Color(60, 48, 34));
    turnBg.setOutlineColor(sf::Color(85, 70, 50));
    turnBg.setOutlineThickness(1);
    window->draw(turnBg);

    std::wstring turnText;
    sf::Color turnColor, indicatorColor;

    if (gameOver) {
        if (isDrawGame) {
            turnText = L"\u548c\u68cb\uff01";
            turnColor = sf::Color(220, 220, 120);
            indicatorColor = sf::Color(200, 200, 100);
        } else {
            if (surrendered) {
                turnText = L"\u8ba4\u8f93";
                turnColor = sf::Color(255, 150, 100);
                indicatorColor = sf::Color(255, 150, 100);
            } else if (inCheck) {
                turnText = L"\u7edd\u6740\uff01";
                turnColor = sf::Color(255, 215, 0);

                indicatorColor = sf::Color(255, 215, 0);
            } else {
                turnText = L"\u56f0\u6bd9\uff01";
                turnColor = sf::Color(255, 180, 50);
                indicatorColor = sf::Color(255, 180, 50);
            }
        }
    } else {
        turnText = (currentTurn == Side::RED) ? L"\u7ea2\u65b9\u8d70\u68cb" : L"\u9ed1\u65b9\u8d70\u68cb";
        turnColor = (currentTurn == Side::RED) ? sf::Color(255, 120, 120) : sf::Color(220, 220, 220);
        indicatorColor = (currentTurn == Side::RED) ? sf::Color(220, 60, 60) : sf::Color(80, 80, 80);
    }

    sf::CircleShape indicator(12);
    indicator.setOrigin(12, 12);
    indicator.setPosition(745, 115);
    indicator.setFillColor(indicatorColor);
    indicator.setOutlineColor(sf::Color(255, 255, 255, 80));
    indicator.setOutlineThickness(2);
    window->draw(indicator);
    DrawTextWithShadow(turnText, 935, 115, 30, turnColor, true);

    if (!gameOver && inCheck) {
        sf::RectangleShape checkBg(sf::Vector2f(140, 40));
        checkBg.setPosition(855, 160);
        checkBg.setFillColor(sf::Color(180, 40, 40, 220));
        checkBg.setOutlineColor(sf::Color(255, 120, 120));
        checkBg.setOutlineThickness(2);
        window->draw(checkBg);
        DrawTextWithShadow(L"\u5c06\u519b\uff01", 925, 180, 24, sf::Color(255, 255, 150), true);
    }

    if (aiThinking) {
        DrawText(L"AI\u601d\u8003\u4e2d...", 925, 210, 18, sf::Color(200, 180, 150), true);
    }
}

void UIManager::DrawNotification() {
    if (!notificationText.empty() && notificationTimer > 0.f) {
        float alpha = std::min(notificationTimer / 3.f, 1.f) * 255.f;
        sf::Color notifColor(255, 100, 60, static_cast<sf::Uint8>(alpha));
        sf::RectangleShape notifBg(sf::Vector2f(300, 30));
        notifBg.setPosition(820, 170);
        notifBg.setFillColor(sf::Color(180, 60, 30, static_cast<sf::Uint8>(alpha * 0.6f)));
        notifBg.setOutlineColor(sf::Color(255, 100, 50, static_cast<sf::Uint8>(alpha * 0.8f)));
        notifBg.setOutlineThickness(2);
        window->draw(notifBg);
        DrawTextWithShadow(notificationText, 970, 185, 16, notifColor, true);
    }
}

void UIManager::DrawMoveLog(const std::vector<std::wstring>& moveLogStrings) {
    if (!fontLoaded) return;

    float logY = logDividerY;
    float logH = 810.f - logDividerY;

    sf::RectangleShape logBg(sf::Vector2f(420, logH));
    logBg.setPosition(715, logY);
    logBg.setFillColor(sf::Color(55, 44, 32));
    logBg.setOutlineColor(sf::Color(85, 70, 52));
    logBg.setOutlineThickness(1);
    window->draw(logBg);

    sf::RectangleShape logHeader(sf::Vector2f(420, 30));
    logHeader.setPosition(715, logY);
    logHeader.setFillColor(sf::Color(70, 56, 40));
    window->draw(logHeader);
    DrawTextWithShadow(L"\u8d70\u68cb\u8bb0\u5f55", 925, logY + 15, 16, sf::Color(230, 210, 180), true);

    int totalLines = (int)moveLogStrings.size();
  int visibleLines = (int)((logH - 40) / 22);
    int maxScroll = std::max(0, totalLines - visibleLines);
    if (logScrollOffset > maxScroll) logScrollOffset = maxScroll;
    if (logScrollOffset < 0) logScrollOffset = 0;

    int startIdx = std::max(0, totalLines - visibleLines - logScrollOffset);
    float y = logY + 40;
    for (int i = startIdx; i < totalLines; i++) {
        if (i % 2 == 0) {
            sf::RectangleShape rowBg(sf::Vector2f(410, 20));
            rowBg.setPosition(720, y - 2);
            rowBg.setFillColor(sf::Color(65, 52, 38, 100));
            window->draw(rowBg);
        }
        std::wostringstream oss;
        oss << (i + 1) << L". " << moveLogStrings[i];
        sf::Color c = (i % 2 == 0) ? sf::Color(255, 140, 140) : sf::Color(220, 220, 220);
        DrawText(oss.str(), 730, y, 13, c);
        y += 22;
    }
}

void UIManager::DrawGameOverEffect(float gameOverTimer, bool isDrawGame, Side winner,
                                    bool surrendered, bool checkmate, int movesWithoutCapture,
                                    bool agreedDraw, bool hasInsufficientMaterial) {
    float alpha = std::min(gameOverTimer * 200.f, 180.f);
    sf::RectangleShape overlay(sf::Vector2f(1150, 820));
    overlay.setPosition(0, 0);
    overlay.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(alpha)));
    window->draw(overlay);

    if (gameOverTimer > 0.5f) {
        float textAlpha = std::min((gameOverTimer - 0.5f) * 400.f, 255.f);
        std::wstring mainText, subText;
        sf::Color mainColor;

        if (isDrawGame) {
            mainText = L"\u548c\u68cb";
            mainColor = sf::Color(255, 255, 100, static_cast<sf::Uint8>(textAlpha));
            if (movesWithoutCapture >= DRAW_LIMIT) {
                subText = L"\u81ea\u7136\u9650\u7740\u548c\u68cb";
            } else if (hasInsufficientMaterial) {
                subText = L"\u5b50\u529b\u4e0d\u8db3\u548c\u68cb";
            } else if (agreedDraw) {
                subText = L"\u53cc\u65b9\u540c\u610f\u548c\u68cb";
            } else {
                subText = L"\u56f0\u6bd9\u548c\u68cb";
            }
        } else {
            Side loser = (winner == Side::RED) ? Side::BLACK : Side::RED;
            if (surrendered) {
                mainText = (winner == Side::RED) ? L"\u7ea2\u65b9\u80dc\u5229" : L"\u9ed1\u65b9\u80dc\u5229";
            mainColor = (winner == Side::RED) ?
                    sf::Color(255, 80, 80, static_cast<sf::Uint8>(textAlpha)) :
                    sf::Color(220, 220, 220, static_cast<sf::Uint8>(textAlpha));
                subText = (loser == Side::RED) ? L"\u7ea2\u65b9\u8ba4\u8f93" : L"\u9ed1\u65b9\u8ba4\u8f93";
            } else if (checkmate) {
                mainText = L"\u7edd\u6740";
                mainColor = sf::Color(255, 215, 0, static_cast<sf::Uint8>(textAlpha));
                subText = (winner == Side::RED) ? L"\u7ea2\u65b9\u7edd\u6740\u9ed1\u65b9" : L"\u9ed1\u65b9\u7edd\u6740\u7ea2\u65b9";
            } else {
                mainText = L"\u56f0\u6bd9";
                mainColor = sf::Color(255, 180, 50, static_cast<sf::Uint8>(textAlpha));
                subText = (loser == Side::RED) ? L"\u7ea2\u65b9\u88ab\u56f0\u6bd9" : L"\u9ed1\u65b9\u88ab\u56f0\u6bd9";
            }
        }

        DrawTextWithShadow(mainText, 575, 360, 72, mainColor, true);
        sf::Color subColor(200, 200, 200, static_cast<sf::Uint8>(textAlpha));
        DrawTextWithShadow(subText, 575, 430, 28, subColor, true);

        float btnX = 475;
        float btnY = 500;
        gameOverRestartBtn.bounds.left = btnX;
        gameOverRestartBtn.bounds.top = btnY;

        sf::RectangleShape btnShadow(sf::Vector2f(200, 50));
        btnShadow.setPosition(btnX + 3, btnY + 3);
        btnShadow.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(textAlpha * 0.5f)));
        window->draw(btnShadow);

        sf::RectangleShape btnRect(sf::Vector2f(200, 50));
        btnRect.setPosition(btnX, btnY);
        sf::Color btnColor = gameOverRestartBtn.hovered ?
            sf::Color(140, 110, 80, static_cast<sf::Uint8>(textAlpha)) :
            sf::Color(100, 80, 60, static_cast<sf::Uint8>(textAlpha));
        btnRect.setFillColor(btnColor);
        btnRect.setOutlineColor(sf::Color(180, 150, 110, static_cast<sf::Uint8>(textAlpha)));
        btnRect.setOutlineThickness(2);
        window->draw(btnRect);

        sf::Color btnTextColor(255, 255, 255, static_cast<sf::Uint8>(textAlpha));
        DrawTextWithShadow(L"\u91cd\u65b0\u5f00\u59cb", btnX + 100, btnY + 25, 22, btnTextColor, true);
    }
}

void UIManager::DrawNetUI(bool netMode, NetState netState, const std::wstring& localIP,
                           const std::wstring& inputIP, bool showIPInput, Side netSide) {
    if (!fontLoaded) return;
    if (!netMode) return;

    if (netState == NetState::OFFLINE) {
        if (showIPInput) {
            sf::RectangleShape inputBg(sf::Vector2f(200, 35));
            inputBg.setPosition(730, 440);
            inputBg.setFillColor(sf::Color(40, 35, 28));
            inputBg.setOutlineColor(sf::Color(120, 100, 80));
            inputBg.setOutlineThickness(2);
            window->draw(inputBg);
            std::wstring displayIP = inputIP.empty() ? L"\u8f93\u5165IP\u5730\u5740..." : inputIP;
            sf::Color ipColor = inputIP.empty() ? sf::Color(120, 110, 100) : sf::Color(240, 230, 220);
            DrawText(displayIP, 740, 447, 15, ipColor);
            DrawNetButton(connectBtn);
        } else {
            DrawNetButton(hostBtn);
            DrawNetButton(joinBtn);
        }
    } else if (netState == NetState::HOST_WAITING) {
        sf::RectangleShape statusBg(sf::Vector2f(200, 90));
        statusBg.setPosition(730, 500);
        statusBg.setFillColor(sf::Color(50, 45, 35));
        statusBg.setOutlineColor(sf::Color(80, 70, 55));
        statusBg.setOutlineThickness(1);
        window->draw(statusBg);
        DrawText(L"\u7b49\u5f85\u8fde\u63a5...", 830, 520, 16, sf::Color(200, 200, 150), true);
        DrawText(L"\u672c\u673aIP:", 830, 546, 14, sf::Color(180, 180, 180), true);
        DrawText(localIP, 830, 568, 16, sf::Color(100, 255, 100), true);
        disconnectBtn.bounds.top = 600;
        DrawNetRedButton(disconnectBtn);
    } else if (netState == NetState::CONNECTED) {
        sf::RectangleShape statusBg(sf::Vector2f(200, 80));
        statusBg.setPosition(730, 500);
        statusBg.setFillColor(sf::Color(40, 60, 40));
        statusBg.setOutlineColor(sf::Color(80, 120, 80));
        statusBg.setOutlineThickness(1);
        window->draw(statusBg);
        DrawText(L"\u5df2\u8fde\u63a5", 830, 520, 18, sf::Color(100, 255, 100), true);
        std::wstring sideText = (netSide == Side::RED) ? L"\u4f60\u662f\u7ea2\u65b9" : L"\u4f60\u662f\u9ed1\u65b9";
        sf::Color sideColor = (netSide == Side::RED) ? sf::Color(255, 120, 120) : sf::Color(200, 200, 200);
        DrawText(sideText, 830, 555, 16, sideColor, true);
        disconnectBtn.bounds.top = 590;
        DrawNetRedButton(disconnectBtn);
    }
}

void UIManager::DrawTutorialPanel() {
    if (!fontLoaded) return;
    sf::RectangleShape bg(sf::Vector2f(440, 820));
    bg.setPosition(710, 0);
    bg.setFillColor(sf::Color(38, 28, 18, 250));
    bg.setOutlineColor(sf::Color(120, 100, 70));
    bg.setOutlineThickness(2);
    window->draw(bg);

    sf::RectangleShape header(sf::Vector2f(440, 36));
    header.setPosition(710, 0);
    header.setFillColor(sf::Color(55, 42, 30));
    window->draw(header);
    DrawTextWithShadow(L"\u65b0\u624b\u6559\u7a0b", 930, 18, 20, sf::Color(255, 220, 150), true);

    sf::RectangleShape closeBtn(sf::Vector2f(50, 28));
    closeBtn.setPosition(1090, 4);
    closeBtn.setFillColor(sf::Color(180, 50, 50));
    closeBtn.setOutlineColor(sf::Color(220, 80, 80));
    closeBtn.setOutlineThickness(2);
    window->draw(closeBtn);
    DrawTextWithShadow(L"\u2716", 1115, 18, 18, sf::Color::White, true);

    const wchar_t* lines[] = {
        L"\u3010\u68cb\u76d8\u3011 9x9\u4ea4\u70b9\uff0c\u68cb\u76d8\u659c\u653e\uff0c",
        L"\u53cc\u65b9\u5404\u6709\u4e00\u4e2a\u4e5d\u5bab\uff083x3\uff09\u3002",
        L"",
        L"\u3010\u8d70\u6cd5\u3011",
        L"\u5c06/\u5e05\uff1a\u6a2a\u7ad6\u4e00\u6b65\uff0c\u4e0d\u51fa\u4e5d\u5bab\u3002",
        L"\u58eb/\u4ed5\uff1a\u516b\u65b9\u5411\u4e00\u6b65\uff0c\u4e0d\u51fa\u4e5d\u5bab\u3002",
        L"\u8c61/\u76f8\uff1a\u8d70\u7530\u5b57\u00b12\u00b12\uff0c\u53ef\u8fc7\u6cb3\u3002",
        L"\u9a6c\uff1a\u8d70\u65e5\u5b57\uff0c\u8e6b\u9a6c\u817f\u3002",
        L"\u8f66\uff1a\u6a2a\u7ad6\u65e0\u9650\uff0c\u4e0d\u8d8a\u5b50\u3002",
        L"\u70ae\uff1a\u79fb\u52a8\u5982\u8f66\uff0c\u5403\u5b50\u5fc5\u987b\u9694\u4e00\u5b50\u3002",
        L"\u5175/\u5352\uff1a\u659c\u524d\u4e00\u6b65\uff0c\u4e0d\u540e\u9000\u3002",
        L"",
        L"\u3010\u80dc\u8d1f\u3011\u5403\u6389\u5bf9\u65b9\u5c06/\u5e05\u5373\u80dc\u3002",
        L"\u3010\u548c\u68cb\u3011120\u6b65\u65e0\u5403\u5b50\u6216\u5b50\u529b\u4e0d\u8db3\u3002",
        L"",
        L"\u3010\u8054\u673a\u3011\u540cWiFi\u4e0b\uff0c\u4e00\u4eba\u521b\u5efa\u623f\u95f4\uff0c",
        L"\u53e6\u4e00\u4eba\u8f93\u5165IP\u52a0\u5165\u3002\u4e3b\u673a\u6267\u7ea2\u5148\u624b\u3002",
        L"\u3010AI\u3011\u4e09\u6863\u96be\u5ea6\uff0c\u8054\u673a\u65f6\u81ea\u52a8\u5173\u95ed\u3002",
        L"\u3010\u614c\u68cb\u3011\u8054\u673a\u65f6\u9700\u53cc\u65b9\u540c\u610f\u3002",
    };
    float y = 50;
    for (const auto& line : lines) {
        bool isHeader = (wcslen(line) > 0 && line[0] == L'\u3010');
        DrawText(line, 730, y, isHeader ? 16 : 14,
                 isHeader ? sf::Color(255, 200, 100) : sf::Color(210, 200, 180));
        y += isHeader ? 26 : 21;
    }
}

void UIManager::DrawSurrenderPopup() {
    sf::RectangleShape overlay(sf::Vector2f(1150, 820));
    overlay.setPosition(0, 0);
    overlay.setFillColor(sf::Color(0, 0, 0, 160));
    window->draw(overlay);

    float panelW = 340, panelH = 200;
    float panelX = (1150 - panelW) / 2, panelY = 310;

    sf::RectangleShape panel(sf::Vector2f(panelW, panelH));
    panel.setPosition(panelX, panelY);
    panel.setFillColor(sf::Color(50, 40, 30));
    panel.setOutlineColor(sf::Color(120, 100, 70));
    panel.setOutlineThickness(3);
    window->draw(panel);

    float centerX = panelX + panelW / 2;
    DrawTextWithShadow(L"\u8ba4\u8f93", centerX, panelY + 35, 26, sf::Color(255, 220, 150), true);
    DrawText(L"\u8bf7\u9009\u62e9\u8ba4\u8f93\u65b9", centerX, panelY + 70, 14, sf::Color(200, 190, 170), true);

    float btnW = 130, btnH = 44, btnY = panelY + 95, gap = 20;
    float btnLeftX = centerX - btnW - gap / 2;
    surrenderRedBtn.bounds = sf::FloatRect(btnLeftX, btnY, btnW, btnH);
    surrenderBlackBtn.bounds = sf::FloatRect(centerX + gap / 2, btnY, btnW, btnH);
    DrawSmallButton(surrenderRedBtn, sf::Color(180, 40, 40), sf::Color(240, 80, 80));
    DrawSmallButton(surrenderBlackBtn, sf::Color(40, 40, 40), sf::Color(100, 100, 100));

    DrawText(L"\u70b9\u51fb\u5916\u90e8\u53d6\u6d88", centerX, panelY + panelH + 18, 13, sf::Color(150, 140, 130), true);
}
