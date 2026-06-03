#include "Engine/Games/DiagonalChess/DiagonalChessGame.h"
#include <cmath>
#include <sstream>

DiagonalChessGame::DiagonalChessGame()
    : window(sf::VideoMode(WIN_W, WIN_H), L"\u5bf9\u89d2\u8c61\u68cb",
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
    , aiMode(false)
    , aiSide(Side::BLACK)
    , aiThinking(false)
    , aiDelayTimer(0.f)
    , receivingMove(false)
    , fontLoaded(false)
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

DiagonalChessGame::~DiagonalChessGame() {
    network.Disconnect();
}

void DiagonalChessGame::SetupNetworkCallbacks() {
    network.onMoveReceived = [this](int fromR, int fromC, int toR, int toC) {
        if (gameOver) return;
        if (network.undoRequestSent || network.restartRequestSent ||
            network.surrenderRequestSent || network.drawRequestSent) return;
        receivingMove = true;
        ExecuteMove(fromR, fromC, toR, toC);
        receivingMove = false;
    };

    network.onUndoRequestReceived = [this]() {};
    network.onRestartRequestReceived = [this]() {};

    network.onUndoAccepted = [this](int steps) {
        if (steps == 1) {
            int actualSteps = (moveHistory.top().side == network.GetNetSide()) ? 1 : 2;
            ApplyUndoSteps(actualSteps);
            network.SendUndoAck(actualSteps);
        } else {
            ApplyUndoSteps(steps);
        }
    };

    network.onRestartAccepted = [this]() { RestartGame(); };
    network.onSurrenderAccepted = [this]() { DoSurrender(network.GetNetSide()); };
    network.onDrawAccepted = [this]() { DoDraw(); };

    network.onRejected = [this](const std::wstring& msg) {
        ui.SetNotification(msg, 3.f);
    };

    network.onDisconnected = [this]() {
        network.Disconnect();
        ui.SetLogDividerY(440.f);
    };
}

void DiagonalChessGame::Update(float dt) {
    ProcessEvents();
    particles.Update(dt);
    ui.UpdateNotification(dt);

    if (gameOver) {
        gameOverTimer += dt;
    }

    network.PollNetwork();

    bool undoReqActive = network.undoRequestSent || network.undoRequestReceived;
    bool restartReqActive = network.restartRequestSent || network.restartRequestReceived;
    bool surrenderReqActive = network.surrenderRequestSent || network.surrenderRequestReceived;
    bool drawReqActive = network.drawRequestSent || network.drawRequestReceived;
    bool anyReqActive = undoReqActive || restartReqActive || surrenderReqActive || drawReqActive;

    if (network.undoRequestSent) {
        ui.undoBtn.label = L"\u7b49\u5f85\u540c\u610f...";
        ui.undoBtn.disabled = true;
    } else if (network.undoRequestReceived) {
        ui.undoBtn.disabled = true;
    } else {
        ui.undoBtn.label = L"\u6094\u68cb";
        ui.undoBtn.disabled = false;
    }

    if (network.restartRequestSent) {
        ui.restartBtn.label = L"\u7b49\u5f85\u540c\u610f...";
        ui.restartBtn.disabled = true;
    } else if (network.restartRequestReceived) {
        ui.restartBtn.disabled = true;
    } else {
        ui.restartBtn.label = L"\u91cd\u65b0\u5f00\u59cb";
        ui.restartBtn.disabled = false;
    }

    if (network.surrenderRequestSent) {
        ui.surrenderBtn.label = L"\u7b49\u5f85\u540c\u610f...";
        ui.surrenderBtn.disabled = true;
    } else if (network.surrenderRequestReceived) {
        ui.surrenderBtn.disabled = true;
    } else if (gameOver || network.GetState() == NetState::HOST_WAITING) {
        ui.surrenderBtn.label = L"\u8ba4\u8f93";
        ui.surrenderBtn.disabled = true;
    } else {
        ui.surrenderBtn.label = L"\u8ba4\u8f93";
        ui.surrenderBtn.disabled = false;
    }

    if (network.drawRequestSent) {
        ui.drawOfferBtn.label = L"\u7b49\u5f85\u540c\u610f...";
        ui.drawOfferBtn.disabled = true;
    } else if (network.drawRequestReceived) {
        ui.drawOfferBtn.disabled = true;
    } else if (gameOver || network.GetState() == NetState::HOST_WAITING) {
        ui.drawOfferBtn.label = L"\u548c\u68cb";
        ui.drawOfferBtn.disabled = true;
    } else {
        ui.drawOfferBtn.label = L"\u548c\u68cb";
        ui.drawOfferBtn.disabled = false;
    }

    if (aiMode && !network.IsNetMode() && currentTurn == aiSide && !gameOver) {
        aiDelayTimer += dt;
        aiThinking = true;
        if (aiDelayTimer >= 1.5f) {
            DoAITurn();
            aiThinking = false;
            aiDelayTimer = 0.f;
        }
    } else {
        aiThinking = false;
        aiDelayTimer = 0.f;
    }
}

void DiagonalChessGame::Render() {
    window.setView(view);
    sf::Color bgColor(40, 32, 22);
    window.clear(bgColor);

    sf::RectangleShape leftPanel(sf::Vector2f(700, WIN_H));
    leftPanel.setPosition(0, 0);
    leftPanel.setFillColor(sf::Color(55, 42, 28));
    window.draw(leftPanel);

    sf::RectangleShape rightPanel(sf::Vector2f(450, WIN_H));
    rightPanel.setPosition(700, 0);
    rightPanel.setFillColor(sf::Color(45, 35, 24));
    window.draw(rightPanel);

    sf::RectangleShape divider(sf::Vector2f(2, WIN_H));
    divider.setPosition(699, 0);
    divider.setFillColor(sf::Color(80, 65, 45));
    window.draw(divider);

    ui.DrawBoard(board);
    ui.DrawPieces(board, selectedR, selectedC, validMoves);
    ui.DrawUI(currentTurn, gameOver, isDrawGame, surrendered, winner,
              rule.IsInCheck(board.GetBoard(), currentTurn), aiThinking);
    ui.DrawButtons();
    ui.DrawRequestPopups(network.undoRequestReceived, network.restartRequestReceived,
                         network.surrenderRequestReceived, network.drawRequestReceived);
    ui.DrawNotification();

    float dy = ui.GetLogDividerY();
    sf::RectangleShape logDivider(sf::Vector2f(420, 6));
    logDivider.setPosition(715, dy - 3);
    logDivider.setFillColor(ui.IsDraggingLogDivider() ? sf::Color(150, 120, 80) : sf::Color(90, 70, 45));
    window.draw(logDivider);

    if (ui.IsShowTutorial()) ui.DrawTutorialPanel();
    ui.DrawNetUI(network.IsNetMode(), network.GetState(), network.GetLocalIP(),
                 ui.GetInputIP(), ui.IsShowIPInput(), network.GetNetSide());
    ui.DrawMoveLog(moveLogStrings);

    if (ui.IsShowSurrenderPopup()) ui.DrawSurrenderPopup();

    if (gameOver) {
        bool loserInCheck = rule.IsCheckmate(board.GetBoard(),
            (winner == Side::RED) ? Side::BLACK : Side::RED);
        ui.DrawGameOverEffect(gameOverTimer, isDrawGame, winner, surrendered,
                               loserInCheck, movesWithoutCapture, agreedDraw,
                               rule.HasInsufficientMaterial(board.GetBoard()));
    }

    particles.Draw(window);
    window.display();
}

void DiagonalChessGame::ProcessEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();

        if (event.type == sf::Event::Resized) {
            float ratio = (float)event.size.height / event.size.width;
            float baseRatio = (float)WIN_H / WIN_W;
            if (ratio > baseRatio) {
                float nh = event.size.width * baseRatio;
                view.setViewport(sf::FloatRect(0, (1.f - nh / event.size.height) / 2.f, 1.f, nh / event.size.height));
            } else {
                float nw = event.size.height / baseRatio;
                view.setViewport(sf::FloatRect((1.f - nw / event.size.width) / 2.f, 0, nw / event.size.width, 1.f));
            }
        }

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f worldPos = window.mapPixelToCoords(
                sf::Vector2i(event.mouseButton.x, event.mouseButton.y), view);
            float mx = worldPos.x;
            float my = worldPos.y;

            float ly = ui.GetLogDividerY();
            if (mx >= 715 && mx <= 1135 && my >= ly - 8 && my <= ly + 8) {
                ui.SetDraggingLogDivider(true);
            } else if (gameOver && gameOverTimer > 0.5f &&
                       ui.gameOverRestartBtn.bounds.contains(mx, my)) {
                RestartGame();
            } else if (ui.IsShowSurrenderPopup() && !aiMode &&
                       network.GetState() != NetState::CONNECTED) {
                if (ui.surrenderRedBtn.bounds.contains(mx, my)) {
                    DoSurrender(Side::RED);
                    ui.SetShowSurrenderPopup(false);
                } else if (ui.surrenderBlackBtn.bounds.contains(mx, my)) {
                    DoSurrender(Side::BLACK);
                    ui.SetShowSurrenderPopup(false);
                } else if (mx < 405 || mx > 745 || my < 310 || my > 510) {
                    ui.SetShowSurrenderPopup(false);
                }
            } else if (mx >= 700) {
                HandleButtonClick(mx, my);
            } else {
                sf::Vector2i gridPos = board.ScreenToGrid(mx, my);
                if (gridPos.x >= 0 && gridPos.x < 9 && gridPos.y >= 0 && gridPos.y < 9) {
                    HandleBoardClick(gridPos.x, gridPos.y);
                }
            }
        }

        if (event.type == sf::Event::MouseWheelScrolled) {
            sf::Vector2f worldPos = window.mapPixelToCoords(
                sf::Vector2i(event.mouseWheelScroll.x, event.mouseWheelScroll.y), view);
            int x = static_cast<int>(worldPos.x);
            if (x >= 715 && x <= 1135) {
                ui.SetLogScrollOffset(ui.GetLogScrollOffset() - (int)event.mouseWheelScroll.delta);
            }
        }

        if (event.type == sf::Event::MouseButtonReleased) {
            ui.SetDraggingLogDivider(false);
        }

        if (event.type == sf::Event::MouseMoved) {
            sf::Vector2f worldPos = window.mapPixelToCoords(
                sf::Vector2i(event.mouseMove.x, event.mouseMove.y), view);
            float mx = worldPos.x;
            float my = worldPos.y;

            if (ui.IsDraggingLogDivider()) {
                ui.SetLogDividerY(std::max(395.f, std::min(my, 740.f)));
            }
            ui.UpdateHover(mx, my);
        }

        if (event.type == sf::Event::TextEntered && ui.IsShowIPInput()) {
            if (event.text.unicode == 8) {
                ui.BackspaceInputIP();
            } else if (event.text.unicode < 128 && event.text.unicode != 13) {
                ui.AppendInputIP(static_cast<wchar_t>(event.text.unicode));
            } else if (event.text.unicode == 13) {
                if (!ui.GetInputIP().empty()) {
                    network.StartClient(ui.GetInputIP());
                    ui.SetShowIPInput(false);
                }
            }
        }
    }
}

