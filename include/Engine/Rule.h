#pragma once
#include "Engine/Common.h"

class Board;

class Rule {
public:
    virtual ~Rule() = default;
    virtual bool IsValidMove(const Board& b, int fr, int fc, int tr, int tc) const = 0;
    virtual bool IsGameOver(const Board& b, Side side, bool& isDraw, Side& winner) const = 0;
    virtual bool IsInCheck(const Board& b, Side side) const = 0;
    virtual bool HasLegalMoves(const Board& b, Side side) const = 0;
};
