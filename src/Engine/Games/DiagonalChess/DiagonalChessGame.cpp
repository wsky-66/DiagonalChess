#include "Engine/Games/DiagonalChess/DiagonalChessGame.h"
#include "Engine/Piece.h"
#include <cmath>
#include <sstream>
#include <algorithm>

DiagonalChessGame::DiagonalChessGame()
    : window(sf::VideoMode(WIN_W, WIN_H), L"\u5BF9\u89D2\u8C61\u68CB",
             sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize)
    , currentTurn(0), selectedR(-1), selectedC(-1), pieceSelected(false)
    , gameOver(false), winner(Side::RED), isDrawGame(false), surrendered(false), agreedDraw(false)
    , gameOverTimer(0.f), movesWithoutCapture(0), fontLoaded(false)
    , aiMode(false), aiSide(1), aiThinking(false), aiDelayTimer(0.f), receivingMove(false)
{
    window.setFramerateLimit(60);
    view = window.getDefaultView();

    if (font.loadFromFile("C:/Windows/Fonts/simhei.ttf") ||
        font.loadFromFile("C:/Windows/Fonts/msyh.ttc") ||
        font.loadFromFile("C:/Windows/Fonts/simsun.ttc")) {
        fontLoaded = true;
    }
    ui.Init(window, font, fontLoaded);
    SetupNetworkCallbacks();
}

DiagonalChessGame::~DiagonalChessGame() { network.Disconnect(); }

void DiagonalChessGame::SetupNetworkCallbacks() {
    network.onMoveReceived = [this](int fr, int fc, int tr, int tc) {
        if (gameOver) return;
        if (network.undoRequestSent || network.restartRequestSent ||
            network.surrenderRequestSent || network.drawRequestSent) return;
        receivingMove = true;
        ExecuteMove(fr, fc, tr, tc);
        receivingMove = false;
    };

    network.onUndoAccepted = [this]() {
        if (gameOver || moveHistory.empty()) return;
        int steps = (moveHistory.top().side == static_cast<Side>(network.GetNetSide())) ? 1 : 2;
        ApplyUndoSteps(steps);
        network.SendUndoAck(steps);
    };

    network.onUndoAckReceived = [this](int steps) {
        if (gameOver) return;
        ApplyUndoSteps(steps);
    };

    network.onRestartAccepted = [this]() { if (!gameOver) RestartGame(); };
    network.onSurrenderAccepted = [this]() { if (!gameOver) DoSurrender(network.GetNetSide()); };
    network.onDrawAccepted = [this]() { if (!gameOver) DoDraw(); };

    network.onRejected = [this](const std::wstring& msg) { ui.SetNotification(msg, 3.f); };
    network.onDisconnected = [this]() { network.Disconnect(); ui.SetLogDividerY(440.f); };
}

void DiagonalChessGame::Update(float dt) {
    ProcessEvents();
    particles.Update(dt);
    ui.UpdateNotification(dt);
    if (gameOver) gameOverTimer += dt;
    network.PollNetwork();

    bool any = network.undoRequestSent || network.undoRequestReceived ||
               network.restartRequestSent || network.restartRequestReceived ||
               network.surrenderRequestSent || network.surrenderRequestReceived ||
               network.drawRequestSent || network.drawRequestReceived;

    ui.undoBtn.disabled = network.undoRequestSent || network.undoRequestReceived;
    ui.undoBtn.label = network.undoRequestSent ? L"\u7B49\u5F85\u540C\u610F..." : L"\u6094\u68CB";

    ui.restartBtn.disabled = network.restartRequestSent || network.restartRequestReceived;
    ui.restartBtn.label = network.restartRequestSent ? L"\u7B49\u5F85\u540C\u610F..." : L"\u91CD\u65B0\u5F00\u59CB";

    ui.surrenderBtn.disabled = network.surrenderRequestSent || network.surrenderRequestReceived ||
                                gameOver || network.GetState() == NetState::HOST_WAITING;
    ui.surrenderBtn.label = network.surrenderRequestSent ? L"\u7B49\u5F85\u540C\u610F..." : L"\u8BA4\u8F93";

    ui.drawOfferBtn.disabled = network.drawRequestSent || network.drawRequestReceived ||
                                gameOver || network.GetState() == NetState::HOST_WAITING;
    ui.drawOfferBtn.label = network.drawRequestSent ? L"\u7B49\u5F85\u540C\u610F..." : L"\u548C\u68CB";

    if (aiMode && !network.IsNetMode() && currentTurn == aiSide && !gameOver) {
        aiDelayTimer += dt; aiThinking = true;
        if (aiDelayTimer >= 1.5f) { DoAITurn(); aiThinking = false; aiDelayTimer = 0.f; }
    } else { aiThinking = false; aiDelayTimer = 0.f; }
}