void DiagonalChessGame::HandleBoardClick(int r, int c) {
    if (gameOver) return;
    if (aiMode && !network.IsNetMode() && currentTurn == aiSide) return;
    if (network.GetState() == NetState::CONNECTED && currentTurn != network.GetNetSide()) return;
    if (network.undoRequestSent || network.undoRequestReceived ||
        network.restartRequestSent || network.restartRequestReceived ||
        network.surrenderRequestSent || network.surrenderRequestReceived ||
        network.drawRequestSent || network.drawRequestReceived) return;

    if (!pieceSelected) {
        const auto& b = board.GetBoard();
        if (b[r][c].alive && b[r][c].side == currentTurn) {
            selectedR = r;
            selectedC = c;
            pieceSelected = true;
            validMoves = rule.GetValidMoves(board.GetBoard(), r, c);
        }
    } else {
        if (r == selectedR && c == selectedC) {
            pieceSelected = false;
            validMoves.clear();
            return;
        }

        const auto& b = board.GetBoard();
        if (b[r][c].alive && b[r][c].side == currentTurn) {
            selectedR = r;
            selectedC = c;
            validMoves = rule.GetValidMoves(board.GetBoard(), r, c);
            return;
        }

        bool isValid = false;
        for (const auto& move : validMoves) {
            if (move.x == r && move.y == c) { isValid = true; break; }
        }

        if (isValid) {
            ExecuteMove(selectedR, selectedC, r, c);
        }
        pieceSelected = false;
        validMoves.clear();
    }
}

