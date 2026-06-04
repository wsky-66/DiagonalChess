#pragma once

// ===================================================================
// DiagonalChessBoard.h — 对角象棋棋盘类
// 9×9 的棋盘，坐标系斜放 45 度（所以叫"对角象棋"）
// 网格坐标 (r,c) 通过旋转矩阵映射到屏幕像素坐标
// ===================================================================

#include "Engine/Board.h"
#include "Engine/Piece/DiagonalChessPiece.h"
#include "Engine/Common.h"
#include <SFML/Graphics.hpp>

class DiagonalChessBoard : public Board {
public:
    DiagonalChessBoard();

    // —— 实现 Board 基类接口 ——
    int GetRows() const override { return 9; }      // 固定 9 行
    int GetCols() const override { return 9; }      // 固定 9 列

    Piece* GetPiece(int r, int c) override;
    const Piece* GetPiece(int r, int c) const override;
    void SetPiece(int r, int c, std::unique_ptr<Piece> p) override;
    std::unique_ptr<Piece> TakePiece(int r, int c) override;

    void Clear() override;                           // 清空棋盘
    void Reset() override;                           // 重置为初始布局

    void ForEachPiece(std::function<void(Piece&, int r, int c)> fn) override;

    // —— 对角象棋专用方法 ——
    // 直接访问棋子（返回引用，效率更高）
    const DiagonalChessPiece& At(int r, int c) const { return cells[r][c]; }
    DiagonalChessPiece& At(int r, int c) { return cells[r][c]; }
    bool IsOccupied(int r, int c) const { return occupied[r][c]; }  // 该位置是否有棋子

    // 坐标转换：网格坐标 → 屏幕像素坐标（45度斜放）
    // 公式：screenX = ORIGIN_X + (row + col) * DIAG
    //       screenY = ORIGIN_Y + (col - row) * DIAG
    sf::Vector2f GridToScreen(int r, int c) const;

    // 坐标转换：屏幕像素 → 最近的网格坐标
    sf::Vector2i ScreenToGrid(float sx, float sy) const;

    // 将当前棋盘状态完整复制到目标棋盘（供 AI 模拟走棋用）
    void CopyTo(DiagonalChessBoard& target) const;

    // 内部移动棋子（不记录历史，不播音效）
    void MovePieceInternal(int fromR, int fromC, int toR, int toC);

    // 操作辅助方法
    void ClearCell(int r, int c);        // 将指定位置清空
    void OccupiedCell(int r, int c);     // 标记指定位置为有棋子

    static constexpr int DIAG = 40;      // 格距（半对角线长度）
    static constexpr float ORIGIN_X = 50.f;
    static constexpr float ORIGIN_Y = 410.f;

private:
    void PlacePieces();                  // 摆放初始棋子（红方在下，黑方在上）

    DiagonalChessPiece cells[9][9];      // 9×9 的棋子数组（值类型，非指针）
    bool occupied[9][9];                 // 9×9 的占用标记（true=有棋子）
};
