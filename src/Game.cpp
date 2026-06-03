#include "Game.h"
#include <cmath>
#include <sstream>
#include <iostream>
#include <random>

Game::Game()
    : window(sf::VideoMode(Game::WIN_W, Game::WIN_H), L"\u5bf9\u89d2\u8c61\u68cb",
             sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize)
    , currentTurn(Side::RED)
    , selectedR(-1), selectedC(-1)
    , pieceSelected(false)
    , gameOver(false)
    , winner(Side::RED)
    , isDrawGame(false)
    , surrendered(false)
    , agreedDraw(false)
    , gameOverTimer(0.f)
    , movesWithoutCapture(0)
    , fontLoaded(false)
    , aiMode(false)
    , aiSide(Side::BLACK)
    , aiDepth(3)
    , aiDifficulty(AIDifficulty::MEDIUM)
    , aiThinking(false)
    , aiDelayTimer(0.f)
    , soundsLoaded(false)
    , showTutorial(false)
    , logScrollOffset(0)
    , logDividerY(440.f)
    , draggingLogDivider(false)
    , netState(NetState::OFFLINE)
    , showIPInput(false)
    , netMode(false)
    , netSide(Side::RED)
    , receivingMove(false)
    , showSurrenderPopup(false)
    , undoRequestSent(false)
    , undoRequestReceived(false)
    , restartRequestSent(false)
    , restartRequestReceived(false)
    , surrenderRequestSent(false)
    , surrenderRequestReceived(false)
    , drawRequestSent(false)
    , drawRequestReceived(false)
    , undoRequesterSide(Side::RED)
    , surrenderRequesterSide(Side::RED)
    , notificationTimer(0.f)
{
    window.setFramerateLimit(60);
    view = window.getDefaultView();

    if (font.loadFromFile("C:/Windows/Fonts/simhei.ttf") ||
        font.loadFromFile("C:/Windows/Fonts/msyh.ttc") ||
        font.loadFromFile("C:/Windows/Fonts/simsun.ttc")) {
        fontLoaded = true;
    }

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

    placePieces();
    initSounds();
}

Game::~Game() {
}

void Game::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f;
        processEvents();
        update(dt);
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();

        if (event.type == sf::Event::Resized) {
            float ratio = (float)event.size.height / event.size.width;
            float baseRatio = (float)Game::WIN_H / Game::WIN_W;
            if (ratio > baseRatio) {
                float nh = event.size.width * baseRatio;
                view.setViewport(sf::FloatRect(0, (1.f - nh / event.size.height) / 2.f, 1.f, nh / event.size.height));
            } else {
                float nw = event.size.height / baseRatio;
                view.setViewport(sf::FloatRect((1.f - nw / event.size.width) / 2.f, 0, nw / event.size.width, 1.f));
            }
        }

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f worldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y), view);
            float mx = worldPos.x;
            float my = worldPos.y;

            float ly = logDividerY;
            if (mx >= 715 && mx <= 1135 && my >= ly - 8 && my <= ly + 8) {
                draggingLogDivider = true;
            } else if (gameOver && gameOverTimer > 0.5f && gameOverRestartBtn.bounds.contains(mx, my)) {
                restartGame();
                playClickSound();
            } else if (showSurrenderPopup && !aiMode && netState != NetState::CONNECTED) {
                if (surrenderRedBtn.bounds.contains(mx, my)) {
                    doSurrender(Side::RED);
                    showSurrenderPopup = false;
                    playClickSound();
                } else if (surrenderBlackBtn.bounds.contains(mx, my)) {
                    doSurrender(Side::BLACK);
                    showSurrenderPopup = false;
                    playClickSound();
                } else if (mx < 405 || mx > 745 || my < 310 || my > 510) {
                    showSurrenderPopup = false;
                }
            } else if (mx >= 700) {
                handleButtonClick(mx, my);
            } else {
                sf::Vector2i gridPos = screenToGrid(mx, my);
                if (gridPos.x >= 0 && gridPos.x < 9 && gridPos.y >= 0 && gridPos.y < 9) {
                    handleBoardClick(gridPos.x, gridPos.y);
                }
            }
        }

        if (event.type == sf::Event::MouseWheelScrolled) {
            sf::Vector2f worldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseWheelScroll.x, event.mouseWheelScroll.y), view);
            int x = static_cast<int>(worldPos.x);
            if (x >= 715 && x <= 1135) {
                logScrollOffset -= (int)event.mouseWheelScroll.delta;
            }
        }

        if (event.type == sf::Event::MouseButtonReleased) {
            draggingLogDivider = false;
        }

        if (event.type == sf::Event::MouseMoved) {
            sf::Vector2f worldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y), view);
            float mx = worldPos.x;
            float my = worldPos.y;

            if (draggingLogDivider) {
                logDividerY = std::max(395.f, std::min(my, 740.f));
            }
            undoBtn.hovered = undoBtn.bounds.contains(mx, my);
            restartBtn.hovered = restartBtn.bounds.contains(mx, my);
            aiBtn.hovered = aiBtn.bounds.contains(mx, my);
            difficultyBtn.hovered = difficultyBtn.bounds.contains(mx, my);
            gameOverRestartBtn.hovered = gameOverRestartBtn.bounds.contains(mx, my);
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
        }

        if (event.type == sf::Event::TextEntered && showIPInput) {
            if (event.text.unicode == 8) {
                if (!inputIP.empty()) inputIP.pop_back();
            } else if (event.text.unicode < 128 && event.text.unicode != 13) {
                if (inputIP.size() < 15) {
                    inputIP += static_cast<wchar_t>(event.text.unicode);
                }
            } else if (event.text.unicode == 13) {
                if (!inputIP.empty()) {
                    startClient();
                }
            }
        }
    }
}

void Game::update(float dt) {
    updateParticles(dt);

    if (notificationTimer > 0.f) {
        notificationTimer -= dt;
        if (notificationTimer <= 0.f) {
            notificationTimer = 0.f;
            notificationText.clear();
        }
    }

    if (gameOver) {
        gameOverTimer += dt;
    }

    if (netState == NetState::HOST_WAITING) {
        sf::Socket::Status status = listener.accept(socket);
        if (status == sf::Socket::Done) {
            socket.setBlocking(false);
            netState = NetState::CONNECTED;
            netSide = Side::RED;
            netMode = true;
            aiMode = false;
            aiBtn.label = L"AI: \u5173\u95ed";
            undoRequestSent = false;
            undoRequestReceived = false;
            restartRequestSent = false;
            restartRequestReceived = false;
            surrenderRequestSent = false;
            surrenderRequestReceived = false;
        drawRequestSent = false;
        drawRequestReceived = false;
        receivingMove = false;
            restartGame();
        }
    }

    if (netState == NetState::CONNECTED && !gameOver) {
        pollNetwork();
    }

    if (undoRequestSent) {
        undoBtn.label = L"\u7b49\u5f85\u540c\u610f...";
        undoBtn.disabled = true;
    } else if (undoRequestReceived) {
        undoBtn.disabled = true;
    } else {
        undoBtn.label = L"\u6094\u68cb";
        undoBtn.disabled = false;
    }

    if (restartRequestSent) {
        restartBtn.label = L"\u7b49\u5f85\u540c\u610f...";
        restartBtn.disabled = true;
    } else if (restartRequestReceived) {
        restartBtn.disabled = true;
    } else {
        restartBtn.label = L"\u91cd\u65b0\u5f00\u59cb";
        restartBtn.disabled = false;
    }

    if (surrenderRequestSent) {
        surrenderBtn.label = L"\u7b49\u5f85\u540c\u610f...";
        surrenderBtn.disabled = true;
    } else if (surrenderRequestReceived) {
        surrenderBtn.disabled = true;
    } else if (gameOver || netState == NetState::HOST_WAITING) {
        surrenderBtn.label = L"\u8ba4\u8f93";
        surrenderBtn.disabled = true;
    } else {
        surrenderBtn.label = L"\u8ba4\u8f93";
        surrenderBtn.disabled = false;
    }

    if (drawRequestSent) {
        drawOfferBtn.label = L"\u7b49\u5f85\u540c\u610f...";
        drawOfferBtn.disabled = true;
    } else if (drawRequestReceived) {
        drawOfferBtn.disabled = true;
    } else if (gameOver || netState == NetState::HOST_WAITING) {
        drawOfferBtn.label = L"\u548c\u68cb";
        drawOfferBtn.disabled = true;
    } else {
        drawOfferBtn.label = L"\u548c\u68cb";
        drawOfferBtn.disabled = false;
    }

    if (aiMode && !netMode && currentTurn == aiSide && !gameOver) {
        aiDelayTimer += dt;
        aiThinking = true;
        
        if (aiDelayTimer >= 1.5f) {
            doAITurn();
            aiThinking = false;
            aiDelayTimer = 0.f;
        }
    } else {
        aiThinking = false;
        aiDelayTimer = 0.f;
    }
}

void Game::render() {
    window.setView(view);
    sf::Color bgColor(40, 32, 22);
    window.clear(bgColor);

    sf::RectangleShape leftPanel(sf::Vector2f(700, Game::WIN_H));
    leftPanel.setPosition(0, 0);
    leftPanel.setFillColor(sf::Color(55, 42, 28));
    window.draw(leftPanel);

    sf::RectangleShape rightPanel(sf::Vector2f(450, Game::WIN_H));
    rightPanel.setPosition(700, 0);
    rightPanel.setFillColor(sf::Color(45, 35, 24));
    window.draw(rightPanel);

    sf::RectangleShape divider(sf::Vector2f(2, Game::WIN_H));
    divider.setPosition(699, 0);
    divider.setFillColor(sf::Color(80, 65, 45));
    window.draw(divider);

    drawBoard();
    drawPieces();
    drawUI();
    drawButtons();
    float dy = logDividerY;
    sf::RectangleShape logDivider(sf::Vector2f(420, 6));
    logDivider.setPosition(715, dy - 3);
    logDivider.setFillColor(draggingLogDivider ? sf::Color(150, 120, 80) : sf::Color(90, 70, 45));
    window.draw(logDivider);
    if (showTutorial) drawTutorialPanel();
    drawNetUI();
    drawMoveLog();

    if (showSurrenderPopup) drawSurrenderPopup();

    if (gameOver) {
        drawGameOverEffect();
    }

    drawParticles();
    window.display();
}

void Game::initBoard() {
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            board[r][c] = {PieceType::NONE, Side::RED, false};
        }
    }
}

