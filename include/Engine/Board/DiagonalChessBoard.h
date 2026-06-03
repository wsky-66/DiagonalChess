#pragma once
#include "Engine/Board.h"
#include "Engine/Common.h"
#include <SFML/Graphics.hpp>

class DiagonalChessBoard : public Board {
public:
    DiagonalChessBoard();

    int GetRows() const override { return 9; }
    int GetCols() const override { return 9; }

    Piece* GetPiece(int r, int c) override;
    const Piece* GetPiece(int r, int c) const override;
    void SetPiece(int r, int c, std::unique_ptr<Piece> p) override;
    std::unique_ptr<Piece> TakePiece(int r, int c) override;

    void Clear() override;
    void Reset() override;

    void ForEachPiece(std::function<void(Piece&, int r, int c)> fn) override;

    const DiagonalChessPiece& At(int r, int c) const { return cells[r][c]; }
    DiagonalChessPiece& At(int r, int c) { return cells[r][c]; }
    bool IsOccupied(int r, int c) const { return occupied[r][c]; }

    sf::Vector2f GridToScreen(int r, int c) const;
    sf::Vector2i ScreenToGrid(float sx, float sy) const;

    void CopyTo(DiagonalChessBoard& target) const;

    void MovePieceInternal(int fromR, int fromC, int toR, int toC);
    void ClearCell(int r, int c);
    void OccupiedCell(int r, int c);

    static constexpr int DIAG = 40;
    static constexpr float ORIGIN_X = 50.f;
    static constexpr float ORIGIN_Y = 410.f;

private:
    void PlacePieces();

    DiagonalChessPiece cells[9][9];
    bool occupied[9][9];
};
