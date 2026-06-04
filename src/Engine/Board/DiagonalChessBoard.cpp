#include "Engine/Board/DiagonalChessBoard.h"
#include <cmath>
#include <cstring>

DiagonalChessBoard::DiagonalChessBoard() {
    std::memset(occupied, 0, sizeof(occupied));
    for (int r = 0; r < 9; r++)
        for (int c = 0; c < 9; c++)
            cells[r][c].SetAlive(false);
    PlacePieces();
}

void DiagonalChessBoard::Clear() {
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            cells[r][c] = DiagonalChessPiece(DChessPieceType::NONE, 0);
            cells[r][c].SetAlive(false);
            occupied[r][c] = false;
        }
    }
}

void DiagonalChessBoard::Reset() {
    Clear();
    PlacePieces();
}

void DiagonalChessBoard::PlacePieces() {
    auto put = [this](int r, int c, DChessPieceType t, int side) {
        cells[r][c] = DiagonalChessPiece(t, side);
        occupied[r][c] = true;
    };

    put(0, 8, DChessPieceType::GENERAL,  0);
    put(0, 7, DChessPieceType::ADVISOR,  0);
    put(1, 8, DChessPieceType::ADVISOR,  0);
    put(0, 6, DChessPieceType::ELEPHANT, 0);
    put(2, 8, DChessPieceType::ELEPHANT, 0);
    put(0, 5, DChessPieceType::HORSE,    0);
    put(3, 8, DChessPieceType::HORSE,    0);
    put(0, 4, DChessPieceType::CHARIOT,  0);
    put(4, 8, DChessPieceType::CHARIOT,  0);
    put(0, 3, DChessPieceType::CANNON,   0);
    put(5, 8, DChessPieceType::CANNON,   0);
    put(0, 2, DChessPieceType::SOLDIER,  0);
    put(2, 4, DChessPieceType::SOLDIER,  0);
    put(2, 6, DChessPieceType::SOLDIER,  0);
    put(4, 6, DChessPieceType::SOLDIER,  0);
    put(6, 8, DChessPieceType::SOLDIER,  0);

    put(8, 0, DChessPieceType::GENERAL,  1);
    put(8, 1, DChessPieceType::ADVISOR,  1);
    put(7, 0, DChessPieceType::ADVISOR,  1);
    put(8, 2, DChessPieceType::ELEPHANT, 1);
    put(6, 0, DChessPieceType::ELEPHANT, 1);
    put(8, 3, DChessPieceType::HORSE,    1);
    put(5, 0, DChessPieceType::HORSE,    1);
    put(8, 4, DChessPieceType::CHARIOT,  1);
    put(4, 0, DChessPieceType::CHARIOT,  1);
    put(8, 5, DChessPieceType::CANNON,   1);
    put(3, 0, DChessPieceType::CANNON,   1);
    put(8, 6, DChessPieceType::SOLDIER,  1);
    put(6, 4, DChessPieceType::SOLDIER,  1);
    put(6, 2, DChessPieceType::SOLDIER,  1);
    put(4, 2, DChessPieceType::SOLDIER,  1);
    put(2, 0, DChessPieceType::SOLDIER,  1);
}

Piece* DiagonalChessBoard::GetPiece(int r, int c) {
    return (r >= 0 && r < 9 && c >= 0 && c < 9 && occupied[r][c]) ? &cells[r][c] : nullptr;
}

const Piece* DiagonalChessBoard::GetPiece(int r, int c) const {
    return (r >= 0 && r < 9 && c >= 0 && c < 9 && occupied[r][c]) ? &cells[r][c] : nullptr;
}

void DiagonalChessBoard::SetPiece(int r, int c, std::unique_ptr<Piece> p) {
    if (!IsInside(r, c)) return;
    if (p) {
        auto* dcp = dynamic_cast<DiagonalChessPiece*>(p.get());
        if (dcp) {
            cells[r][c] = *dcp;
            occupied[r][c] = true;
        }
    } else {
        cells[r][c] = DiagonalChessPiece(DChessPieceType::NONE, 0);
        cells[r][c].SetAlive(false);
        occupied[r][c] = false;
    }
}

std::unique_ptr<Piece> DiagonalChessBoard::TakePiece(int r, int c) {
    if (!IsInside(r, c) || !occupied[r][c]) return nullptr;
    auto p = std::make_unique<DiagonalChessPiece>(cells[r][c]);
    cells[r][c] = DiagonalChessPiece(DChessPieceType::NONE, 0);
    cells[r][c].SetAlive(false);
    occupied[r][c] = false;
    return p;
}

void DiagonalChessBoard::ForEachPiece(std::function<void(Piece&, int r, int c)> fn) {
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (occupied[r][c]) {
                fn(cells[r][c], r, c);
            }
        }
    }
}

void DiagonalChessBoard::CopyTo(DiagonalChessBoard& target) const {
    target.Clear();
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            target.cells[r][c] = cells[r][c];
            target.occupied[r][c] = occupied[r][c];
        }
    }
}

void DiagonalChessBoard::MovePieceInternal(int fromR, int fromC, int toR, int toC) {
    if (!IsInside(fromR, fromC) || !IsInside(toR, toC)) return;
    cells[toR][toC] = cells[fromR][fromC];
    occupied[toR][toC] = true;
    cells[fromR][fromC] = DiagonalChessPiece(DChessPieceType::NONE, 0);
    cells[fromR][fromC].SetAlive(false);
    occupied[fromR][fromC] = false;
}

void DiagonalChessBoard::ClearCell(int r, int c) {
    if (!IsInside(r, c)) return;
    cells[r][c] = DiagonalChessPiece(DChessPieceType::NONE, 0);
    cells[r][c].SetAlive(false);
    occupied[r][c] = false;
}

void DiagonalChessBoard::OccupiedCell(int r, int c) {
    if (!IsInside(r, c)) return;
    occupied[r][c] = true;
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