void Game::placePieces() {
    initBoard();

    board[0][8] = {PieceType::GENERAL, Side::RED, true};
    board[0][7] = {PieceType::ADVISOR, Side::RED, true};
    board[1][8] = {PieceType::ADVISOR, Side::RED, true};
    board[0][6] = {PieceType::ELEPHANT, Side::RED, true};
    board[2][8] = {PieceType::ELEPHANT, Side::RED, true};
    board[0][5] = {PieceType::HORSE, Side::RED, true};
    board[3][8] = {PieceType::HORSE, Side::RED, true};
    board[0][4] = {PieceType::CHARIOT, Side::RED, true};
    board[4][8] = {PieceType::CHARIOT, Side::RED, true};

    board[0][3] = {PieceType::CANNON, Side::RED, true};
    board[5][8] = {PieceType::CANNON, Side::RED, true};

    board[0][2] = {PieceType::SOLDIER, Side::RED, true};
    board[2][4] = {PieceType::SOLDIER, Side::RED, true};
    board[2][6] = {PieceType::SOLDIER, Side::RED, true};
    board[4][6] = {PieceType::SOLDIER, Side::RED, true};
    board[6][8] = {PieceType::SOLDIER, Side::RED, true};

    board[8][0] = {PieceType::GENERAL, Side::BLACK, true};
    board[8][1] = {PieceType::ADVISOR, Side::BLACK, true};
    board[7][0] = {PieceType::ADVISOR, Side::BLACK, true};
    board[8][2] = {PieceType::ELEPHANT, Side::BLACK, true};
    board[6][0] = {PieceType::ELEPHANT, Side::BLACK, true};
    board[8][3] = {PieceType::HORSE, Side::BLACK, true};
    board[5][0] = {PieceType::HORSE, Side::BLACK, true};
    board[8][4] = {PieceType::CHARIOT, Side::BLACK, true};
    board[4][0] = {PieceType::CHARIOT, Side::BLACK, true};

    board[8][5] = {PieceType::CANNON, Side::BLACK, true};
    board[3][0] = {PieceType::CANNON, Side::BLACK, true};

    board[8][6] = {PieceType::SOLDIER, Side::BLACK, true};
    board[6][4] = {PieceType::SOLDIER, Side::BLACK, true};
    board[6][2] = {PieceType::SOLDIER, Side::BLACK, true};
    board[4][2] = {PieceType::SOLDIER, Side::BLACK, true};
    board[2][0] = {PieceType::SOLDIER, Side::BLACK, true};

    currentTurn = Side::RED;
    pieceSelected = false;
    gameOver = false;
    isDrawGame = false;
    surrendered = false;
    agreedDraw = false;
    gameOverTimer = 0.f;
    movesWithoutCapture = 0;
    notificationTimer = 0.f;
    notificationText.clear();
    while (!moveHistory.empty()) moveHistory.pop();
    moveLogStrings.clear();
    particles.clear();
}

sf::Vector2f Game::gridToScreen(int r, int c) const {
    float x = Game::ORIGIN_X + (r + c) * Game::DIAG;
    float y = Game::ORIGIN_Y + (c - r) * Game::DIAG;
    return sf::Vector2f(x, y);
}

sf::Vector2i Game::screenToGrid(float sx, float sy) const {
    float dx = sx - Game::ORIGIN_X;
    float dy = sy - Game::ORIGIN_Y;
    float rf = (dx / Game::DIAG - dy / Game::DIAG) / 2.f;
    float cf = (dx / Game::DIAG + dy / Game::DIAG) / 2.f;
    int r = static_cast<int>(std::round(rf));
    int c = static_cast<int>(std::round(cf));
    return sf::Vector2i(r, c);
}

bool Game::isInPalace(int r, int c, Side side) const {
    if (side == Side::RED) {
        return r >= 0 && r <= 2 && c >= 6 && c <= 8;
    } else {
        return r >= 6 && r <= 8 && c >= 0 && c <= 2;
    }
}

std::vector<sf::Vector2i> Game::getValidMoves(int r, int c) const {
    std::vector<sf::Vector2i> moves;
    if (r < 0 || r >= 9 || c < 0 || c >= 9) return moves;
    if (!board[r][c].alive) return moves;

    const Piece& piece = board[r][c];
    for (int tr = 0; tr < 9; tr++) {
        for (int tc = 0; tc < 9; tc++) {
            if (tr == r && tc == c) continue;
            if (board[tr][tc].alive && board[tr][tc].side == piece.side) continue;
            if (isValidMove(r, c, tr, tc)) {
                if (!wouldBeInCheck(r, c, tr, tc, piece.side)) {
                    moves.push_back(sf::Vector2i(tr, tc));
                }
            }
        }
    }
    return moves;
}

bool Game::isValidMove(int fromR, int fromC, int toR, int toC) const {
    return isValidMoveB(board, fromR, fromC, toR, toC);
}

bool Game::isValidMoveB(const Piece b[9][9], int fromR, int fromC, int toR, int toC) const {
    if (fromR < 0 || fromR >= 9 || fromC < 0 || fromC >= 9) return false;
    if (toR < 0 || toR >= 9 || toC < 0 || toC >= 9) return false;

    const Piece& piece = b[fromR][fromC];
    if (!piece.alive) return false;
    if (b[toR][toC].alive && b[toR][toC].side == piece.side) return false;

    switch (piece.type) {
        case PieceType::CHARIOT: return canChariotMove(b, fromR, fromC, toR, toC);
        case PieceType::HORSE: return canHorseMove(b, fromR, fromC, toR, toC);
        case PieceType::ELEPHANT: return canElephantMove(b, fromR, fromC, toR, toC);
        case PieceType::ADVISOR: return canAdvisorMove(b, fromR, fromC, toR, toC);
        case PieceType::GENERAL: return canGeneralMove(b, fromR, fromC, toR, toC);
        case PieceType::CANNON: return canCannonMove(b, fromR, fromC, toR, toC);
        case PieceType::SOLDIER: return canSoldierMove(b, fromR, fromC, toR, toC, piece.side);
        default: return false;
    }
}

bool Game::canChariotMove(const Piece b[9][9], int fr, int fc, int tr, int tc) const {
    if (fr != tr && fc != tc) return false;
    return countPiecesBetween(b, fr, fc, tr, tc) == 0;
}

bool Game::canHorseMove(const Piece b[9][9], int fr, int fc, int tr, int tc) const {
    int dr = std::abs(tr - fr);
    int dc = std::abs(tc - fc);
    if (!((dr == 2 && dc == 1) || (dr == 1 && dc == 2))) return false;
    return !isBlockedHorse(b, fr, fc, tr, tc);
}

bool Game::isBlockedHorse(const Piece b[9][9], int fr, int fc, int tr, int tc) const {
    int dr = tr - fr;
    int dc = tc - fc;
    if (std::abs(dr) == 2) {
        int blockR = fr + (dr > 0 ? 1 : -1);
        if (b[blockR][fc].alive) return true;
    } else {
        int blockC = fc + (dc > 0 ? 1 : -1);
        if (b[fr][blockC].alive) return true;
    }
    return false;
}

bool Game::canElephantMove(const Piece b[9][9], int fr, int fc, int tr, int tc) const {
    int dr = std::abs(tr - fr);
    int dc = std::abs(tc - fc);
    if (dr != 2 || dc != 2) return false;
    return !isBlockedElephant(b, fr, fc, tr, tc);
}

bool Game::isBlockedElephant(const Piece b[9][9], int fr, int fc, int tr, int tc) const {
    int midR = (fr + tr) / 2;
    int midC = (fc + tc) / 2;
    return b[midR][midC].alive;
}

bool Game::canAdvisorMove(const Piece b[9][9], int fr, int fc, int tr, int tc) const {
    if (!isInPalace(tr, tc, b[fr][fc].side)) return false;
    int dr = std::abs(tr - fr);
    int dc = std::abs(tc - fc);
    return (dr <= 1 && dc <= 1 && (dr + dc > 0));
}

bool Game::canGeneralMove(const Piece b[9][9], int fr, int fc, int tr, int tc) const {
    if (!isInPalace(tr, tc, b[fr][fc].side)) return false;
    int dr = std::abs(tr - fr);
    int dc = std::abs(tc - fc);
    return (dr + dc == 1);
}

bool Game::canCannonMove(const Piece b[9][9], int fr, int fc, int tr, int tc) const {
    if (fr != tr && fc != tc) return false;
    int count = countPiecesBetween(b, fr, fc, tr, tc);
    if (b[tr][tc].alive) {
        return count == 1;
    } else {
        return count == 0;
    }
}

int Game::countPiecesBetween(const Piece b[9][9], int fr, int fc, int tr, int tc) const {
    int count = 0;
    if (fr == tr) {
        int minC = std::min(fc, tc);
        int maxC = std::max(fc, tc);
        for (int c = minC + 1; c < maxC; c++) {
            if (b[fr][c].alive) count++;
        }
    } else if (fc == tc) {
        int minR = std::min(fr, tr);
        int maxR = std::max(fr, tr);
        for (int r = minR + 1; r < maxR; r++) {
            if (b[r][fc].alive) count++;
        }
    }
    return count;
}

bool Game::canSoldierMove(const Piece b[9][9], int fr, int fc, int tr, int tc, Side side) const {
    int dr = tr - fr;
    int dc = tc - fc;
    if (std::abs(dr) + std::abs(dc) != 1) return false;
    if (side == Side::RED) {
        return (dr == 1 && dc == 0) || (dr == 0 && dc == -1);
    } else {
        return (dr == -1 && dc == 0) || (dr == 0 && dc == 1);
    }
}

void Game::handleBoardClick(int r, int c) {
    if (gameOver) return;
    if (aiMode && !netMode && currentTurn == aiSide) return;
    if (netState == NetState::CONNECTED && currentTurn != netSide) return;
    if (undoRequestSent || undoRequestReceived || restartRequestSent || restartRequestReceived ||
        surrenderRequestSent || surrenderRequestReceived || drawRequestSent || drawRequestReceived) return;

    if (!pieceSelected) {
        if (board[r][c].alive && board[r][c].side == currentTurn) {
            selectedR = r;
            selectedC = c;
            pieceSelected = true;
            validMoves = getValidMoves(r, c);
        }
    } else {
        if (r == selectedR && c == selectedC) {
            pieceSelected = false;
            validMoves.clear();
            return;
        }

        if (board[r][c].alive && board[r][c].side == currentTurn) {
            selectedR = r;
            selectedC = c;
            validMoves = getValidMoves(r, c);
            return;
        }

        bool isValid = false;
        for (const auto& move : validMoves) {
            if (move.x == r && move.y == c) {
                isValid = true;
                break;
            }
        }

        if (isValid) {
            executeMove(selectedR, selectedC, r, c);
        }

    pieceSelected = false;
    validMoves.clear();
    }
}