void DiagonalChessGame::Render() {
    window.setView(view);
    window.clear(sf::Color(40, 32, 22));

    sf::RectangleShape lp(sf::Vector2f(700, WIN_H)); lp.setPosition(0, 0);
    lp.setFillColor(sf::Color(55, 42, 28)); window.draw(lp);
    sf::RectangleShape rp(sf::Vector2f(450, WIN_H)); rp.setPosition(700, 0);
    rp.setFillColor(sf::Color(45, 35, 24)); window.draw(rp);
    sf::RectangleShape dv(sf::Vector2f(2, WIN_H)); dv.setPosition(699, 0);
    dv.setFillColor(sf::Color(80, 65, 45)); window.draw(dv);

    ui.DrawBoard(board);
    ui.DrawPieces(board, selectedR, selectedC, validMoves);

    bool inCheck = rule.IsInCheckRaw(board, currentTurn);
    ui.DrawUI(currentTurn, gameOver, isDrawGame, surrendered, winner, inCheck, aiThinking);
    ui.DrawButtons();
    ui.DrawRequestPopups(network.undoRequestReceived, network.restartRequestReceived,
                         network.surrenderRequestReceived, network.drawRequestReceived);
    ui.DrawNotification();

    float dy = ui.GetLogDividerY();
    sf::RectangleShape ld(sf::Vector2f(420, 6)); ld.setPosition(715, dy - 3);
    ld.setFillColor(ui.IsDraggingLogDivider() ? sf::Color(150, 120, 80) : sf::Color(90, 70, 45));
    window.draw(ld);

    if (ui.IsShowTutorial()) ui.DrawTutorialPanel();
    ui.DrawNetUI(network.IsNetMode(), network.GetState(), network.GetLocalIP(),
                 ui.GetInputIP(), ui.IsShowIPInput(), (int)network.GetNetSide());
    ui.DrawMoveLog(moveLogStrings);
    if (ui.IsShowSurrenderPopup()) ui.DrawSurrenderPopup();

    if (gameOver) {
        bool cm = rule.IsCheckmate(board, (winner == Side::RED) ? 1 : 0);
        bool ins = rule.HasInsufficientMaterial(board);
        ui.DrawGameOverEffect(gameOverTimer, isDrawGame, winner, surrendered, cm, movesWithoutCapture, agreedDraw, ins);
    }

    particles.Draw(window);
    window.display();
}

