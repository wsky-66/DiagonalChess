#pragma once
#include "Engine/Common.h"
#include "Engine/Board.h"
#include "Engine/Rule.h"

class AIPlayer {
public:
    virtual ~AIPlayer() = default;
    virtual AIMove Think(const Piece b[9][9], Side aiSide, const Rule& rule) = 0;
    virtual void SetDifficulty(AIDifficulty d) = 0;
};