void Game::handleButtonClick(float mx, float my) {
    if (showTutorial && mx >= 1090 && mx <= 1140 && my >= 4 && my <= 32) {
        showTutorial = false;
        playClickSound();
        return;
    }
    if (undoAcceptBtn.bounds.contains(mx, my) && undoRequestReceived) {
        if (gameOver) return;
        sf::Packet pkt;
        pkt << 2;
        socket.send(pkt);
        undoRequestReceived = false;
        playClickSound();
        return;
    }
    if (undoRejectBtn.bounds.contains(mx, my) && undoRequestReceived) {
        sf::Packet pkt;
        pkt << 3;
        socket.send(pkt);
        undoRequestReceived = false;
        playClickSound();
        return;
    }
    if (surrenderAcceptBtn.bounds.contains(mx, my) && surrenderRequestReceived) {
        if (gameOver) { surrenderRequestReceived = false; return; }
        sf::Packet pkt;
        pkt << 10;
        socket.send(pkt);
        surrenderRequestReceived = false;
        doSurrender(surrenderRequesterSide);
        playClickSound();
        return;
    }
    if (surrenderRejectBtn.bounds.contains(mx, my) && surrenderRequestReceived) {
        sf::Packet pkt;
        pkt << 11;
        socket.send(pkt);
        surrenderRequestReceived = false;
        playClickSound();
        return;
    }
    if (drawAcceptBtn.bounds.contains(mx, my) && drawRequestReceived) {
        if (gameOver) { drawRequestReceived = false; return; }
        sf::Packet pkt;
        pkt << 13;
        socket.send(pkt);
        drawRequestReceived = false;
        doDraw();
        playClickSound();
        return;
    }
    if (drawRejectBtn.bounds.contains(mx, my) && drawRequestReceived) {
        sf::Packet pkt;
        pkt << 14;
        socket.send(pkt);
        drawRequestReceived = false;
        playClickSound();
        return;
    }
    if (restartAcceptBtn.bounds.contains(mx, my) && restartRequestReceived) {
        if (gameOver) { restartRequestReceived = false; return; }
        sf::Packet pkt;
        pkt << 5;
        socket.send(pkt);
        restartRequestReceived = false;
        playClickSound();
        return;
    }
    if (restartRejectBtn.bounds.contains(mx, my) && restartRequestReceived) {
        sf::Packet pkt;
        pkt << 6;
        socket.send(pkt);
        restartRequestReceived = false;
        playClickSound();
        return;
    }
    if (undoBtn.bounds.contains(mx, my) && !undoBtn.disabled) {
        if (netState == NetState::CONNECTED) {
            if (!undoRequestSent && moveHistory.size() >= 1) {
                undoRequesterSide = netSide;
                sendUndoRequest();
                undoRequestSent = true;
            }
        } else {
            undoMove();
        }
        playClickSound();
    } else if (restartBtn.bounds.contains(mx, my) && !restartBtn.disabled) {
        if (netState == NetState::CONNECTED) {
            if (!restartRequestSent) {
                sendRestartRequest();
                restartRequestSent = true;
            }
        } else {
            restartGame();
        }
        playClickSound();
    } else if (aiBtn.bounds.contains(mx, my) && !aiBtn.disabled) {
        aiMode = !aiMode;
        aiBtn.label = aiMode ? L"AI: \u5f00" : L"AI: \u5173";
        playClickSound();
    } else if (difficultyBtn.bounds.contains(mx, my) && !difficultyBtn.disabled) {
        if (aiDifficulty == AIDifficulty::EASY) {
            aiDifficulty = AIDifficulty::MEDIUM;
            aiDepth = 3;
            difficultyBtn.label = L"\u4e2d\u7b49";
        } else if (aiDifficulty == AIDifficulty::MEDIUM) {
            aiDifficulty = AIDifficulty::HARD;
            aiDepth = 4;
            difficultyBtn.label = L"\u56f0\u96be";
        } else {
            aiDifficulty = AIDifficulty::EASY;
            aiDepth = 2;
            difficultyBtn.label = L"\u7b80\u5355";
        }
        playClickSound();
    } else if (onlineBtn.bounds.contains(mx, my) && !onlineBtn.disabled) {
        netMode = !netMode;
        onlineBtn.label = netMode ? L"\u8054\u673a: \u5f00" : L"\u8054\u673a: \u5173";
        if (!netMode) {
            disconnectNetwork();
            showIPInput = false;
            logDividerY = 440.f;
        } else {
            logDividerY = 550.f;
        }
        playClickSound();
    } else if (hostBtn.bounds.contains(mx, my) && netMode && netState == NetState::OFFLINE) {
        startHost();
        playClickSound();
    } else if (joinBtn.bounds.contains(mx, my) && netMode && netState == NetState::OFFLINE) {
        showIPInput = !showIPInput;
        playClickSound();
    } else if (connectBtn.bounds.contains(mx, my) && showIPInput && !inputIP.empty()) {
        startClient();
        playClickSound();
    } else if (disconnectBtn.bounds.contains(mx, my) && netState != NetState::OFFLINE) {
        disconnectNetwork();
        playClickSound();
    } else if (tutorialBtn.bounds.contains(mx, my) && !tutorialBtn.disabled) {
        showTutorial = !showTutorial;
        playClickSound();
    } else if (surrenderBtn.bounds.contains(mx, my) && !surrenderBtn.disabled) {
        if (aiMode) {
            doSurrender(Side::RED);
        } else if (netState == NetState::CONNECTED) {
            if (!surrenderRequestSent) {
                sendSurrenderRequest();
                surrenderRequestSent = true;
                surrenderRequesterSide = netSide;
            }
        } else {
            showSurrenderPopup = true;
        }
        playClickSound();
    } else if (drawOfferBtn.bounds.contains(mx, my) && !drawOfferBtn.disabled) {
        if (netState == NetState::CONNECTED) {
            if (drawRequestReceived) {
                sf::Packet pkt;
                pkt << 13;
                socket.send(pkt);
                drawRequestReceived = false;
                doDraw();
            } else if (!drawRequestSent) {
                sendDrawRequest();
                drawRequestSent = true;
            }
        } else if (aiMode) {
            doDraw();
        } else {
            doDraw();
        }
        playClickSound();
    }
}

void Game::executeMove(int fromR, int fromC, int toR, int toC) {
    MoveRecord record;
    record.fromR = fromR;
    record.fromC = fromC;
    record.toR = toR;
    record.toC = toC;
    record.movedPiece = board[fromR][fromC];
    record.capturedPiece = board[toR][toC];
    record.side = currentTurn;
    record.prevMovesWithoutCapture = movesWithoutCapture;

    bool captured = board[toR][toC].alive;

    if (captured) {
        playCaptureSound();
    } else {
        playMoveSound();
    }

    if (captured && board[toR][toC].type == PieceType::GENERAL) {
        gameOver = true;
        winner = currentTurn;
        gameOverTimer = 0.f;
        createWinParticles(winner);
        if (winner == Side::RED) {
            playWinSound();
        } else {
            playLoseSound();
        }
    }

    board[toR][toC] = board[fromR][fromC];
    board[fromR][fromC] = {PieceType::NONE, Side::RED, false};

    if (captured) {
        movesWithoutCapture = 0;
    } else {
        movesWithoutCapture++;
    }

    moveHistory.push(record);

    std::wstring moveStr = getMoveString(record);
    moveLogStrings.push_back(moveStr);
    if ((int)moveLogStrings.size() > Game::MAX_LOG * 2) {
        moveLogStrings.erase(moveLogStrings.begin());
    }

    if (netState == NetState::CONNECTED && !receivingMove) {
        sendMove(fromR, fromC, toR, toC);
    }

    currentTurn = (currentTurn == Side::RED) ? Side::BLACK : Side::RED;

    if (!gameOver) {
        checkGameEnd();
    }
}

void Game::undoMove() {
    if (moveHistory.empty()) return;
    if (gameOver) {
        gameOver = false;
        isDrawGame = false;
        gameOverTimer = 0.f;
        particles.clear();
    }

    if (aiMode) {
        if (moveHistory.size() < 2) return;
        for (int i = 0; i < 2; i++) {
            MoveRecord record = moveHistory.top();
            moveHistory.pop();
            board[record.fromR][record.fromC] = record.movedPiece;
            board[record.toR][record.toC] = record.capturedPiece;
            currentTurn = record.side;
            if (!moveLogStrings.empty()) moveLogStrings.pop_back();
        }
        movesWithoutCapture = moveHistory.top().prevMovesWithoutCapture;
        moveLogStrings.push_back(L"\u6094\u68cb");
    } else {
        MoveRecord record = moveHistory.top();
        moveHistory.pop();
        board[record.fromR][record.fromC] = record.movedPiece;
        board[record.toR][record.toC] = record.capturedPiece;
        currentTurn = record.side;
        movesWithoutCapture = record.prevMovesWithoutCapture;
        if (!moveLogStrings.empty()) moveLogStrings.pop_back();
        moveLogStrings.push_back(L"\u6094\u68cb");
    }

    pieceSelected = false;
    validMoves.clear();
    showSurrenderPopup = false;
}

void Game::restartGame() {
    placePieces();
    moveLogStrings.push_back(L"\u91cd\u65b0\u5f00\u59cb");
    pieceSelected = false;
    validMoves.clear();
    undoRequestSent = false;
    undoRequestReceived = false;
    restartRequestSent = false;
    restartRequestReceived = false;
    surrenderRequestSent = false;
    surrenderRequestReceived = false;
    drawRequestSent = false;
    drawRequestReceived = false;
    showSurrenderPopup = false;
}

void Game::doSurrender(Side side) {
    showSurrenderPopup = false;
    gameOver = true;
    winner = (side == Side::RED) ? Side::BLACK : Side::RED;
    isDrawGame = false;
    surrendered = true;
    gameOverTimer = 0.f;
    moveLogStrings.push_back((side == Side::RED) ? L"\u7ea2\u65b9\u8ba4\u8f93" : L"\u9ed1\u65b9\u8ba4\u8f93");
    createWinParticles(winner);
    if (winner == Side::RED) {
        playWinSound();
    } else {
        playLoseSound();
    }
}

void Game::doDraw() {
    gameOver = true;
    isDrawGame = true;
    surrendered = false;
    agreedDraw = true;
    gameOverTimer = 0.f;
    moveLogStrings.push_back(L"\u548c\u68cb");
    createDrawParticles();
    playDrawSound();
}

void Game::checkGameEnd() {
    showSurrenderPopup = false;
    if (movesWithoutCapture >= Game::DRAW_LIMIT) {
        gameOver = true;
        isDrawGame = true;
        gameOverTimer = 0.f;
        createDrawParticles();
        playDrawSound();
        return;
    }

    if (hasInsufficientMaterial()) {
        gameOver = true;
        isDrawGame = true;
        gameOverTimer = 0.f;
        createDrawParticles();
        playDrawSound();
        return;
    }

    if (!hasLegalMoves(currentTurn)) {
        gameOver = true;
        if (isInCheck(currentTurn)) {
            winner = (currentTurn == Side::RED) ? Side::BLACK : Side::RED;
            isDrawGame = false;
        } else {
            isDrawGame = true;
        }
        gameOverTimer = 0.f;
        if (isDrawGame) {
            createDrawParticles();
            playDrawSound();
        } else {
            createWinParticles(winner);
            if (winner == Side::RED) {
                playWinSound();
            } else {
                playLoseSound();
            }
        }
    }
}

bool Game::isDraw() const {
    return isDrawGame;
}

bool Game::hasLegalMoves(Side side) const {
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (board[r][c].alive && board[r][c].side == side) {
                auto moves = getValidMoves(r, c);
                if (!moves.empty()) return true;
            }
        }
    }
    return false;
}