void DiagonalChessGame::ProcessEvents() {
    sf::Event e;
    while (window.pollEvent(e)) {
        if (e.type == sf::Event::Closed) { window.close(); return; }

        if (e.type == sf::Event::Resized) {
            float ratio = (float)e.size.height / e.size.width;
            float base = (float)WIN_H / WIN_W;
            if (ratio > base) {
                float nh = e.size.width * base;
                view.setViewport(sf::FloatRect(0, (1.f - nh / e.size.height) / 2.f, 1.f, nh / e.size.height));
            } else {
                float nw = e.size.height / base;
                view.setViewport(sf::FloatRect((1.f - nw / e.size.width) / 2.f, 0, nw / e.size.width, 1.f));
            }
        }

        if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
            auto wp = window.mapPixelToCoords(sf::Vector2i(e.mouseButton.x, e.mouseButton.y), view);
            float mx = wp.x, my = wp.y, ly = ui.GetLogDividerY();

            if (mx >= 715 && mx <= 1135 && my >= ly - 8 && my <= ly + 8) {
                ui.SetDraggingLogDivider(true);
            } else if (gameOver && gameOverTimer > 0.5f && ui.gameOverRestartBtn.bounds.contains(mx, my)) {
                RestartGame();
            } else if (ui.IsShowSurrenderPopup() && !aiMode && network.GetState() != NetState::CONNECTED) {
                if (ui.surrenderRedBtn.bounds.contains(mx, my)) { DoSurrender(Side::RED); ui.SetShowSurrenderPopup(false); }
                else if (ui.surrenderBlackBtn.bounds.contains(mx, my)) { DoSurrender(Side::BLACK); ui.SetShowSurrenderPopup(false); }
                else if (mx < 405 || mx > 745 || my < 310 || my > 510) { ui.SetShowSurrenderPopup(false); }
            } else if (mx >= 700) {
                HandleButtonClick(mx, my);
            } else {
                auto g = board.ScreenToGrid(mx, my);
                if (g.x >= 0 && g.x < 9 && g.y >= 0 && g.y < 9) HandleBoardClick(g.x, g.y);
            }
        }

        if (e.type == sf::Event::MouseWheelScrolled) {
            auto wp = window.mapPixelToCoords(sf::Vector2i(e.mouseWheelScroll.x, e.mouseWheelScroll.y), view);
            if ((int)wp.x >= 715 && (int)wp.x <= 1135)
                ui.SetLogScrollOffset(ui.GetLogScrollOffset() - (int)e.mouseWheelScroll.delta);
        }

        if (e.type == sf::Event::MouseButtonReleased) { ui.SetDraggingLogDivider(false); }

        if (e.type == sf::Event::MouseMoved) {
            auto wp = window.mapPixelToCoords(sf::Vector2i(e.mouseMove.x, e.mouseMove.y), view);
            if (ui.IsDraggingLogDivider()) ui.SetLogDividerY(std::max(395.f, std::min(wp.y, 740.f)));
            ui.UpdateHover(wp.x, wp.y);
        }

        if (e.type == sf::Event::TextEntered && ui.IsShowIPInput()) {
            if (e.text.unicode == 8) ui.BackspaceInputIP();
            else if (e.text.unicode < 128 && e.text.unicode != 13) ui.AppendInputIP((wchar_t)e.text.unicode);
            else if (e.text.unicode == 13) {
                if (!ui.GetInputIP().empty()) { network.StartClient(ui.GetInputIP()); ui.SetShowIPInput(false); }
            }
        }
    }
}

void DiagonalChessGame::HandleBoardClick(int r, int c) {
    if (gameOver) return;
    if (aiMode && !network.IsNetMode() && currentTurn == aiSide) return;
    if (network.GetState() == NetState::CONNECTED && currentTurn != (int)network.GetNetSide()) return;
    if (network.undoRequestSent || network.undoRequestReceived || network.restartRequestSent ||
        network.restartRequestReceived || network.surrenderRequestSent || network.surrenderRequestReceived ||
        network.drawRequestSent || network.drawRequestReceived) return;

    if (!pieceSelected) {
        if (board.IsOccupied(r, c) && board.At(r, c).GetSide() == currentTurn) {
            selectedR = r; selectedC = c; pieceSelected = true;
            validMoves = rule.GetValidMoves(board, r, c);
        }
    } else {
        if (r == selectedR && c == selectedC) { pieceSelected = false; validMoves.clear(); return; }
        if (board.IsOccupied(r, c) && board.At(r, c).GetSide() == currentTurn) {
            selectedR = r; selectedC = c; validMoves = rule.GetValidMoves(board, r, c); return;
        }
        for (const auto& m : validMoves) {
            if (m.x == r && m.y == c) { ExecuteMove(selectedR, selectedC, r, c); break; }
        }
        pieceSelected = false; validMoves.clear();
    }
}

