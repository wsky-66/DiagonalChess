#pragma once
#include "Engine/Board.h"
#include "Engine/Common.h"
#include <SFML/Graphics.hpp>

class DiagonalChessBoard : public Board {
public:
    DiagonalChessBoard();

    int GetRows() const override { return 9; }
    int GetCols() const override { return 9; }
    void Reset() override;

    const Piece(&GetBoard() const)[9][9] { return board; }
    Piece(&GetBoard())[9][9] { return board; }

    sf::Vector2f GridToScreen(int r, int c) const;
    sf::Vector2i ScreenToGrid(float sx, float sy) const;

    static constexpr int DIAG = 40;
    static constexpr float ORIGIN_X = 50.f;

    static constexpr float ORIGIN_Y = 410.f;
private:
    void InitBoard();
    void PlacePieces();

    Piece board[9][9];
};
