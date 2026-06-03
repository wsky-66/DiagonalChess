#include "Engine/Board/DiagonalChessBoard.h"
#include <cmath>

DiagonalChessBoard::DiagonalChessBoard() {
    PlacePieces();
}

void DiagonalChessBoard::InitBoard() {
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            board[r][c] = {PieceType::NONE, Side::RED, false};
        }
    }
}

void DiagonalChessBoard::PlacePieces() {
    InitBoard();

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
}

void DiagonalChessBoard::Reset() {
    PlacePieces();
}

sf::Vector2f DiagonalChessBoard::GridToScreen(int r, int c) const {
    float x = ORIGIN_X + (r + c) * DIAG;
    float y = ORIGIN_Y + (c - r) * DIAG;
    return sf::Vector2f(x, y);
}

sf::Vector2i DiagonalChessBoard::ScreenToGrid(float sx, float sy) const {
    float dx = sx - ORIGIN_X;
    float dy = sy - ORIGIN_Y;
    float rf = (dx / DIAG - dy / DIAG) / 2.f;
    float cf = (dx / DIAG + dy / DIAG) / 2.f;
    int r = static_cast<int>(std::round(rf));
    int c = static_cast<int>(std::round(cf));
    return sf::Vector2i(r, c);
}
