// ===================================================================
// GomokuBoard.cpp — 五子棋棋盘的实现
// 15×15 标准棋盘，使用 cells[p,q] 存储棋子状态
// 0=空, 1=黑子, 2=白子
// ===================================================================

#include "Engine/Board/GomokuBoard.h"
#include <cmath>
#include <cstring>

// 构造函数：初始化所有格子为空
GomokuBoard::GomokuBoard() {
    std::memset(occupied, 0, sizeof(occupied));       // 所有位置标记为"无棋子"
    std::memset(cells, 0, sizeof(cells));              // 所有单元格重置为 0（空）
    for (int r = 0; r < SIZE; r++)
        for (int c = 0; c < SIZE; c++)
            pieces[r][c] = GomokuPiece(0);             // 初始化空棋子对象
}

// 清空棋盘
void GomokuBoard::Clear() {
    std::memset(occupied, 0, sizeof(occupied));
    std::memset(cells, 0, sizeof(cells));
    for (int r = 0; r < SIZE; r++)
        for (int c = 0; c < SIZE; c++)
            pieces[r][c] = GomokuPiece(0);
}

// 重置（五子棋重置即清空，无需初始布局）
void GomokuBoard::Reset() {
    Clear();
}

// —— 实现 Board 基类接口 ——

// 获取棋子的可修改指针（如果无棋子则返回 nullptr）
Piece* GomokuBoard::GetPiece(int r, int c) {
    if (!IsInside(r, c) || !occupied[r][c]) return nullptr;
    return &pieces[r][c];
}

// 获取棋子的只读指针
const Piece* GomokuBoard::GetPiece(int r, int c) const {
    if (!IsInside(r, c) || !occupied[r][c]) return nullptr;
    return &pieces[r][c];
}

// 在指定位置放置棋子
void GomokuBoard::SetPiece(int r, int c, std::unique_ptr<Piece> p) {
    if (!IsInside(r, c)) return;
    if (p) {
        auto* gp = dynamic_cast<GomokuPiece*>(p.get());  // 安全类型转换
        if (gp) {
            pieces[r][c] = *gp;
            cells[r][c] = gp->GetSide();                 // 同步 cells 数组
            occupied[r][c] = true;
        }
    } else {
        pieces[r][c] = GomokuPiece(0);
        cells[r][c] = 0;
        occupied[r][c] = false;
    }
}

// 从指定位置拿走棋子（返回所有权）
std::unique_ptr<Piece> GomokuBoard::TakePiece(int r, int c) {
    if (!IsInside(r, c) || !occupied[r][c]) return nullptr;
    auto p = std::make_unique<GomokuPiece>(pieces[r][c]);
    pieces[r][c] = GomokuPiece(0);
    cells[r][c] = 0;
    occupied[r][c] = false;
    return p;
}

// 遍历所有棋子，对每个棋子调用回调函数
void GomokuBoard::ForEachPiece(std::function<void(Piece&, int r, int c)> fn) {
    for (int r = 0; r < SIZE; r++)
        for (int c = 0; c < SIZE; c++)
            if (occupied[r][c])
                fn(pieces[r][c], r, c);
}

// —— 五子棋专用操作 ——

// 落子：在 (r,c) 放置 side 方的棋子
void GomokuBoard::PlaceStone(int r, int c, int side) {
    if (!IsInside(r, c) || side == 0) return;
    cells[r][c] = side;
    pieces[r][c] = GomokuPiece(side);
    occupied[r][c] = true;
}

// 移除棋子（悔棋操作）
void GomokuBoard::RemoveStone(int r, int c) {
    if (!IsInside(r, c)) return;
    cells[r][c] = 0;
    pieces[r][c] = GomokuPiece(0);
    occupied[r][c] = false;
}

// 坐标转换：网格坐标 → 屏幕像素坐标（交叉点中心）
// 公式：pixelX = ORIGIN_X + col * CELL, pixelY = ORIGIN_Y + row * CELL
sf::Vector2f GomokuBoard::GridToScreen(int r, int c) const {
    float x = ORIGIN_X + c * CELL;
    float y = ORIGIN_Y + r * CELL;
    return sf::Vector2f(x, y);
}

// 坐标转换：屏幕像素 → 最近的网格交叉点
// 使用四舍五入找到最近的整数坐标，并限制在棋盘范围内
sf::Vector2i GomokuBoard::ScreenToGrid(float sx, float sy) const {
    int c = static_cast<int>(std::round((sx - ORIGIN_X) / CELL));
    int r = static_cast<int>(std::round((sy - ORIGIN_Y) / CELL));
    if (r < 0) r = 0; if (r >= SIZE) r = SIZE - 1;    // 边界保护
    if (c < 0) c = 0; if (c >= SIZE) c = SIZE - 1;
    return sf::Vector2i(r, c);
}