void DiagonalChessGame::HandleButtonClick(float mx, float my) {
    if (ui.IsShowTutorial() && mx >= 1090 && mx <= 1140 && my >= 4 && my <= 32) {
        ui.SetShowTutorial(false);
        audio.PlayClickSound();
        return;
    }

    bool undoRecv = network.undoRequestReceived;
    bool restartRecv = network.restartRequestReceived;
    bool surrRecv = network.surrenderRequestReceived;
    bool drawRecv = network.drawRequestReceived;

    if (ui.undoAcceptBtn.bounds.contains(mx, my) && undoRecv) {
        if (gameOver) { network.undoRequestReceived = false; return; }
        network.SendUndoAccept();
        network.undoRequestReceived = false;
        audio.PlayClickSound();
        return;
    }
    if (ui.undoRejectBtn.bounds.contains(mx, my) && undoRecv) {
        network.SendUndoReject();
        network.undoRequestReceived = false;
        audio.PlayClickSound();
        return;
    }
    if (ui.surrenderAcceptBtn.bounds.contains(mx, my) && surrRecv) {
        if (gameOver) { network.surrenderRequestReceived = false; return; }
        network.SendSurrenderResponse(true);
        network.surrenderRequestReceived = false;
        DoSurrender(network.surrenderRequesterSide);
        audio.PlayClickSound();
        return;
    }
    if (ui.surrenderRejectBtn.bounds.contains(mx, my) && surrRecv) {
        network.SendSurrenderResponse(false);
        network.surrenderRequestReceived = false;
        audio.PlayClickSound();
        return;
    }
    if (ui.drawAcceptBtn.bounds.contains(mx, my) && drawRecv) {
        if (gameOver) { network.drawRequestReceived = false; return; }
        network.SendDrawResponse(true);
        network.drawRequestReceived = false;
        DoDraw();
        audio.PlayClickSound();
        return;
    }
    if (ui.drawRejectBtn.bounds.contains(mx, my) && drawRecv) {
        network.SendDrawResponse(false);
        network.drawRequestReceived = false;
        audio.PlayClickSound();
        return;
    }
    if (ui.restartAcceptBtn.bounds.contains(mx, my) && restartRecv) {
        if (gameOver) { network.restartRequestReceived = false; return; }
        network.SendRestartAccept();
        network.restartRequestReceived = false;
        RestartGame();
        network.SendRestartAck();
        audio.PlayClickSound();
        return;
    }
    if (ui.restartRejectBtn.bounds.contains(mx, my) && restartRecv) {
        network.SendRestartReject();
        network.restartRequestReceived = false;
        audio.PlayClickSound();
        return;
    }
    if (ui.undoBtn.bounds.contains(mx, my) && !ui.undoBtn.disabled) {
        if (network.GetState() == NetState::CONNECTED) {
            if (!network.undoRequestSent && moveHistory.size() >= 1) {
                network.undoRequesterSide = network.GetNetSide();
                network.SendUndoRequest();
                network.undoRequestSent = true;
            }
        } else {
            UndoMove();
        }
        audio.PlayClickSound();
    } else if (ui.restartBtn.bounds.contains(mx, my) && !ui.restartBtn.disabled) {
        if (network.GetState() == NetState::CONNECTED) {
            if (!network.restartRequestSent) {
                network.SendRestartRequest();
                network.restartRequestSent = true;
            }
        } else {
            RestartGame();
        }
        audio.PlayClickSound();
    } else if (ui.aiBtn.bounds.contains(mx, my) && !ui.aiBtn.disabled) {
        aiMode = !aiMode;
        ui.aiBtn.label = aiMode ? L"AI: \u5f00" : L"AI: \u5173";
        audio.PlayClickSound();
    } else if (ui.difficultyBtn.bounds.contains(mx, my) && !ui.difficultyBtn.disabled) {
        AIDifficulty d = ai.GetDifficulty();
        if (d == AIDifficulty::EASY) {
            ai.SetDifficulty(AIDifficulty::MEDIUM);
            ui.difficultyBtn.label = L"\u4e2d\u7b49";
        } else if (d == AIDifficulty::MEDIUM) {
            ai.SetDifficulty(AIDifficulty::HARD);
            ui.difficultyBtn.label = L"\u56f0\u96be";
        } else {
            ai.SetDifficulty(AIDifficulty::EASY);
            ui.difficultyBtn.label = L"\u7b80\u5355";
        }
        audio.PlayClickSound();
    } else if (ui.onlineBtn.bounds.contains(mx, my) && !ui.onlineBtn.disabled) {
        bool netMode = network.IsNetMode();
        netMode = !netMode;
        network.SetNetMode(netMode);
        ui.onlineBtn.label = netMode ? L"\u8054\u673a: \u5f00" : L"\u8054\u673a: \u5173";
        if (!netMode) {
            network.Disconnect();
            ui.SetShowIPInput(false);
            ui.SetLogDividerY(440.f);
        } else {
            ui.SetLogDividerY(550.f);
        }
        audio.PlayClickSound();
    } else if (ui.hostBtn.bounds.contains(mx, my) && network.IsNetMode() && network.GetState() == NetState::OFFLINE) {
        network.StartHost();
        RestartGame();
        audio.PlayClickSound();
    } else if (ui.joinBtn.bounds.contains(mx, my) && network.IsNetMode() && network.GetState() == NetState::OFFLINE) {
        ui.SetShowIPInput(!ui.IsShowIPInput());
        audio.PlayClickSound();
    } else if (ui.connectBtn.bounds.contains(mx, my) && ui.IsShowIPInput() && !ui.GetInputIP().empty()) {
        network.StartClient(ui.GetInputIP());
        if (network.GetState() == NetState::CONNECTED) {
            aiMode = false;
            ui.aiBtn.label = L"AI: \u5173\u95ed";
            Reset();
        }
        ui.SetShowIPInput(false);
        audio.PlayClickSound();
    } else if (ui.disconnectBtn.bounds.contains(mx, my) && network.GetState() != NetState::OFFLINE) {
        network.Disconnect();
        ui.SetLogDividerY(440.f);
        audio.PlayClickSound();
    } else if (ui.tutorialBtn.bounds.contains(mx, my) && !ui.tutorialBtn.disabled) {
        ui.SetShowTutorial(!ui.IsShowTutorial());
        audio.PlayClickSound();
    } else if (ui.surrenderBtn.bounds.contains(mx, my) && !ui.surrenderBtn.disabled) {
        if (aiMode) {
            DoSurrender(Side::RED);
        } else if (network.GetState() == NetState::CONNECTED) {
            if (!network.surrenderRequestSent) {
                network.SendSurrenderRequest();
                network.surrenderRequestSent = true;
                network.surrenderRequesterSide = network.GetNetSide();
            }
        } else {
            ui.SetShowSurrenderPopup(true);
        }
        audio.PlayClickSound();
    } else if (ui.drawOfferBtn.bounds.contains(mx, my) && !ui.drawOfferBtn.disabled) {
        if (network.GetState() == NetState::CONNECTED) {
            if (network.drawRequestReceived) {
                network.SendDrawResponse(true);
                network.drawRequestReceived = false;
                DoDraw();
            } else if (!network.drawRequestSent) {
                network.SendDrawRequest();
                network.drawRequestSent = true;
            }
        } else {
            DoDraw();
        }
        audio.PlayClickSound();
    }
}

