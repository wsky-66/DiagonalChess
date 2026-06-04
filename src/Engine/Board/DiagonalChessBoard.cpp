// ===================================================================
// DiagonalChessBoard.cpp — 对角象棋棋盘的实现
// 9×9 棋盘，坐标系斜放 45 度
// 屏幕坐标公式：
//   x = ORIGIN_X + (row + col) * DIAG
//   y = ORIGIN_Y + (col - row) * DIAG
// ===================================================================

#include "Engine/Board/DiagonalChessBoard.h"
#include <cmath>
#include <cstring>

// 构造函数：初始化棋盘并摆放初始棋子
DiagonalChessBoard::DiagonalChessBoard() {
    std::memset(occupied, 0, sizeof(occupied));       // 全部标记为空
    for (int r = 0; r < 9; r++)
        for (int c = 0; c < 9; c++)
            cells[r][c].SetAlive(false);               // 标记所有棋子为"死亡"状态
    PlacePieces();                                     // 摆放初始布局
}

// 清空棋盘（所有位置变为空）
void DiagonalChessBoard::Clear() {
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            cells[r][c] = DiagonalChessPiece(DChessPieceType::NONE, 0);  // 空棋子
            cells[r][c].SetAlive(false);
            occupied[r][c] = false;
        }
    }
}

// 重置棋盘（清空后重新摆放初始棋子）
void DiagonalChessBoard::Reset() {
    Clear();
    PlacePieces();
}

// ==================== 摆放初始棋子布局 ====================
// 红方（side=0）位于棋盘下方（行 0~4，列 2~8）
// 黑方（side=1）位于棋盘上方（行 4~8，列 0~6）
// 布局与传统中国象棋布局一致（旋转45度后）
void DiagonalChessBoard::PlacePieces() {
    // 辅助 lambda：在指定位置放一个指定类型和阵营的棋子
    auto put = [this](int r, int c, DChessPieceType t, int side) {
        cells[r][c] = DiagonalChessPiece(t, side);
        occupied[r][c] = true;
    };

    // —— 红方（side=0）布局 ——
    // 行0: 帅(0,8) 仕(0,7) 相(0,6) 马(0,5) 车(0,4) 炮(0,3) 兵(0,2)
    // 其他: 仕(1,8) 相(2,8) 马(3,8) 车(4,8) 炮(5,8) 兵(2,4)(2,6)(4,6)(6,8)
    put(0, 8, DChessPieceType::GENERAL,  0);    // 帅
    put(0, 7, DChessPieceType::ADVISOR,  0);    // 仕（左）
    put(1, 8, DChessPieceType::ADVISOR,  0);    // 仕（右）
    put(0, 6, DChessPieceType::ELEPHANT, 0);    // 相（左）
    put(2, 8, DChessPieceType::ELEPHANT, 0);    // 相（右）
    put(0, 5, DChessPieceType::HORSE,    0);    // 马（左）
    put(3, 8, DChessPieceType::HORSE,    0);    // 马（右）
    put(0, 4, DChessPieceType::CHARIOT,  0);    // 车（左）
    put(4, 8, DChessPieceType::CHARIOT,  0);    // 车（右）
    put(0, 3, DChessPieceType::CANNON,   0);    // 炮（左）
    put(5, 8, DChessPieceType::CANNON,   0);    // 炮（右）
    put(0, 2, DChessPieceType::SOLDIER,  0);    // 兵
    put(2, 4, DChessPieceType::SOLDIER,  0);
    put(2, 6, DChessPieceType::SOLDIER,  0);
    put(4, 6, DChessPieceType::SOLDIER,  0);
    put(6, 8, DChessPieceType::SOLDIER,  0);

    // —— 黑方（side=1）布局（与红方对称）——
    put(8, 0, DChessPieceType::GENERAL,  1);    // 将
    put(8, 1, DChessPieceType::ADVISOR,  1);    // 士
    put(7, 0, DChessPieceType::ADVISOR,  1);
    put(8, 2, DChessPieceType::ELEPHANT, 1);    // 象
    put(6, 0, DChessPieceType::ELEPHANT, 1);
    put(8, 3, DChessPieceType::HORSE,    1);    // 马
    put(5, 0, DChessPieceType::HORSE,    1);
    put(8, 4, DChessPieceType::CHARIOT,  1);    // 车
    put(4, 0, DChessPieceType::CHARIOT,  1);
    put(8, 5, DChessPieceType::CANNON,   1);    // 炮
    put(3, 0, DChessPieceType::CANNON,   1);
    put(8, 6, DChessPieceType::SOLDIER,  1);    // 卒
    put(6, 4, DChessPieceType::SOLDIER,  1);
    put(6, 2, DChessPieceType::SOLDIER,  1);
    put(4, 2, DChessPieceType::SOLDIER,  1);
    put(2, 0, DChessPieceType::SOLDIER,  1);
}