bool Game::isInCheck(Side side) const {
    return isInCheckB(board, side);
}

bool Game::isInCheckB(const Piece b[9][9], Side side) const {
    int gr = -1, gc = -1;
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (b[r][c].alive && b[r][c].type == PieceType::GENERAL && b[r][c].side == side) {
                gr = r;
                gc = c;
                break;
            }
        }
    }
    if (gr < 0 || gc < 0) return false;

    Side opp = (side == Side::RED) ? Side::BLACK : Side::RED;
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (b[r][c].alive && b[r][c].side == opp) {
                if (isValidMoveB(b, r, c, gr, gc)) return true;
            }
        }
    }
    return false;
}

bool Game::isCheckmate(Side side) const {
    if (!isInCheck(side)) return false;
    return !hasLegalMoves(side);
}

bool Game::hasInsufficientMaterial() const {
    int redPieces = 0, blackPieces = 0;
    bool redHasAttack = false, blackHasAttack = false;

    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (!board[r][c].alive) continue;
            if (board[r][c].side == Side::RED) {
                redPieces++;
                if (board[r][c].type != PieceType::GENERAL &&
                    board[r][c].type != PieceType::ADVISOR) {
                    redHasAttack = true;
                }
            } else {
                blackPieces++;
                if (board[r][c].type != PieceType::GENERAL &&
                    board[r][c].type != PieceType::ADVISOR) {
                    blackHasAttack = true;
                }
            }
        }
    }

    if (redPieces <= 1 && blackPieces <= 1) return true;
    if (!redHasAttack && !blackHasAttack) return true;
    return false;
}

bool Game::wouldBeInCheck(int fromR, int fromC, int toR, int toC, Side side) const {
    Piece tempBoard[9][9];
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            tempBoard[r][c] = board[r][c];
        }
    }

    tempBoard[toR][toC] = tempBoard[fromR][fromC];
    tempBoard[fromR][fromC] = {PieceType::NONE, Side::RED, false};

    return isInCheckB(tempBoard, side);
}

bool Game::wouldBeInCheckB(const Piece b[9][9], int fromR, int fromC, int toR, int toC, Side side) const {
    Piece tempBoard[9][9];
    for (int r = 0; r < 9; r++)
        for (int c = 0; c < 9; c++)
            tempBoard[r][c] = b[r][c];
    tempBoard[toR][toC] = tempBoard[fromR][fromC];
    tempBoard[fromR][fromC] = {PieceType::NONE, Side::RED, false};
    return isInCheckB(tempBoard, side);
}

std::wstring Game::getPieceName(PieceType type, Side side) const {
    if (side == Side::RED) {
        switch (type) {
            case PieceType::GENERAL: return L"\u5e05";
            case PieceType::ADVISOR: return L"\u4ed5";
            case PieceType::ELEPHANT: return L"\u76f8";
            case PieceType::HORSE: return L"\u9a6c";
            case PieceType::CHARIOT: return L"\u8f66";
            case PieceType::CANNON: return L"\u70ae";
            case PieceType::SOLDIER: return L"\u5175";
            default: return L"?";
        }
    } else {
        switch (type) {
            case PieceType::GENERAL: return L"\u5c06";
            case PieceType::ADVISOR: return L"\u58eb";
            case PieceType::ELEPHANT: return L"\u8c61";
            case PieceType::HORSE: return L"\u9a6c";
            case PieceType::CHARIOT: return L"\u8f66";
            case PieceType::CANNON: return L"\u70ae";
            case PieceType::SOLDIER: return L"\u5352";
            default: return L"?";
        }
    }
}

std::wstring Game::getMoveString(const MoveRecord& move) const {
    std::wostringstream oss;
    std::wstring sideStr = (move.side == Side::RED) ? L"\u7ea2" : L"\u9ed1";
    std::wstring pieceName = getPieceName(move.movedPiece.type, move.side);
    oss << sideStr << pieceName
        << L" (" << move.fromR << L"," << move.fromC << L")\u2192("
        << move.toR << L"," << move.toC << L")";
    if (move.capturedPiece.alive) {
        oss << L" \u5403" << getPieceName(move.capturedPiece.type, move.capturedPiece.side);
    }
    return oss.str();
}

void Game::drawBoard() {
    sf::ConvexShape bg(4);
    bg.setPoint(0, gridToScreen(0, 0));
    bg.setPoint(1, gridToScreen(8, 0));
    bg.setPoint(2, gridToScreen(8, 8));
    bg.setPoint(3, gridToScreen(0, 8));
    bg.setFillColor(sf::Color(210, 180, 140));
    bg.setOutlineColor(sf::Color(80, 50, 20));
    bg.setOutlineThickness(4);
    window.draw(bg);

    sf::ConvexShape innerBg(4);
    innerBg.setPoint(0, gridToScreen(1, 1));
    innerBg.setPoint(1, gridToScreen(7, 1));
    innerBg.setPoint(2, gridToScreen(7, 7));
    innerBg.setPoint(3, gridToScreen(1, 7));
    innerBg.setFillColor(sf::Color(220, 190, 150));
    window.draw(innerBg);

    sf::Color lineColor(60, 40, 20);

    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 8; c++) {
            sf::Vector2f p1 = gridToScreen(r, c);
            sf::Vector2f p2 = gridToScreen(r, c + 1);
            sf::Vertex line[] = {sf::Vertex(p1, lineColor), sf::Vertex(p2, lineColor)};
            window.draw(line, 2, sf::Lines);
        }
    }

    for (int c = 0; c < 9; c++) {
        for (int r = 0; r < 8; r++) {
            sf::Vector2f p1 = gridToScreen(r, c);
            sf::Vector2f p2 = gridToScreen(r + 1, c);
            sf::Vertex line[] = {sf::Vertex(p1, lineColor), sf::Vertex(p2, lineColor)};
            window.draw(line, 2, sf::Lines);
        }
    }

    sf::Color palaceColor(100, 60, 30, 180);

    for (int r = 0; r < 2; r++) {
        for (int c = 6; c < 8; c++) {
            sf::Vector2f p1 = gridToScreen(r, c);
            sf::Vector2f p2 = gridToScreen(r + 1, c + 1);
            sf::Vertex d1[] = {sf::Vertex(p1, palaceColor), sf::Vertex(p2, palaceColor)};
            window.draw(d1, 2, sf::Lines);

            sf::Vector2f p3 = gridToScreen(r + 1, c);
            sf::Vector2f p4 = gridToScreen(r, c + 1);
            sf::Vertex d2[] = {sf::Vertex(p3, palaceColor), sf::Vertex(p4, palaceColor)};
            window.draw(d2, 2, sf::Lines);
        }
    }

    for (int r = 6; r < 8; r++) {
        for (int c = 0; c < 2; c++) {
            sf::Vector2f p1 = gridToScreen(r, c);
            sf::Vector2f p2 = gridToScreen(r + 1, c + 1);
            sf::Vertex d1[] = {sf::Vertex(p1, palaceColor), sf::Vertex(p2, palaceColor)};
            window.draw(d1, 2, sf::Lines);

            sf::Vector2f p3 = gridToScreen(r + 1, c);
            sf::Vector2f p4 = gridToScreen(r, c + 1);
            sf::Vertex d2[] = {sf::Vertex(p3, palaceColor), sf::Vertex(p4, palaceColor)};
            window.draw(d2, 2, sf::Lines);
        }
    }

    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            sf::Vector2f pos = gridToScreen(r, c);
            sf::CircleShape dot(3);
            dot.setOrigin(3, 3);
            dot.setPosition(pos);
            dot.setFillColor(sf::Color(80, 50, 20));
            window.draw(dot);
        }
    }
}

void Game::drawPieces() {
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (!board[r][c].alive) continue;

            sf::Vector2f pos = gridToScreen(r, c);

            sf::CircleShape shadow(Game::PIECE_R + 2);
            shadow.setOrigin(Game::PIECE_R + 2, Game::PIECE_R + 2);
            shadow.setPosition(pos.x + 2, pos.y + 2);
            shadow.setFillColor(sf::Color(0, 0, 0, 80));
            window.draw(shadow);

            sf::CircleShape piece(Game::PIECE_R);
            piece.setOrigin(Game::PIECE_R, Game::PIECE_R);
            piece.setPosition(pos);

            if (board[r][c].side == Side::RED) {
                piece.setFillColor(sf::Color(180, 30, 30));
                piece.setOutlineColor(sf::Color(120, 20, 20));
            } else {
                piece.setFillColor(sf::Color(40, 40, 40));
                piece.setOutlineColor(sf::Color(20, 20, 20));
            }
            piece.setOutlineThickness(3);
            window.draw(piece);

            sf::CircleShape highlight(Game::PIECE_R - 4);
            highlight.setOrigin(Game::PIECE_R - 4, Game::PIECE_R - 4);
            highlight.setPosition(pos.x - 3, pos.y - 3);
            highlight.setFillColor(sf::Color(255, 255, 255, 40));
            window.draw(highlight);

            if (r == selectedR && c == selectedC) {
                sf::CircleShape hl(Game::PIECE_R + 5);
                hl.setOrigin(Game::PIECE_R + 5, Game::PIECE_R + 5);
                hl.setPosition(pos);
                hl.setFillColor(sf::Color::Transparent);
                hl.setOutlineColor(sf::Color(255, 220, 0, 200));
                hl.setOutlineThickness(4);
                window.draw(hl);
            }

            if (fontLoaded) {
                std::wstring text = getPieceName(board[r][c].type, board[r][c].side);
                sf::Text t;
                t.setFont(font);
                t.setString(text);
                t.setCharacterSize(20);
                t.setFillColor(sf::Color::White);
                sf::FloatRect bounds = t.getLocalBounds();
                t.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
                t.setPosition(pos);
                window.draw(t);
            }
        }
    }

    for (const auto& move : validMoves) {
        sf::Vector2f pos = gridToScreen(move.x, move.y);
        if (board[move.x][move.y].alive) {
            sf::CircleShape ring(Game::PIECE_R + 3);
            ring.setOrigin(Game::PIECE_R + 3, Game::PIECE_R + 3);
            ring.setPosition(pos);
            ring.setFillColor(sf::Color::Transparent);
            ring.setOutlineColor(sf::Color(255, 60, 60, 220));
            ring.setOutlineThickness(4);
            window.draw(ring);
        } else {
            sf::CircleShape dot(10);
            dot.setOrigin(10, 10);
            dot.setPosition(pos);
            dot.setFillColor(sf::Color(80, 220, 80, 180));
            window.draw(dot);
        }
    }
}