void DiagonalChessGame::ExecuteMove(int fromR, int fromC, int toR, int toC) {
    MoveRecord record;
    record.fromR = fromR; record.fromC = fromC;
    record.toR = toR; record.toC = toC;
    record.movedPiece = board.GetBoard()[fromR][fromC];
    record.capturedPiece = board.GetBoard()[toR][toC];
    record.side = currentTurn;
    record.prevMovesWithoutCapture = movesWithoutCapture;

    bool captured = board.GetBoard()[toR][toC].alive;

    if (captured) audio.PlayCaptureSound();
    else audio.PlayMoveSound();

    if (captured && board.GetBoard()[toR][toC].type == PieceType::GENERAL) {
        gameOver = true;
        winner = currentTurn;
        gameOverTimer = 0.f;
        particles.CreateWinParticles(winner);
        if (winner == Side::RED) audio.PlayWinSound();
        else audio.PlayLoseSound();
    }

    board.GetBoard()[toR][toC] = board.GetBoard()[fromR][fromC];
    board.GetBoard()[fromR][fromC] = {PieceType::NONE, Side::RED, false};

    if (captured) movesWithoutCapture = 0;
    else movesWithoutCapture++;

    moveHistory.push(record);
    std::wstring moveStr = GetMoveString(record);
    moveLogStrings.push_back(moveStr);
    if ((int)moveLogStrings.size() > MAX_LOG * 2) {
        moveLogStrings.erase(moveLogStrings.begin());
    }

    if (network.GetState() == NetState::CONNECTED && !receivingMove) {
        network.SendMove(fromR, fromC, toR, toC);
    }

    currentTurn = (currentTurn == Side::RED) ? Side::BLACK : Side::RED;

    if (!gameOver) CheckGameEnd();
}

