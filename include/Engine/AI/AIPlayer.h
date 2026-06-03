#pragma once
#include "Engine/Common.h"

class Board;
class Rule;

class AIPlayer {
public:
    virtual ~AIPlayer() = default;
    virtual void SetDifficulty(AIDifficulty d) = 0;
};
