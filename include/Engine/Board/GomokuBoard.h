#pragma once
#include "Engine/Board.h"
#include "Engine/Piece.h"
#include <SFML/Graphics.hpp>
#include <functional>

// 五子棋棋子：仅用 side 区分颜色，1=黑方，2=白方
class GomokuPiece : public Piece {
public:
    GomokuPiece(int s = 0) : Piece(s) {}
    std::wstring GetSymbol() const override { return L""; }
    int GetType() const override { return side; }
    std::unique_ptr<Piece> Clone() const override {
        return std::make_unique<GomokuPiece>(*this);
    }
};

// 五子棋棋盘：15x15 交叉点，使用 GomokuPiece 存储状态
// 0 = 空，1 = 黑子，2 = 白子
class GomokuBoard : public Board {
public:
    static constexpr int SIZE = 15;
    static constexpr int BOARD_W = 1150;
    static constexpr int BOARD_H = 820;
    static constexpr int LEFT_W = 700;       // 左侧棋盘区域宽度
    static constexpr float CELL = 36.f;      // 格距
    static constexpr float ORIGIN_X = 98.f;  // 棋盘左上角 X（在左侧区域内居中）
    static constexpr float ORIGIN_Y = 158.f; // 棋盘左上角 Y
    static constexpr float STONE_R = 16.f;   // 棋子半径

    GomokuBoard();

    int GetRows() const override { return SIZE; }
    int GetCols() const override { return SIZE; }

    Piece* GetPiece(int r, int c) override;
    const Piece* GetPiece(int r, int c) const override;
    void SetPiece(int r, int c, std::unique_ptr<Piece> p) override;
    std::unique_ptr<Piece> TakePiece(int r, int c) override;

    void Clear() override;
    void Reset() override;

    void ForEachPiece(std::function<void(Piece&, int r, int c)> fn) override;

    // 五子棋专用接口
    int  GetCell(int r, int c) const { return cells[r][c]; }
    void SetCell(int r, int c, int side) { cells[r][c] = side; }
    bool IsEmpty(int r, int c) const { return cells[r][c] == 0; }
    bool IsOccupiedG(int r, int c) const { return cells[r][c] != 0; }

    // 落子（会更新 occupied 和 pieces 数组）
    void PlaceStone(int r, int c, int side);
    // 移除棋子
    void RemoveStone(int r, int c);

    // 坐标转换：网格 → 像素（交叉点中心）
    sf::Vector2f GridToScreen(int r, int c) const;
    // 坐标转换：像素 → 最近网格
    sf::Vector2i ScreenToGrid(float sx, float sy) const;

private:
    int cells[SIZE][SIZE];          // 0=空, 1=黑, 2=白
    bool occupied[SIZE][SIZE];      // 是否有棋子
    GomokuPiece pieces[SIZE][SIZE]; // Piece 对象，服务 Board 抽象接口
};