void Game::drawUI() {
    if (!fontLoaded) return;

    sf::RectangleShape titleBg(sf::Vector2f(420, 50));
    titleBg.setPosition(715, 20);
    titleBg.setFillColor(sf::Color(65, 50, 35));
    titleBg.setOutlineColor(sf::Color(90, 75, 55));
    titleBg.setOutlineThickness(1);
    window.draw(titleBg);
    drawTextWithShadow(L"\u5bf9\u89d2\u8c61\u68cb", 925, 45, 26, sf::Color(220, 200, 170), true);

    sf::RectangleShape turnBg(sf::Vector2f(420, 70));
    turnBg.setPosition(715, 80);
    turnBg.setFillColor(sf::Color(60, 48, 34));
    turnBg.setOutlineColor(sf::Color(85, 70, 50));
    turnBg.setOutlineThickness(1);
    window.draw(turnBg);

    std::wstring turnText;
    sf::Color turnColor;
    sf::Color indicatorColor;
    
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
            } else if (isCheckmate((winner == Side::RED) ? Side::BLACK : Side::RED)) {
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
    window.draw(indicator);
    
    drawTextWithShadow(turnText, 935, 115, 30, turnColor, true);

    if (!gameOver && isInCheck(currentTurn)) {
        sf::RectangleShape checkBg(sf::Vector2f(140, 40));
        checkBg.setPosition(855, 160);
        checkBg.setFillColor(sf::Color(180, 40, 40, 220));
        checkBg.setOutlineColor(sf::Color(255, 120, 120));
        checkBg.setOutlineThickness(2);
        window.draw(checkBg);
        drawTextWithShadow(L"\u5c06\u519b\uff01", 925, 180, 24, sf::Color(255, 255, 150), true);
    }

    if (aiThinking) {
        drawText(L"AI\u601d\u8003\u4e2d...", 925, 210, 18, sf::Color(200, 180, 150), true);
    }

    if (!notificationText.empty() && notificationTimer > 0.f) {
        float alpha = std::min(notificationTimer / 3.f, 1.f) * 255.f;
        sf::Color notifColor(255, 100, 60, static_cast<sf::Uint8>(alpha));
        sf::RectangleShape notifBg(sf::Vector2f(300, 30));
        notifBg.setPosition(820, 170);
        notifBg.setFillColor(sf::Color(180, 60, 30, static_cast<sf::Uint8>(alpha * 0.6f)));
        notifBg.setOutlineColor(sf::Color(255, 100, 50, static_cast<sf::Uint8>(alpha * 0.8f)));
        notifBg.setOutlineThickness(2);
        window.draw(notifBg);
        drawTextWithShadow(notificationText, 970, 185, 16, notifColor, true);
    }
}

void Game::drawButtons() {
    auto drawBtn = [this](const UIButton& btn) {
        sf::RectangleShape shadow(sf::Vector2f(btn.bounds.width, btn.bounds.height));
        shadow.setPosition(btn.bounds.left + 2, btn.bounds.top + 2);
        shadow.setFillColor(sf::Color(20, 15, 10, 100));
        window.draw(shadow);

        sf::RectangleShape rect(sf::Vector2f(btn.bounds.width, btn.bounds.height));
        rect.setPosition(btn.bounds.left, btn.bounds.top);

        sf::Color fillColor;
        sf::Color outlineColor;
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
        window.draw(rect);

        if (fontLoaded) {
            sf::Text t;
            t.setFont(font);
            t.setString(btn.label);
            t.setCharacterSize(18);
            sf::Color textColor = btn.disabled ? sf::Color(110, 100, 88) : sf::Color(245, 235, 220);
            t.setFillColor(textColor);
            sf::FloatRect bounds = t.getLocalBounds();
            t.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
            t.setPosition(btn.bounds.left + btn.bounds.width / 2.f,
                         btn.bounds.top + btn.bounds.height / 2.f);
            window.draw(t);
        }
    };

    drawBtn(undoBtn);
    drawBtn(restartBtn);
    drawBtn(aiBtn);
    drawBtn(difficultyBtn);
    drawBtn(onlineBtn);
    drawBtn(tutorialBtn);
    drawBtn(surrenderBtn);
    drawBtn(drawOfferBtn);

    auto drawSmallBtn = [this](const UIButton& btn, sf::Color fill, sf::Color outline) {
        sf::RectangleShape rect(sf::Vector2f(btn.bounds.width, btn.bounds.height));
        rect.setPosition(btn.bounds.left, btn.bounds.top);
        rect.setFillColor(btn.hovered ? sf::Color(fill.r + 40, fill.g + 30, fill.b + 20) : fill);
        rect.setOutlineColor(outline);
        rect.setOutlineThickness(2);
        window.draw(rect);
        sf::Text t;
        t.setFont(font);
        t.setString(btn.label);
        t.setCharacterSize(14);
        t.setFillColor(sf::Color(255, 255, 255));
        sf::FloatRect bounds = t.getLocalBounds();
        t.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
        t.setPosition(btn.bounds.left + btn.bounds.width / 2.f,
                     btn.bounds.top + btn.bounds.height / 2.f);
        window.draw(t);
    };

    if (undoRequestReceived) {
        sf::RectangleShape bg(sf::Vector2f(200, 70));
        bg.setPosition(940, 240);
        bg.setFillColor(sf::Color(180, 130, 30, 220));
        bg.setOutlineColor(sf::Color(255, 180, 50));
        bg.setOutlineThickness(2);
        window.draw(bg);
        drawText(L"\u5bf9\u65b9\u8bf7\u6c42\u6094\u68cb", 1040, 260, 16, sf::Color(255, 255, 200), true);
        undoAcceptBtn.bounds = sf::FloatRect(940, 275, 95, 44);
        undoRejectBtn.bounds = sf::FloatRect(1045, 275, 95, 44);
        drawSmallBtn(undoAcceptBtn, sf::Color(60, 140, 60), sf::Color(80, 180, 80));
        drawSmallBtn(undoRejectBtn, sf::Color(160, 50, 50), sf::Color(200, 80, 80));
    }

    if (restartRequestReceived) {
        sf::RectangleShape bg(sf::Vector2f(200, 70));
        bg.setPosition(940, 290);
        bg.setFillColor(sf::Color(180, 130, 30, 220));
        bg.setOutlineColor(sf::Color(255, 180, 50));
        bg.setOutlineThickness(2);
        window.draw(bg);
        drawText(L"\u5bf9\u65b9\u8bf7\u6c42\u91cd\u5f00", 1040, 310, 16, sf::Color(255, 255, 200), true);
        restartAcceptBtn.bounds = sf::FloatRect(940, 325, 95, 44);
        restartRejectBtn.bounds = sf::FloatRect(1045, 325, 95, 44);
        drawSmallBtn(restartAcceptBtn, sf::Color(60, 140, 60), sf::Color(80, 180, 80));
        drawSmallBtn(restartRejectBtn, sf::Color(160, 50, 50), sf::Color(200, 80, 80));
    }

    if (surrenderRequestReceived) {
        sf::RectangleShape bg(sf::Vector2f(200, 70));
        bg.setPosition(940, 340);
        bg.setFillColor(sf::Color(180, 40, 40, 220));
        bg.setOutlineColor(sf::Color(255, 80, 80));
        bg.setOutlineThickness(2);
        window.draw(bg);
        drawText(L"\u5bf9\u65b9\u8ba4\u8f93\u4e86", 1040, 360, 16, sf::Color(255, 220, 200), true);
        surrenderAcceptBtn.bounds = sf::FloatRect(940, 375, 95, 44);
        surrenderRejectBtn.bounds = sf::FloatRect(1045, 375, 95, 44);
        drawSmallBtn(surrenderAcceptBtn, sf::Color(60, 140, 60), sf::Color(80, 180, 80));
        drawSmallBtn(surrenderRejectBtn, sf::Color(160, 50, 50), sf::Color(200, 80, 80));
    }

    if (drawRequestReceived) {
        sf::RectangleShape bg(sf::Vector2f(200, 70));
        bg.setPosition(940, 390);
        bg.setFillColor(sf::Color(180, 180, 40, 220));
        bg.setOutlineColor(sf::Color(255, 255, 80));
        bg.setOutlineThickness(2);
        window.draw(bg);
        drawText(L"\u5bf9\u65b9\u8bf7\u6c42\u548c\u68cb", 1040, 410, 16, sf::Color(255, 255, 200), true);
        drawAcceptBtn.bounds = sf::FloatRect(940, 425, 95, 44);
        drawRejectBtn.bounds = sf::FloatRect(1045, 425, 95, 44);
        drawSmallBtn(drawAcceptBtn, sf::Color(60, 140, 60), sf::Color(80, 180, 80));
        drawSmallBtn(drawRejectBtn, sf::Color(160, 50, 50), sf::Color(200, 80, 80));
    }
}

void Game::drawMoveLog() {
    if (!fontLoaded) return;

    float logY = logDividerY;
    float logH = 810.f - logDividerY;

    sf::RectangleShape logBg(sf::Vector2f(420, logH));
    logBg.setPosition(715, logY);
    logBg.setFillColor(sf::Color(55, 44, 32));
    logBg.setOutlineColor(sf::Color(85, 70, 52));
    logBg.setOutlineThickness(1);
    window.draw(logBg);

    sf::RectangleShape logHeader(sf::Vector2f(420, 30));
    logHeader.setPosition(715, logY);
    logHeader.setFillColor(sf::Color(70, 56, 40));
    window.draw(logHeader);
    
    drawTextWithShadow(L"\u8d70\u68cb\u8bb0\u5f55", 925, logY + 15, 16, sf::Color(230, 210, 180), true);

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
            window.draw(rowBg);
        }
        std::wostringstream oss;
        oss << (i + 1) << L". " << moveLogStrings[i];
        sf::Color c = (i % 2 == 0) ? sf::Color(255, 140, 140) : sf::Color(220, 220, 220);
        drawText(oss.str(), 730, y, 13, c);
        y += 22;
    }
}

