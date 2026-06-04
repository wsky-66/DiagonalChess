#include "Engine/Board/GomokuBoard.h"
#include <cmath>
#include <cstring>

GomokuBoard::GomokuBoard() {
    std::memset(occupied, 0, sizeof(occupied));
    std::memset(cells, 0, sizeof(cells));
    // 初始化 pieces（空棋子 side=0）
    for (int r = 0; r < SIZE; r++)
        for (int c = 0; c < SIZE; c++)
            pieces[r][c] = GomokuPiece(0);
}

void GomokuBoard::Clear() {
    std::memset(occupied, 0, sizeof(occupied));
    std::memset(cells, 0, sizeof(cells));
    for (int r = 0; r < SIZE; r++)
        for (int c = 0; c < SIZE; c++)
            pieces[r][c] = GomokuPiece(0);
}

void GomokuBoard::Reset() {
    Clear();
}

Piece* GomokuBoard::GetPiece(int r, int c) {
    if (!IsInside(r, c) || !occupied[r][c]) return nullptr;
    return &pieces[r][c];
}

const Piece* GomokuBoard::GetPiece(int r, int c) const {
    if (!IsInside(r, c) || !occupied[r][c]) return nullptr;
    return &pieces[r][c];
}

void GomokuBoard::SetPiece(int r, int c, std::unique_ptr<Piece> p) {
    if (!IsInside(r, c)) return;
    if (p) {
        auto* gp = dynamic_cast<GomokuPiece*>(p.get());
        if (gp) {
            pieces[r][c] = *gp;
            cells[r][c] = gp->GetSide();
            occupied[r][c] = true;
        }
    } else {
        pieces[r][c] = GomokuPiece(0);
        cells[r][c] = 0;
        occupied[r][c] = false;
    }
}

std::unique_ptr<Piece> GomokuBoard::TakePiece(int r, int c) {
    if (!IsInside(r, c) || !occupied[r][c]) return nullptr;
    auto p = std::make_unique<GomokuPiece>(pieces[r][c]);
    pieces[r][c] = GomokuPiece(0);
    cells[r][c] = 0;
    occupied[r][c] = false;
    return p;
}

void GomokuBoard::ForEachPiece(std::function<void(Piece&, int r, int c)> fn) {
    for (int r = 0; r < SIZE; r++)
        for (int c = 0; c < SIZE; c++)
            if (occupied[r][c])
                fn(pieces[r][c], r, c);
}

void GomokuBoard::PlaceStone(int r, int c, int side) {
    if (!IsInside(r, c) || side == 0) return;
    cells[r][c] = side;
    pieces[r][c] = GomokuPiece(side);
    occupied[r][c] = true;
}

void GomokuBoard::RemoveStone(int r, int c) {
    if (!IsInside(r, c)) return;
    cells[r][c] = 0;
    pieces[r][c] = GomokuPiece(0);
    occupied[r][c] = false;
}

sf::Vector2f GomokuBoard::GridToScreen(int r, int c) const {
    float x = ORIGIN_X + c * CELL;
    float y = ORIGIN_Y + r * CELL;
    return sf::Vector2f(x, y);
}

sf::Vector2i GomokuBoard::ScreenToGrid(float sx, float sy) const {
    int c = static_cast<int>(std::round((sx - ORIGIN_X) / CELL));
    int r = static_cast<int>(std::round((sy - ORIGIN_Y) / CELL));
    if (r < 0) r = 0; if (r >= SIZE) r = SIZE - 1;
    if (c < 0) c = 0; if (c >= SIZE) c = SIZE - 1;
    return sf::Vector2i(r, c);
}
