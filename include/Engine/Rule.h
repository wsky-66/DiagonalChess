#pragma once
#include "Engine/Common.h"

class Rule {
public:
    virtual ~Rule() = default;
    virtual bool IsValidMove(const Piece b[9][9], int fromR, int fromC, int toR, int toC) const = 0;
    virtual bool IsGameOver(const Piece b[9][9], Side currentTurn, bool& isDraw, Side& winner) const = 0;
    virtual bool IsInCheck(const Piece b[9][9], Side side) const = 0;
    virtual bool HasLegalMoves(const Piece b[9][9], Side side) const = 0;
};