void DiagonalChessGame::HandleButtonClick(float mx, float my) {
    if (ui.IsShowTutorial() && mx >= 1090 && mx <= 1140 && my >= 4 && my <= 32) {
        ui.SetShowTutorial(false); audio.PlayClickSound(); return;
    }

    bool uR = network.undoRequestReceived, rR = network.restartRequestReceived;
    bool sR = network.surrenderRequestReceived, dR = network.drawRequestReceived;

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
        network.SendDrawResponse(true); network.drawRequestReceived = false; DoDraw(); audio.PlayClickSound();
    } else if (ui.drawRejectBtn.bounds.contains(mx, my) && dR) {
        network.SendDrawResponse(false); network.drawRequestReceived = false; audio.PlayClickSound();
    } else if (ui.restartAcceptBtn.bounds.contains(mx, my) && rR) {
        if (gameOver) { network.restartRequestReceived = false; return; }
        network.SendRestartAccept(); network.restartRequestReceived = false;
        RestartGame(); network.SendRestartAck(); audio.PlayClickSound();
    } else if (ui.restartRejectBtn.bounds.contains(mx, my) && rR) {
        network.SendRestartReject(); network.restartRequestReceived = false; audio.PlayClickSound();
    } else if (ui.undoBtn.bounds.contains(mx, my) && !ui.undoBtn.disabled) {
        if (network.GetState() == NetState::CONNECTED) {
            if (!network.undoRequestSent && moveHistory.size() >= 1) {
                network.SendUndoRequest(); network.undoRequestSent = true;
            }
        } else { UndoMove(); }
        audio.PlayClickSound();
    } else if (ui.restartBtn.bounds.contains(mx, my) && !ui.restartBtn.disabled) {
        if (network.GetState() == NetState::CONNECTED) {
            if (!network.restartRequestSent) { network.SendRestartRequest(); network.restartRequestSent = true; }
        } else { RestartGame(); }
        audio.PlayClickSound();
    } else if (ui.aiBtn.bounds.contains(mx, my) && !ui.aiBtn.disabled) {
        aiMode = !aiMode; ui.aiBtn.label = aiMode ? L"AI: \u5F00" : L"AI: \u5173"; audio.PlayClickSound();
    } else if (ui.difficultyBtn.bounds.contains(mx, my) && !ui.difficultyBtn.disabled) {
        auto d = ai.GetDifficulty();
        if (d == AIDifficulty::EASY) { ai.SetDifficulty(AIDifficulty::MEDIUM); ui.difficultyBtn.label = L"\u4E2D\u7B49"; }
        else if (d == AIDifficulty::MEDIUM) { ai.SetDifficulty(AIDifficulty::HARD); ui.difficultyBtn.label = L"\u56F0\u96BE"; }
        else { ai.SetDifficulty(AIDifficulty::EASY); ui.difficultyBtn.label = L"\u7B80\u5355"; }
        audio.PlayClickSound();
    } else if (ui.onlineBtn.bounds.contains(mx, my) && !ui.onlineBtn.disabled) {
        bool nm = !network.IsNetMode(); network.SetNetMode(nm);
        ui.onlineBtn.label = nm ? L"\u8054\u673A: \u5F00" : L"\u8054\u673A: \u5173";
        if (!nm) { network.Disconnect(); ui.SetShowIPInput(false); ui.SetLogDividerY(440.f); }
        else ui.SetLogDividerY(550.f);
        audio.PlayClickSound();
    } else if (ui.hostBtn.bounds.contains(mx, my) && network.IsNetMode() && network.GetState() == NetState::OFFLINE) {
        network.StartHost(); RestartGame(); audio.PlayClickSound();
    } else if (ui.joinBtn.bounds.contains(mx, my) && network.IsNetMode() && network.GetState() == NetState::OFFLINE) {
        ui.SetShowIPInput(!ui.IsShowIPInput()); audio.PlayClickSound();
    } else if (ui.connectBtn.bounds.contains(mx, my) && ui.IsShowIPInput() && !ui.GetInputIP().empty()) {
        network.StartClient(ui.GetInputIP());
        if (network.GetState() == NetState::CONNECTED) { aiMode = false; ui.aiBtn.label = L"AI: \u5173\u95ED"; Reset(); }
        ui.SetShowIPInput(false); audio.PlayClickSound();
    } else if (ui.disconnectBtn.bounds.contains(mx, my) && network.GetState() != NetState::OFFLINE) {
        network.Disconnect(); ui.SetLogDividerY(440.f); audio.PlayClickSound();
    } else if (ui.tutorialBtn.bounds.contains(mx, my) && !ui.tutorialBtn.disabled) {
        ui.SetShowTutorial(!ui.IsShowTutorial()); audio.PlayClickSound();
    } else if (ui.surrenderBtn.bounds.contains(mx, my) && !ui.surrenderBtn.disabled) {
        if (aiMode) DoSurrender(Side::RED);
        else if (network.GetState() == NetState::CONNECTED) {
            if (!network.surrenderRequestSent) { network.SendSurrenderRequest(); network.surrenderRequestSent = true; network.surrenderRequesterSide = network.GetNetSide(); }
        } else ui.SetShowSurrenderPopup(true);
        audio.PlayClickSound();
    } else if (ui.drawOfferBtn.bounds.contains(mx, my) && !ui.drawOfferBtn.disabled) {
        if (network.GetState() == NetState::CONNECTED) {
            if (network.drawRequestReceived) { network.SendDrawResponse(true); network.drawRequestReceived = false; DoDraw(); }
            else if (!network.drawRequestSent) { network.SendDrawRequest(); network.drawRequestSent = true; }
        } else DoDraw();
        audio.PlayClickSound();
    }
}