void Game::drawGameOverEffect() {
    float alpha = std::min(gameOverTimer * 200.f, 180.f);

    sf::RectangleShape overlay(sf::Vector2f(Game::WIN_W, Game::WIN_H));
    overlay.setPosition(0, 0);
    overlay.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(alpha)));
    window.draw(overlay);

    if (gameOverTimer > 0.5f) {
        float textAlpha = std::min((gameOverTimer - 0.5f) * 400.f, 255.f);

        std::wstring mainText;
        std::wstring subText;
        sf::Color mainColor;

        if (isDrawGame) {
            mainText = L"\u548c\u68cb";
            mainColor = sf::Color(255, 255, 100, static_cast<sf::Uint8>(textAlpha));
            if (movesWithoutCapture >= Game::DRAW_LIMIT) {
                subText = L"\u81ea\u7136\u9650\u7740\u548c\u68cb";
            } else if (hasInsufficientMaterial()) {
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
            } else if (isCheckmate(loser)) {
                mainText = L"\u7edd\u6740";
                mainColor = sf::Color(255, 215, 0, static_cast<sf::Uint8>(textAlpha));
                subText = (winner == Side::RED) ? L"\u7ea2\u65b9\u7edd\u6740\u9ed1\u65b9" : L"\u9ed1\u65b9\u7edd\u6740\u7ea2\u65b9";
            } else {
                mainText = L"\u56f0\u6bd9";
                mainColor = sf::Color(255, 180, 50, static_cast<sf::Uint8>(textAlpha));
                subText = (loser == Side::RED) ? L"\u7ea2\u65b9\u88ab\u56f0\u6bd9" : L"\u9ed1\u65b9\u88ab\u56f0\u6bd9";
            }
        }

        drawTextWithShadow(mainText, Game::WIN_W / 2.f, Game::WIN_H / 2.f - 50, 72, mainColor, true);
        
        sf::Color subColor(200, 200, 200, static_cast<sf::Uint8>(textAlpha));
        drawTextWithShadow(subText, Game::WIN_W / 2.f, Game::WIN_H / 2.f + 20, 28, subColor, true);

        float btnX = Game::WIN_W / 2.f - 100;
        float btnY = Game::WIN_H / 2.f + 80;
        gameOverRestartBtn.bounds.left = btnX;
        gameOverRestartBtn.bounds.top = btnY;
        
        sf::RectangleShape btnShadow(sf::Vector2f(200, 50));
        btnShadow.setPosition(btnX + 3, btnY + 3);
        btnShadow.setFillColor(sf::Color(0, 0, 0, static_cast<sf::Uint8>(textAlpha * 0.5f)));
        window.draw(btnShadow);
        
        sf::RectangleShape btnRect(sf::Vector2f(200, 50));
        btnRect.setPosition(btnX, btnY);
        sf::Color btnColor = gameOverRestartBtn.hovered ? 
            sf::Color(140, 110, 80, static_cast<sf::Uint8>(textAlpha)) :
            sf::Color(100, 80, 60, static_cast<sf::Uint8>(textAlpha));
        btnRect.setFillColor(btnColor);
        btnRect.setOutlineColor(sf::Color(180, 150, 110, static_cast<sf::Uint8>(textAlpha)));
        btnRect.setOutlineThickness(2);
        window.draw(btnRect);
        
        sf::Color btnTextColor(255, 255, 255, static_cast<sf::Uint8>(textAlpha));
        drawTextWithShadow(L"\u91cd\u65b0\u5f00\u59cb", btnX + 100, btnY + 25, 22, btnTextColor, true);
    }
}

void Game::drawText(const std::wstring& text, float x, float y,
                    unsigned int charSize, sf::Color color, bool center) {
    sf::Text t;
    t.setFont(font);
    t.setString(text);
    t.setCharacterSize(charSize);
    t.setFillColor(color);

    if (center) {
        sf::FloatRect bounds = t.getLocalBounds();
        t.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
    }
    t.setPosition(x, y);
    window.draw(t);
}

void Game::drawTextWithShadow(const std::wstring& text, float x, float y,
                               unsigned int charSize, sf::Color color, bool center) {
    sf::Color shadowColor(0, 0, 0, color.a / 2);
    drawText(text, x + 2, y + 2, charSize, shadowColor, center);
    drawText(text, x, y, charSize, color, center);
}

void Game::updateParticles(float dt) {
    for (auto it = particles.begin(); it != particles.end();) {
        it->pos += it->vel * dt;
        it->vel.y += 200.f * dt;
        it->life -= dt;

        if (it->life <= 0) {
            it = particles.erase(it);
        } else {
            ++it;
        }
    }
}

void Game::drawParticles() {
    for (const auto& p : particles) {
        float alpha = (p.life / p.maxLife) * 255.f;
        sf::Color c = p.color;
        c.a = static_cast<sf::Uint8>(alpha);

        sf::CircleShape particle(p.size);
        particle.setOrigin(p.size, p.size);
        particle.setPosition(p.pos);
        particle.setFillColor(c);
        window.draw(particle);
    }
}

void Game::createWinParticles(Side winner) {
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> xDist(100, 600);
    std::uniform_real_distribution<float> yDist(200, 600);
    std::uniform_real_distribution<float> velX(-150, 150);
    std::uniform_real_distribution<float> velY(-400, -200);
    std::uniform_real_distribution<float> size(3, 8);

    sf::Color color1, color2;
    if (winner == Side::RED) {
        color1 = sf::Color(255, 100, 100);
        color2 = sf::Color(255, 200, 50);
    } else {
        color1 = sf::Color(200, 200, 200);
        color2 = sf::Color(100, 150, 255);
    }

    for (int i = 0; i < 100; i++) {
        Particle p;
        p.pos = sf::Vector2f(xDist(rng), yDist(rng));
        p.vel = sf::Vector2f(velX(rng), velY(rng));
        p.color = (i % 2 == 0) ? color1 : color2;
        p.life = 3.0f;
        p.maxLife = 3.0f;
        p.size = size(rng);
        particles.push_back(p);
    }
}

void Game::createDrawParticles() {
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> xDist(100, 600);
    std::uniform_real_distribution<float> yDist(200, 600);
    std::uniform_real_distribution<float> velX(-100, 100);
    std::uniform_real_distribution<float> velY(-300, -150);
    std::uniform_real_distribution<float> size(2, 6);

    for (int i = 0; i < 80; i++) {
        Particle p;
        p.pos = sf::Vector2f(xDist(rng), yDist(rng));
        p.vel = sf::Vector2f(velX(rng), velY(rng));
        p.color = sf::Color(200, 200, 100);
        p.life = 2.5f;
        p.maxLife = 2.5f;
        p.size = size(rng);
        particles.push_back(p);
    }
}

void Game::playMoveSound() {
    if (soundsLoaded) {
        moveSound.play();
    }
}

void Game::playCaptureSound() {
    if (soundsLoaded) {
        captureSound.play();
    }
}

void Game::playWinSound() {
    if (soundsLoaded) {
        winSound.play();
    }
}

void Game::playLoseSound() {
    if (soundsLoaded) {
        loseSound.play();
    }
}

void Game::playDrawSound() {
    if (soundsLoaded) {
        drawSound.play();
    }
}

void Game::playClickSound() {
    if (soundsLoaded) {
        clickSound.play();
    }
}

void Game::initSounds() {
    const int sampleRate = 44100;
    const float PI = 3.14159265f;
    
    std::vector<sf::Int16> moveSamples(sampleRate * 0.1f);
    for (size_t i = 0; i < moveSamples.size(); i++) {
        float t = (float)i / sampleRate;
        float envelope = 1.0f - t / 0.1f;
        float wave = std::sin(2 * PI * 800 * t) * 0.3f;
        moveSamples[i] = (sf::Int16)(wave * envelope * 32767);
    }
    moveSoundBuffer.loadFromSamples(&moveSamples[0], moveSamples.size(), 1, sampleRate);
    moveSound.setBuffer(moveSoundBuffer);
    
    std::vector<sf::Int16> captureSamples(sampleRate * 0.15f);
    for (size_t i = 0; i < captureSamples.size(); i++) {
        float t = (float)i / sampleRate;
        float envelope = 1.0f - t / 0.15f;
        float wave = std::sin(2 * PI * 600 * t) * 0.4f + std::sin(2 * PI * 900 * t) * 0.3f;
        captureSamples[i] = (sf::Int16)(wave * envelope * 32767);
    }
    captureSoundBuffer.loadFromSamples(&captureSamples[0], captureSamples.size(), 1, sampleRate);
    captureSound.setBuffer(captureSoundBuffer);
    
    std::vector<sf::Int16> winSamples(sampleRate * 0.8f);
    for (size_t i = 0; i < winSamples.size(); i++) {
        float t = (float)i / sampleRate;
        float envelope = (t < 0.1f) ? t / 0.1f : (t > 0.7f ? (0.8f - t) / 0.1f : 1.0f);
        float freq = 523.25f + (t / 0.8f) * 261.63f;
        float wave = std::sin(2 * PI * freq * t) * 0.3f + std::sin(2 * PI * freq * 1.5f * t) * 0.2f;
        winSamples[i] = (sf::Int16)(wave * envelope * 32767);
    }
    winSoundBuffer.loadFromSamples(&winSamples[0], winSamples.size(), 1, sampleRate);
    winSound.setBuffer(winSoundBuffer);
    
    std::vector<sf::Int16> loseSamples(sampleRate * 0.6f);
    for (size_t i = 0; i < loseSamples.size(); i++) {
        float t = (float)i / sampleRate;
        float envelope = (t < 0.05f) ? t / 0.05f : (t > 0.5f ? (0.6f - t) / 0.1f : 1.0f);
        float freq = 440.0f - (t / 0.6f) * 110.0f;
        float wave = std::sin(2 * PI * freq * t) * 0.35f;
        loseSamples[i] = (sf::Int16)(wave * envelope * 32767);
    }
    loseSoundBuffer.loadFromSamples(&loseSamples[0], loseSamples.size(), 1, sampleRate);
    loseSound.setBuffer(loseSoundBuffer);
    
    std::vector<sf::Int16> drawSamples(sampleRate * 0.5f);
    for (size_t i = 0; i < drawSamples.size(); i++) {
        float t = (float)i / sampleRate;
        float envelope = (t < 0.05f) ? t / 0.05f : (t > 0.4f ? (0.5f - t) / 0.1f : 1.0f);
        float wave = std::sin(2 * PI * 440 * t) * 0.25f + std::sin(2 * PI * 554.37f * t) * 0.25f;
        drawSamples[i] = (sf::Int16)(wave * envelope * 32767);
    }
    drawSoundBuffer.loadFromSamples(&drawSamples[0], drawSamples.size(), 1, sampleRate);
    drawSound.setBuffer(drawSoundBuffer);
    
    std::vector<sf::Int16> clickSamples(sampleRate * 0.05f);
    for (size_t i = 0; i < clickSamples.size(); i++) {
        float t = (float)i / sampleRate;
        float envelope = 1.0f - t / 0.05f;
        float wave = std::sin(2 * PI * 1200 * t) * 0.2f;
        clickSamples[i] = (sf::Int16)(wave * envelope * 32767);
    }
    clickSoundBuffer.loadFromSamples(&clickSamples[0], clickSamples.size(), 1, sampleRate);
    clickSound.setBuffer(clickSoundBuffer);
    
    soundsLoaded = true;
}

void Game::doAITurn() {
    Piece tempBoard[9][9];
    for (int r = 0; r < 9; r++)
        for (int c = 0; c < 9; c++)
            tempBoard[r][c] = board[r][c];

    auto moves = generateAllMoves(tempBoard, aiSide);
    if (moves.empty()) return;

    int bestScore = (aiSide == Side::RED) ? -999999 : 999999;
    AIMove best = moves[0];

    for (auto& move : moves) {
        Piece captured;
        makeMoveAI(tempBoard, move, captured);
        Piece moved = tempBoard[move.toR][move.toC];

        int score = minimax(tempBoard, aiDepth - 1, -999999, 999999,
                           aiSide == Side::BLACK, aiSide);

        undoMoveAI(tempBoard, move, captured, moved);

        if (aiSide == Side::RED) {
            if (score > bestScore) {
                bestScore = score;
                best = move;
            }
        } else {
            if (score < bestScore) {
                bestScore = score;
                best = move;
            }
        }
    }

    executeMove(best.fromR, best.fromC, best.toR, best.toC);
}