// —— 实现 Board 基类接口 ——

// 获取棋子的可修改指针
Piece* DiagonalChessBoard::GetPiece(int r, int c) {
    return (r >= 0 && r < 9 && c >= 0 && c < 9 && occupied[r][c]) ? &cells[r][c] : nullptr;
}

// 获取棋子的只读指针
const Piece* DiagonalChessBoard::GetPiece(int r, int c) const {
    return (r >= 0 && r < 9 && c >= 0 && c < 9 && occupied[r][c]) ? &cells[r][c] : nullptr;
}

// 在指定位置放置棋子
void DiagonalChessBoard::SetPiece(int r, int c, std::unique_ptr<Piece> p) {
    if (!IsInside(r, c)) return;
    if (p) {
        auto* dcp = dynamic_cast<DiagonalChessPiece*>(p.get());  // 安全类型转换
        if (dcp) {
            cells[r][c] = *dcp;
            occupied[r][c] = true;
        }
    } else {
        ClearCell(r, c);                             // nullptr 表示清空该位置
    }
}

// 从指定位置拿走棋子
std::unique_ptr<Piece> DiagonalChessBoard::TakePiece(int r, int c) {
    if (!IsInside(r, c) || !occupied[r][c]) return nullptr;
    auto p = std::make_unique<DiagonalChessPiece>(cells[r][c]);  // 复制棋子
    ClearCell(r, c);                                  // 清空原位置
    return p;
}

// 遍历所有棋子
void DiagonalChessBoard::ForEachPiece(std::function<void(Piece&, int r, int c)> fn) {
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (occupied[r][c]) {
                fn(cells[r][c], r, c);
            }
        }
    }
}

// 将当前棋盘完整复制到目标棋盘（用于 AI 模拟走棋）
void DiagonalChessBoard::CopyTo(DiagonalChessBoard& target) const {
    target.Clear();
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            target.cells[r][c] = cells[r][c];
            target.occupied[r][c] = occupied[r][c];
        }
    }
}

// 内部移动棋子（不记录历史，不播音效）
void DiagonalChessBoard::MovePieceInternal(int fromR, int fromC, int toR, int toC) {
    if (!IsInside(fromR, fromC) || !IsInside(toR, toC)) return;
    cells[toR][toC] = cells[fromR][fromC];           // 目标位置 = 源棋子
    occupied[toR][toC] = true;
    ClearCell(fromR, fromC);                          // 清空源位置
}

// 清空指定单元格
void DiagonalChessBoard::ClearCell(int r, int c) {
    if (!IsInside(r, c)) return;
    cells[r][c] = DiagonalChessPiece(DChessPieceType::NONE, 0);
    cells[r][c].SetAlive(false);
    occupied[r][c] = false;
}

// 标记指定单元格为有棋子
void DiagonalChessBoard::OccupiedCell(int r, int c) {
    if (!IsInside(r, c)) return;
    occupied[r][c] = true;
}

// ==================== 坐标转换 ====================
// 对角象棋的棋盘是 45 度斜放的，所以坐标转换公式如下：
// screenX = ORIGIN_X + (row + col) * DIAG   （越往右下，X 越大）
// screenY = ORIGIN_Y + (col - row) * DIAG   （越往右上，Y 越小）

// 网格 → 屏幕像素
sf::Vector2f DiagonalChessBoard::GridToScreen(int r, int c) const {
    float x = ORIGIN_X + (r + c) * DIAG;
    float y = ORIGIN_Y + (c - r) * DIAG;
    return sf::Vector2f(x, y);
}

// 屏幕像素 → 网格（反向求解线性方程组）
// 由 screenX = OX + (r+c)*DIAG, screenY = OY + (c-r)*DIAG
// 解得：r = (dx/DIAG - dy/DIAG) / 2, c = (dx/DIAG + dy/DIAG) / 2
sf::Vector2i DiagonalChessBoard::ScreenToGrid(float sx, float sy) const {
    float dx = sx - ORIGIN_X;
    float dy = sy - ORIGIN_Y;
    float rf = (dx / DIAG - dy / DIAG) / 2.f;
    float cf = (dx / DIAG + dy / DIAG) / 2.f;
    int r = static_cast<int>(std::round(rf));         // 四舍五入到最近的整数网格
    int c = static_cast<int>(std::round(cf));
    return sf::Vector2i(r, c);
}