void DiagonalChessGame::ExecuteMove(int fr, int fc, int tr, int tc) {
    MoveRecord rec; rec.fromR = fr; rec.fromC = fc; rec.toR = tr; rec.toC = tc;
    rec.movedType = (int)board.At(fr, fc).GetDType();
    rec.movedSide = board.At(fr, fc).GetSide();
    rec.capturedType = board.IsOccupied(tr, tc) ? (int)board.At(tr, tc).GetDType() : 0;
    rec.capturedSide = board.IsOccupied(tr, tc) ? board.At(tr, tc).GetSide() : -1;
    rec.side = static_cast<Side>(currentTurn);
    rec.prevMovesWithoutCapture = movesWithoutCapture;

    bool captured = board.IsOccupied(tr, tc);
    if (captured) audio.PlayCaptureSound(); else audio.PlayMoveSound();

    if (captured && board.At(tr, tc).GetDType() == DChessPieceType::GENERAL) {
        gameOver = true; winner = static_cast<Side>(currentTurn); gameOverTimer = 0.f;
        particles.CreateWinParticles(static_cast<Side>(currentTurn));
        if (currentTurn == 0) audio.PlayWinSound(); else audio.PlayLoseSound();
    }

    board.MovePieceInternal(fr, fc, tr, tc);

    if (captured) movesWithoutCapture = 0;
    else movesWithoutCapture++;

    moveHistory.push(rec);

    std::wstring moveStr = GetMoveString(rec);
    moveLogStrings.push_back(moveStr);
    if ((int)moveLogStrings.size() > MAX_LOG * 2) {
        moveLogStrings.erase(moveLogStrings.begin());
    }

    if (network.GetState() == NetState::CONNECTED && !receivingMove) {
        network.SendMove(fr, fc, tr, tc);
    }

    currentTurn = (currentTurn == 0) ? 1 : 0;

    if (!gameOver) CheckGameEnd();
}

void DiagonalChessGame::UndoMove() {
    if (moveHistory.empty()) return;
    if (gameOver) { gameOver = false; isDrawGame = false; gameOverTimer = 0.f; particles.Clear(); }

    auto popOne = [this]() {
        auto rec = moveHistory.top(); moveHistory.pop();
        board.At(rec.fromR, rec.fromC) = DiagonalChessPiece((DChessPieceType)rec.movedType, rec.movedSide);
        board.OccupiedCell(rec.fromR, rec.fromC);
        if (rec.capturedSide >= 0) {
            board.At(rec.toR, rec.toC) = DiagonalChessPiece((DChessPieceType)rec.capturedType, rec.capturedSide);
            board.OccupiedCell(rec.toR, rec.toC);
        } else {
            board.ClearCell(rec.toR, rec.toC);
        }
        currentTurn = (int)rec.side;
        if (!moveLogStrings.empty()) moveLogStrings.pop_back();
    };

    if (aiMode) {
        if (moveHistory.size() < 2) return;
        popOne(); popOne();
        movesWithoutCapture = moveHistory.empty() ? 0 : moveHistory.top().prevMovesWithoutCapture;
        moveLogStrings.push_back(L"\u6094\u68CB");
    } else {
        auto rec = moveHistory.top();
        popOne();
        movesWithoutCapture = rec.prevMovesWithoutCapture;
        moveLogStrings.push_back(L"\u6094\u68CB");
    }
    pieceSelected = false; validMoves.clear(); ui.SetShowSurrenderPopup(false);
}

void DiagonalChessGame::RestartGame() {
    board.Reset();
    currentTurn = 0; pieceSelected = false; validMoves.clear();
    gameOver = false; isDrawGame = false; surrendered = false; agreedDraw = false;
    gameOverTimer = 0.f; movesWithoutCapture = 0;
    moveLogStrings.clear(); moveLogStrings.push_back(L"\u91CD\u65B0\u5F00\u59CB");
    while (!moveHistory.empty()) moveHistory.pop();
    particles.Clear(); network.ResetRequests(); ui.SetShowSurrenderPopup(false);
}