std::vector<AIMove> Game::generateAllMoves(const Piece b[9][9], Side side) const {
    std::vector<AIMove> moves;
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (!b[r][c].alive || b[r][c].side != side) continue;
            for (int tr = 0; tr < 9; tr++) {
                for (int tc = 0; tc < 9; tc++) {
                    if (tr == r && tc == c) continue;
                    if (b[tr][tc].alive && b[tr][tc].side == side) continue;
                    if (isValidMoveB(b, r, c, tr, tc)) {
                        if (wouldBeInCheckB(b, r, c, tr, tc, b[r][c].side)) continue;
                        AIMove m;
                        m.fromR = r; m.fromC = c;
                        m.toR = tr; m.toC = tc;
                        m.score = 0;
                        moves.push_back(m);
                }
            }
        }
    }
    }
    return moves;
}

int Game::evaluate(const Piece b[9][9]) const {
    bool redGeneral = false, blackGeneral = false;
    int score = 0;

    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (!b[r][c].alive) continue;
            if (b[r][c].type == PieceType::GENERAL) {
                if (b[r][c].side == Side::RED) redGeneral = true;
                else blackGeneral = true;
            }
            int val = getPieceValue(b[r][c].type) + getPositionBonus(b[r][c].type, r, c, b[r][c].side);
            if (b[r][c].side == Side::RED) score += val;
            else score -= val;
        }
    }

    if (!redGeneral) return -100000;
    if (!blackGeneral) return 100000;
    return score;
}

int Game::minimax(Piece b[9][9], int depth, int alpha, int beta, bool isMaximizing, Side aiSide) {
    if (depth == 0) return evaluate(b);

    Side currentSide = isMaximizing ? Side::RED : Side::BLACK;
    auto moves = generateAllMoves(b, currentSide);

    if (moves.empty()) {
        return isMaximizing ? -100000 : 100000;
    }

    if (isMaximizing) {
        int maxEval = -999999;
        for (const auto& move : moves) {
            Piece captured;
            makeMoveAI(b, move, captured);
            Piece moved = b[move.toR][move.toC];
            int eval = minimax(b, depth - 1, alpha, beta, false, aiSide);
            undoMoveAI(b, move, captured, moved);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) break;
        }
        return maxEval;
    } else {
        int minEval = 999999;
        for (const auto& move : moves) {
            Piece captured;
            makeMoveAI(b, move, captured);
            Piece moved = b[move.toR][move.toC];
            int eval = minimax(b, depth - 1, alpha, beta, true, aiSide);
            undoMoveAI(b, move, captured, moved);
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) break;
        }
        return minEval;
    }
}

void Game::makeMoveAI(Piece b[9][9], const AIMove& move, Piece& captured) const {
    captured = b[move.toR][move.toC];
    b[move.toR][move.toC] = b[move.fromR][move.fromC];
    b[move.fromR][move.fromC] = {PieceType::NONE, Side::RED, false};
}

void Game::undoMoveAI(Piece b[9][9], const AIMove& move, const Piece& captured, const Piece& moved) const {
    b[move.fromR][move.fromC] = moved;
    b[move.toR][move.toC] = captured;
}

int Game::getPieceValue(PieceType type) const {
    switch (type) {
        case PieceType::GENERAL: return 10000;
        case PieceType::CHARIOT: return 900;
        case PieceType::CANNON: return 450;
        case PieceType::HORSE: return 400;
        case PieceType::ELEPHANT: return 200;
        case PieceType::ADVISOR: return 200;
        case PieceType::SOLDIER: return 100;
        default: return 0;
    }
}

int Game::getPositionBonus(PieceType type, int r, int c, Side side) const {
    int dist = std::abs(r - 4) + std::abs(c - 4);
    switch (type) {
        case PieceType::SOLDIER:
            if (side == Side::RED) return r * 10;
            else return (8 - r) * 10;
        case PieceType::HORSE:
            if (dist <= 3) return 30;
            return 0;
        case PieceType::CHARIOT:
            if (dist <= 4) return 20;
            return 0;
        case PieceType::CANNON:
            if (dist <= 4) return 10;
            return 0;
        default:
            return 0;
    }
}

void Game::startHost() {
    localIP = getLocalIP();
    listener.setBlocking(false);
    if (listener.listen(55001) == sf::Socket::Done) {
        netState = NetState::HOST_WAITING;
        netSide = Side::RED;
        undoRequestSent = false;
        undoRequestReceived = false;
        restartRequestSent = false;
        restartRequestReceived = false;
        surrenderRequestSent = false;
        surrenderRequestReceived = false;
        drawRequestSent = false;
        drawRequestReceived = false;
    }
}

void Game::startClient() {
    socket.setBlocking(true);
    std::string ipStr(inputIP.begin(), inputIP.end());
    sf::Socket::Status status = socket.connect(sf::IpAddress(ipStr), 55001, sf::seconds(3));
    if (status == sf::Socket::Done) {
        socket.setBlocking(false);
        netState = NetState::CONNECTED;
        netSide = Side::BLACK;
        aiMode = false;
        aiBtn.label = L"AI: \u5173\u95ed";
        showIPInput = false;
        undoRequestSent = false;
        undoRequestReceived = false;
        restartRequestSent = false;
        restartRequestReceived = false;
        surrenderRequestSent = false;
        surrenderRequestReceived = false;
        drawRequestSent = false;
        drawRequestReceived = false;
        restartGame();
    } else {
        showIPInput = true;
    }
}

void Game::sendMove(int fromR, int fromC, int toR, int toC) {
    sf::Packet packet;
    packet << 0 << fromR << fromC << toR << toC;
    socket.send(packet);
}

void Game::sendUndoRequest() {
    sf::Packet packet;
    packet << 1;
    socket.send(packet);
}

void Game::sendRestartRequest() {
    sf::Packet packet;
    packet << 4;
    socket.send(packet);
}

void Game::sendSurrenderRequest() {
    sf::Packet packet;
    packet << 9;
    socket.send(packet);
}

void Game::sendSurrenderResponse(bool accept) {
    sf::Packet packet;
    packet << (accept ? 10 : 11);
    socket.send(packet);
}

void Game::sendDrawRequest() {
    sf::Packet packet;
    packet << 12;
    socket.send(packet);
}

void Game::sendDrawResponse(bool accept) {
    sf::Packet packet;
    packet << (accept ? 13 : 14);
    socket.send(packet);
}

void Game::applyUndoSteps(int steps) {
    if ((int)moveHistory.size() < steps) return;
    for (int i = 0; i < steps; i++) {
        MoveRecord record = moveHistory.top();
        moveHistory.pop();
        board[record.fromR][record.fromC] = record.movedPiece;
        board[record.toR][record.toC] = record.capturedPiece;
        currentTurn = record.side;
        if (!moveLogStrings.empty()) moveLogStrings.pop_back();
    }
    if (!moveHistory.empty()) {
        movesWithoutCapture = moveHistory.top().prevMovesWithoutCapture;
    } else {
        movesWithoutCapture = 0;
    }
    pieceSelected = false;
    validMoves.clear();
    if (gameOver) {
        gameOver = false;
        isDrawGame = false;
        gameOverTimer = 0.f;
        particles.clear();
    }
}

void Game::pollNetwork() {
    sf::Packet packet;
    sf::Socket::Status status = socket.receive(packet);
    if (status == sf::Socket::Done) {
        int msgType;
        packet >> msgType;
        if (msgType == 0) {
            int fromR, fromC, toR, toC;
            packet >> fromR >> fromC >> toR >> toC;
            if (gameOver) return;
            if (undoRequestSent || restartRequestSent || surrenderRequestSent || drawRequestSent) return;
            receivingMove = true;
            executeMove(fromR, fromC, toR, toC);
            receivingMove = false;
        } else if (msgType == 1) {
            if (undoRequestSent) {
                undoRequestSent = false;
                if (!gameOver && !moveHistory.empty()) {
                    int steps = (moveHistory.top().side == netSide) ? 1 : 2;
                    applyUndoSteps(steps);
                    sf::Packet undoPkt;
                    undoPkt << 7 << steps;
                    socket.send(undoPkt);
                }
            } else {
                undoRequestReceived = true;
                undoRequesterSide = (netSide == Side::RED) ? Side::BLACK : Side::RED;
            }
        } else if (msgType == 2) {
            undoRequestSent = false;
            if (!gameOver && !moveHistory.empty()) {
                int steps = (moveHistory.top().side == netSide) ? 1 : 2;
                applyUndoSteps(steps);
                sf::Packet undoPkt;
                undoPkt << 7 << steps;
                socket.send(undoPkt);
            }
        } else if (msgType == 3) {
            undoRequestSent = false;
            notificationText = L"\u5bf9\u65b9\u62d2\u7edd\u4e86\u6094\u68cb\u8bf7\u6c42";
            notificationTimer = 3.f;
        } else if (msgType == 4) {
            if (restartRequestSent) {
                restartRequestSent = false;
                if (!gameOver) {
                    restartGame();
                    sf::Packet rstPkt;
                    rstPkt << 8;
                    socket.send(rstPkt);
                }
            } else {
                restartRequestReceived = true;
            }
        } else if (msgType == 5) {
            restartRequestSent = false;
            if (!gameOver) {
                restartGame();
                sf::Packet rstPkt;
                rstPkt << 8;
                socket.send(rstPkt);
            }
        } else if (msgType == 6) {
            restartRequestSent = false;
            notificationText = L"\u5bf9\u65b9\u62d2\u7edd\u4e86\u91cd\u5f00\u8bf7\u6c42";
            notificationTimer = 3.f;
        } else if (msgType == 7) {
            int steps;
            packet >> steps;
            applyUndoSteps(steps);
        } else if (msgType == 8) {
            restartGame();
        } else if (msgType == 9) {
            if (surrenderRequestSent) {
                surrenderRequestSent = false;
                if (!gameOver) doSurrender(netSide);
            } else {
                surrenderRequestReceived = true;
                surrenderRequesterSide = (netSide == Side::RED) ? Side::BLACK : Side::RED;
            }
        } else if (msgType == 10) {
            surrenderRequestSent = false;
            if (!gameOver) doSurrender(netSide);
        } else if (msgType == 11) {
            surrenderRequestSent = false;
            notificationText = L"\u5bf9\u65b9\u62d2\u7edd\u4e86\u8ba4\u8f93\u8bf7\u6c42";
            notificationTimer = 3.f;
        } else if (msgType == 12) {
            if (drawRequestSent) {
                drawRequestSent = false;
                if (!gameOver) doDraw();
            } else {
                drawRequestReceived = true;
            }
        } else if (msgType == 13) {
            drawRequestSent = false;
            if (!gameOver) doDraw();
        } else if (msgType == 14) {
            drawRequestSent = false;
            notificationText = L"\u5bf9\u65b9\u62d2\u7edd\u4e86\u548c\u68cb\u8bf7\u6c42";
            notificationTimer = 3.f;
        }
    } else if (status == sf::Socket::Disconnected || status == sf::Socket::Error) {
        disconnectNetwork();
    }
}