void DiagonalChessGame::UndoMove() {
    if (moveHistory.empty()) return;
    if (gameOver) {
        gameOver = false; isDrawGame = false; gameOverTimer = 0.f; particles.Clear();
    }

    if (aiMode) {
        if (moveHistory.size() < 2) return;
        for (int i = 0; i < 2; i++) {
            MoveRecord record = moveHistory.top(); moveHistory.pop();
            board.GetBoard()[record.fromR][record.fromC] = record.movedPiece;
            board.GetBoard()[record.toR][record.toC] = record.capturedPiece;
            currentTurn = record.side;
            if (!moveLogStrings.empty()) moveLogStrings.pop_back();
        }
        movesWithoutCapture = moveHistory.top().prevMovesWithoutCapture;
        moveLogStrings.push_back(L"\u6094\u68cb");
    } else {
        MoveRecord record = moveHistory.top(); moveHistory.pop();
        board.GetBoard()[record.fromR][record.fromC] = record.movedPiece;
        board.GetBoard()[record.toR][record.toC] = record.capturedPiece;
        currentTurn = record.side;
        movesWithoutCapture = record.prevMovesWithoutCapture;
        if (!moveLogStrings.empty()) moveLogStrings.pop_back();
        moveLogStrings.push_back(L"\u6094\u68cb");
    }

    pieceSelected = false; validMoves.clear(); ui.SetShowSurrenderPopup(false);
}