void DiagonalChessGame::DoSurrender(Side side) {
    ui.SetShowSurrenderPopup(false);
    gameOver = true; winner = (side == Side::RED) ? Side::BLACK : Side::RED;
    isDrawGame = false; surrendered = true; gameOverTimer = 0.f;
    moveLogStrings.push_back((side == Side::RED) ? L"\u7EA2\u65B9\u8BA4\u8F93" : L"\u9ED1\u65B9\u8BA4\u8F93");
    particles.CreateWinParticles(winner);
    if (winner == Side::RED) audio.PlayWinSound(); else audio.PlayLoseSound();
}

void DiagonalChessGame::DoDraw() {
    gameOver = true; isDrawGame = true; surrendered = false; agreedDraw = true; gameOverTimer = 0.f;
    moveLogStrings.push_back(L"\u548C\u68CB"); particles.CreateDrawParticles(); audio.PlayDrawSound();
}

void DiagonalChessGame::CheckGameEnd() {
    ui.SetShowSurrenderPopup(false);
    if (movesWithoutCapture >= DRAW_LIMIT) {
        gameOver = true; isDrawGame = true; gameOverTimer = 0.f;
        particles.CreateDrawParticles(); audio.PlayDrawSound(); return;
    }
    if (rule.HasInsufficientMaterial(board)) {
        gameOver = true; isDrawGame = true; gameOverTimer = 0.f;
        particles.CreateDrawParticles(); audio.PlayDrawSound(); return;
    }
    if (!rule.HasLegalMovesRaw(board, currentTurn)) {
        gameOver = true;
        if (rule.IsInCheckRaw(board, currentTurn)) {
            winner = (currentTurn == 0) ? Side::BLACK : Side::RED; isDrawGame = false;
        } else { isDrawGame = true; }
        gameOverTimer = 0.f;
        if (isDrawGame) { particles.CreateDrawParticles(); audio.PlayDrawSound(); }
        else { particles.CreateWinParticles(winner); if (winner == Side::RED) audio.PlayWinSound(); else audio.PlayLoseSound(); }
    }
}

void DiagonalChessGame::DoAITurn() {
    auto best = ai.Think(board, aiSide, rule);
    if (best.fromR < 0) return;
    ExecuteMove(best.fromR, best.fromC, best.toR, best.toC);
}

void DiagonalChessGame::ApplyUndoSteps(int steps) {
    if ((int)moveHistory.size() < steps) return;
    for (int i = 0; i < steps; i++) {
        auto rec = moveHistory.top(); moveHistory.pop();
        board.At(rec.fromR, rec.fromC) = DiagonalChessPiece((DChessPieceType)rec.movedType, rec.movedSide);
        board.OccupiedCell(rec.fromR, rec.fromC);
        if (rec.capturedSide >= 0) {
            board.At(rec.toR, rec.toC) = DiagonalChessPiece((DChessPieceType)rec.capturedType, rec.capturedSide);
            board.OccupiedCell(rec.toR, rec.toC);
        } else {
            board.ClearCell(rec.toR, rec.toC);
        }
        currentTurn = (int)rec.side;
        if (!moveLogStrings.empty()) moveLogStrings.pop_back();
    }
    if (!moveHistory.empty()) movesWithoutCapture = moveHistory.top().prevMovesWithoutCapture;
    else movesWithoutCapture = 0;
    pieceSelected = false; validMoves.clear();
    if (gameOver) { gameOver = false; isDrawGame = false; gameOverTimer = 0.f; particles.Clear(); }
}

std::wstring DiagonalChessGame::GetMoveString(const MoveRecord& rec) const {
    std::wostringstream oss;
    std::wstring sideStr = (rec.side == Side::RED) ? L"\u7EA2" : L"\u9ED1";
    auto sym = DChessGetSymbol((DChessPieceType)rec.movedType, rec.movedSide);
    oss << sideStr << sym << L" (" << rec.fromR << L"," << rec.fromC << L")\u2192("
        << rec.toR << L"," << rec.toC << L")";
    if (rec.capturedSide >= 0) {
        auto cs = DChessGetSymbol((DChessPieceType)rec.capturedType, rec.capturedSide);
        oss << L" \u5403" << cs;
    }
    return oss.str();
}

void DiagonalChessGame::Reset() { RestartGame(); }
