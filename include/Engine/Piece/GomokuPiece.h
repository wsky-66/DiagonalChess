#pragma once

// ===================================================================
// GomokuPiece.h — 五子棋棋子类
// 五子棋的棋子很简单：只有黑和白两种，用 side 区分
// side=1 表示黑子, side=2 表示白子, side=0 表示空位
// ===================================================================

#include "Engine/Piece.h"
#include <memory>

class GomokuPiece : public Piece {
public:
    GomokuPiece(int s = 0);
    std::wstring GetSymbol() const override;
    int GetType() const override;
    std::unique_ptr<Piece> Clone() const override;
};