void DiagonalChessGame::RestartGame() {
    board.Reset();
    currentTurn = Side::RED;
    moveLogStrings.clear();
    moveLogStrings.push_back(L"\u91cd\u65b0\u5f00\u59cb");
    pieceSelected = false; validMoves.clear();
    gameOver = false; isDrawGame = false; surrendered = false; agreedDraw = false;
    gameOverTimer = 0.f; movesWithoutCapture = 0;
    while (!moveHistory.empty()) moveHistory.pop();
    particles.Clear();
    network.ResetRequests();
    ui.SetShowSurrenderPopup(false);
}

void DiagonalChessGame::DoSurrender(Side side) {
    ui.SetShowSurrenderPopup(false);
    gameOver = true;
    winner = (side == Side::RED) ? Side::BLACK : Side::RED;
    isDrawGame = false; surrendered = true; gameOverTimer = 0.f;
    moveLogStrings.push_back((side == Side::RED) ? L"\u7ea2\u65b9\u8ba4\u8f93" : L"\u9ed1\u65b9\u8ba4\u8f93");
    particles.CreateWinParticles(winner);
    if (winner == Side::RED) audio.PlayWinSound();
    else audio.PlayLoseSound();
}

void DiagonalChessGame::DoDraw() {
    gameOver = true; isDrawGame = true; surrendered = false; agreedDraw = true; gameOverTimer = 0.f;
    moveLogStrings.push_back(L"\u548c\u68cb");
    particles.CreateDrawParticles();
    audio.PlayDrawSound();
}