void Game::disconnectNetwork() {
    if (netState != NetState::OFFLINE) {
        socket.disconnect();
        listener.close();
        netState = NetState::OFFLINE;
        netMode = false;
        onlineBtn.label = L"\u8054\u673a: \u5173\u95ed";
        showIPInput = false;
        logDividerY = 440.f;
        undoRequestSent = false;
        undoRequestReceived = false;
        restartRequestSent = false;
        restartRequestReceived = false;
        surrenderRequestSent = false;
        surrenderRequestReceived = false;
        drawRequestSent = false;
        drawRequestReceived = false;
    }
}

std::wstring Game::getLocalIP() const {
    sf::IpAddress ip = sf::IpAddress::getLocalAddress();
    std::string ipStr = ip.toString();
    return std::wstring(ipStr.begin(), ipStr.end());
}

void Game::drawNetUI() {
    if (!fontLoaded) return;
    if (!netMode) return;

    if (netState == NetState::OFFLINE) {
        auto drawNetBtn = [this](const UIButton& btn) {
            sf::RectangleShape shadow(sf::Vector2f(btn.bounds.width, btn.bounds.height));
            shadow.setPosition(btn.bounds.left + 2, btn.bounds.top + 2);
            shadow.setFillColor(sf::Color(20, 15, 10, 100));
            window.draw(shadow);

            sf::RectangleShape rect(sf::Vector2f(btn.bounds.width, btn.bounds.height));
            rect.setPosition(btn.bounds.left, btn.bounds.top);
            sf::Color fillColor = btn.hovered ? sf::Color(80, 120, 80) : sf::Color(60, 90, 60);
            rect.setFillColor(fillColor);
            rect.setOutlineColor(sf::Color(100, 150, 100));
            rect.setOutlineThickness(2);
            window.draw(rect);

            sf::Text t;
            t.setFont(font);
            t.setString(btn.label);
            t.setCharacterSize(18);
            t.setFillColor(sf::Color(230, 255, 230));
            sf::FloatRect bounds = t.getLocalBounds();
            t.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
            t.setPosition(btn.bounds.left + btn.bounds.width / 2.f,
                         btn.bounds.top + btn.bounds.height / 2.f);
            window.draw(t);
        };

        if (showIPInput) {
            sf::RectangleShape inputBg(sf::Vector2f(200, 35));
            inputBg.setPosition(730, 440);
            inputBg.setFillColor(sf::Color(40, 35, 28));
            inputBg.setOutlineColor(sf::Color(120, 100, 80));
            inputBg.setOutlineThickness(2);
            window.draw(inputBg);

            std::wstring displayIP = inputIP.empty() ? L"\u8f93\u5165IP\u5730\u5740..." : inputIP;
            sf::Color ipColor = inputIP.empty() ? sf::Color(120, 110, 100) : sf::Color(240, 230, 220);
            drawText(displayIP, 740, 447, 15, ipColor);

            drawNetBtn(connectBtn);
        } else {
            drawNetBtn(hostBtn);
            drawNetBtn(joinBtn);
        }
    } else if (netState == NetState::HOST_WAITING) {
        sf::RectangleShape statusBg(sf::Vector2f(200, 90));
        statusBg.setPosition(730, 500);
        statusBg.setFillColor(sf::Color(50, 45, 35));
        statusBg.setOutlineColor(sf::Color(80, 70, 55));
        statusBg.setOutlineThickness(1);
        window.draw(statusBg);

        drawText(L"\u7b49\u5f85\u8fde\u63a5...", 830, 520, 16, sf::Color(200, 200, 150), true);
        drawText(L"\u672c\u673aIP:", 830, 546, 14, sf::Color(180, 180, 180), true);
        drawText(localIP, 830, 568, 16, sf::Color(100, 255, 100), true);

        auto drawNetBtn = [this](const UIButton& btn) {
            sf::RectangleShape rect(sf::Vector2f(btn.bounds.width, btn.bounds.height));
            rect.setPosition(btn.bounds.left, btn.bounds.top);
            rect.setFillColor(btn.hovered ? sf::Color(150, 60, 60) : sf::Color(120, 40, 40));
            rect.setOutlineColor(sf::Color(180, 80, 80));
            rect.setOutlineThickness(2);
            window.draw(rect);

            sf::Text t;
            t.setFont(font);
            t.setString(btn.label);
            t.setCharacterSize(18);
            t.setFillColor(sf::Color(255, 220, 220));
            sf::FloatRect bounds = t.getLocalBounds();
            t.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
            t.setPosition(btn.bounds.left + btn.bounds.width / 2.f,
                         btn.bounds.top + btn.bounds.height / 2.f);
            window.draw(t);
        };

        disconnectBtn.bounds.top = 600;
        drawNetBtn(disconnectBtn);
    } else if (netState == NetState::CONNECTED) {
        sf::RectangleShape statusBg(sf::Vector2f(200, 80));
        statusBg.setPosition(730, 500);
        statusBg.setFillColor(sf::Color(40, 60, 40));
        statusBg.setOutlineColor(sf::Color(80, 120, 80));
        statusBg.setOutlineThickness(1);
        window.draw(statusBg);

        drawText(L"\u5df2\u8fde\u63a5", 830, 520, 18, sf::Color(100, 255, 100), true);

        std::wstring sideText = (netSide == Side::RED) ? L"\u4f60\u662f\u7ea2\u65b9" : L"\u4f60\u662f\u9ed1\u65b9";
        sf::Color sideColor = (netSide == Side::RED) ? sf::Color(255, 120, 120) : sf::Color(200, 200, 200);
        drawText(sideText, 830, 555, 16, sideColor, true);

        auto drawNetBtn = [this](const UIButton& btn) {
            sf::RectangleShape rect(sf::Vector2f(btn.bounds.width, btn.bounds.height));
            rect.setPosition(btn.bounds.left, btn.bounds.top);
            rect.setFillColor(btn.hovered ? sf::Color(150, 60, 60) : sf::Color(120, 40, 40));
            rect.setOutlineColor(sf::Color(180, 80, 80));
            rect.setOutlineThickness(2);
            window.draw(rect);

            sf::Text t;
            t.setFont(font);
            t.setString(btn.label);
            t.setCharacterSize(18);
            t.setFillColor(sf::Color(255, 220, 220));
            sf::FloatRect bounds = t.getLocalBounds();
            t.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
            t.setPosition(btn.bounds.left + btn.bounds.width / 2.f,
                         btn.bounds.top + btn.bounds.height / 2.f);
            window.draw(t);
        };

        disconnectBtn.bounds.top = 590;
        drawNetBtn(disconnectBtn);
    }
}

void Game::drawTutorialPanel() {
    if (!fontLoaded) return;
    
    sf::RectangleShape bg(sf::Vector2f(440, 820));
    bg.setPosition(710, 0);
    bg.setFillColor(sf::Color(38, 28, 18, 250));
    bg.setOutlineColor(sf::Color(120, 100, 70));
    bg.setOutlineThickness(2);
    window.draw(bg);
    
    sf::RectangleShape header(sf::Vector2f(440, 36));
    header.setPosition(710, 0);
    header.setFillColor(sf::Color(55, 42, 30));
    window.draw(header);
    drawTextWithShadow(L"\u65b0\u624b\u6559\u7a0b", 930, 18, 20, sf::Color(255, 220, 150), true);
    
    sf::RectangleShape closeBtn(sf::Vector2f(50, 28));
    closeBtn.setPosition(1090, 4);
    closeBtn.setFillColor(sf::Color(180, 50, 50));
    closeBtn.setOutlineColor(sf::Color(220, 80, 80));
    closeBtn.setOutlineThickness(2);
    window.draw(closeBtn);
    drawTextWithShadow(L"\u2716", 1115, 18, 18, sf::Color::White, true);
    
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
        drawText(line, 730, y, isHeader ? 16 : 14, 
                 isHeader ? sf::Color(255, 200, 100) : sf::Color(210, 200, 180));
        y += isHeader ? 26 : 21;
    }
}

void Game::drawSurrenderPopup() {
    sf::RectangleShape overlay(sf::Vector2f(1150, 820));
    overlay.setPosition(0, 0);
    overlay.setFillColor(sf::Color(0, 0, 0, 160));
    window.draw(overlay);

    float panelW = 340;
    float panelH = 200;
    float panelX = (1150 - panelW) / 2;
    float panelY = 310;

    sf::RectangleShape panel(sf::Vector2f(panelW, panelH));
    panel.setPosition(panelX, panelY);
    panel.setFillColor(sf::Color(50, 40, 30));
    panel.setOutlineColor(sf::Color(120, 100, 70));
    panel.setOutlineThickness(3);
    window.draw(panel);

    float centerX = panelX + panelW / 2;

    drawTextWithShadow(L"\u8ba4\u8f93", centerX, panelY + 35, 26, sf::Color(255, 220, 150), true);
    drawText(L"\u8bf7\u9009\u62e9\u8ba4\u8f93\u65b9", centerX, panelY + 70, 14, sf::Color(200, 190, 170), true);

    float btnW = 130;
    float btnH = 44;
    float btnY = panelY + 95;
    float gap = 20;
    float btnLeftX = centerX - btnW - gap / 2;

    surrenderRedBtn.bounds = sf::FloatRect(btnLeftX, btnY, btnW, btnH);
    surrenderBlackBtn.bounds = sf::FloatRect(centerX + gap / 2, btnY, btnW, btnH);

    auto drawSurrenderOption = [this](const UIButton& btn, sf::Color fill, sf::Color outline) {
        sf::RectangleShape rect(sf::Vector2f(btn.bounds.width, btn.bounds.height));
        rect.setPosition(btn.bounds.left, btn.bounds.top);
        rect.setFillColor(btn.hovered ? sf::Color(fill.r + 50, fill.g + 30, fill.b + 20) : fill);
        rect.setOutlineColor(outline);
        rect.setOutlineThickness(3);
        window.draw(rect);

        sf::Text t;
        t.setFont(font);
        t.setString(btn.label);
        t.setCharacterSize(18);
        t.setFillColor(sf::Color(255, 255, 255));
        sf::FloatRect bounds = t.getLocalBounds();
        t.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
        t.setPosition(btn.bounds.left + btn.bounds.width / 2.f,
                     btn.bounds.top + btn.bounds.height / 2.f);
        window.draw(t);
    };

    drawSurrenderOption(surrenderRedBtn, sf::Color(180, 40, 40), sf::Color(240, 80, 80));
    drawSurrenderOption(surrenderBlackBtn, sf::Color(40, 40, 40), sf::Color(100, 100, 100));

    drawText(L"\u70b9\u51fb\u5916\u90e8\u53d6\u6d88", centerX, panelY + panelH + 18, 13, sf::Color(150, 140, 130), true);
}
