#pragma once

// ===================================================================
// GomokuBoard.h — 五子棋棋盘类
// 15×15 的交叉点标准棋盘
// 支持：落子、移除棋子、坐标转换
// ===================================================================

#include "Engine/Board.h"
#include "Engine/Piece.h"
#include "Engine/Piece/GomokuPiece.h"
#include <SFML/Graphics.hpp>
#include <functional>

// ==================== 五子棋棋盘 ====================
class GomokuBoard : public Board {
public:
    // —— 常量 ——
    static constexpr int SIZE = 15;            // 棋盘大小 15×15
    static constexpr int BOARD_W = 1150;       // 窗口宽度
    static constexpr int BOARD_H = 820;        // 窗口高度
    static constexpr int LEFT_W = 700;         // 左侧棋盘区域宽度
    static constexpr float CELL = 36.f;        // 相邻交叉点的间距（像素）
    static constexpr float ORIGIN_X = 98.f;    // 棋盘左上角 X 坐标
    static constexpr float ORIGIN_Y = 158.f;   // 棋盘左上角 Y 坐标
    static constexpr float STONE_R = 16.f;     // 棋子半径

    GomokuBoard();

    // —— 实现 Board 基类接口 ——
    int GetRows() const override { return SIZE; }
    int GetCols() const override { return SIZE; }

    Piece* GetPiece(int r, int c) override;
    const Piece* GetPiece(int r, int c) const override;
    void SetPiece(int r, int c, std::unique_ptr<Piece> p) override;
    std::unique_ptr<Piece> TakePiece(int r, int c) override;

    void Clear() override;
    void Reset() override;

    void ForEachPiece(std::function<void(Piece&, int r, int c)> fn) override;

    // —— 五子棋专用接口 ——
    int  GetCell(int r, int c) const { return cells[r][c]; }   // 获取该位置的值（0=空,1=黑,2=白）
    void SetCell(int r, int c, int side) { cells[r][c] = side; }
    bool IsEmpty(int r, int c) const { return cells[r][c] == 0; }      // 是否空位
    bool IsOccupiedG(int r, int c) const { return cells[r][c] != 0; }  // 是否有棋子（G=Gomoku）

    // 落子：在 (r,c) 放一个 side 方棋子
    void PlaceStone(int r, int c, int side);
    // 移除棋子
    void RemoveStone(int r, int c);

    // 坐标转换：网格 → 像素（交叉点中心）
    sf::Vector2f GridToScreen(int r, int c) const;
    // 坐标转换：像素 → 最近的网格交叉点
    sf::Vector2i ScreenToGrid(float sx, float sy) const;

private:
    int cells[SIZE][SIZE];               // 棋盘状态数组：0=空, 1=黑, 2=白
    bool occupied[SIZE][SIZE];           // 是否有棋子（与 cells 同步）
    GomokuPiece pieces[SIZE][SIZE];      // Piece 对象数组（服务 Board 抽象接口）
};