void DiagonalChessGame::CheckGameEnd() {
    ui.SetShowSurrenderPopup(false);
    if (movesWithoutCapture >= DRAW_LIMIT) {
        gameOver = true; isDrawGame = true; gameOverTimer = 0.f;
        particles.CreateDrawParticles(); audio.PlayDrawSound();
        return;
    }

    if (rule.HasInsufficientMaterial(board.GetBoard())) {
        gameOver = true; isDrawGame = true; gameOverTimer = 0.f;
        particles.CreateDrawParticles(); audio.PlayDrawSound();
        return;
    }

    if (!rule.HasLegalMoves(board.GetBoard(), currentTurn)) {
        gameOver = true;
        if (rule.IsInCheck(board.GetBoard(), currentTurn)) {
            winner = (currentTurn == Side::RED) ? Side::BLACK : Side::RED;
            isDrawGame = false;
        } else {
            isDrawGame = true;
        }
        gameOverTimer = 0.f;
        if (isDrawGame) {
            particles.CreateDrawParticles(); audio.PlayDrawSound();
        } else {
            particles.CreateWinParticles(winner);
            if (winner == Side::RED) audio.PlayWinSound();
            else audio.PlayLoseSound();
        }
    }
}

void DiagonalChessGame::DoAITurn() {
    AIMove best = ai.Think(board.GetBoard(), aiSide, rule);
    if (best.fromR < 0) return;
    ExecuteMove(best.fromR, best.fromC, best.toR, best.toC);
}

void DiagonalChessGame::ApplyUndoSteps(int steps) {
    if ((int)moveHistory.size() < steps) return;
    for (int i = 0; i < steps; i++) {
        MoveRecord record = moveHistory.top(); moveHistory.pop();
        board.GetBoard()[record.fromR][record.fromC] = record.movedPiece;
        board.GetBoard()[record.toR][record.toC] = record.capturedPiece;
        currentTurn = record.side;
        if (!moveLogStrings.empty()) moveLogStrings.pop_back();
    }
    if (!moveHistory.empty()) {
        movesWithoutCapture = moveHistory.top().prevMovesWithoutCapture;
    } else {
        movesWithoutCapture = 0;
    }
    pieceSelected = false; validMoves.clear();
    if (gameOver) { gameOver = false; isDrawGame = false; gameOverTimer = 0.f; particles.Clear(); }
}

std::wstring DiagonalChessGame::GetMoveString(const MoveRecord& move) const {
    std::wostringstream oss;
    std::wstring sideStr = (move.side == Side::RED) ? L"\u7ea2" : L"\u9ed1";
    std::wstring pieceName = GetPieceName(move.movedPiece.type, move.side);
    oss << sideStr << pieceName
        << L" (" << move.fromR << L"," << move.fromC << L")\u2192("
        << move.toR << L"," << move.toC << L")";
    if (move.capturedPiece.alive) {
        oss << L" \u5403" << GetPieceName(move.capturedPiece.type, move.capturedPiece.side);
    }
    return oss.str();
}

void DiagonalChessGame::Reset() {
    RestartGame();
}
