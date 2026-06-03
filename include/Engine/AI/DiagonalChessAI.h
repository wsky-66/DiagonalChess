#pragma once
#include "Engine/AI/AIPlayer.h"
#include "Engine/Rule.h"
#include "Engine/Rule/DiagonalChessRule.h"
#include "Engine/Board/DiagonalChessBoard.h"

class DiagonalChessAI : public AIPlayer {
public:
    DiagonalChessAI();

    AIMove Think(const DiagonalChessBoard& board, int aiSide, const DiagonalChessRule& rule);
    void SetDifficulty(AIDifficulty d) override { difficulty = d; UpdateDepth(); }
    AIDifficulty GetDifficulty() const { return difficulty; }

private:
    void UpdateDepth();
    std::vector<AIMove> GenerateAllMoves(const DiagonalChessBoard& b, int side, const DiagonalChessRule& rule) const;
    int Evaluate(const DiagonalChessBoard& b) const;
    int Minimax(DiagonalChessBoard& b, int depth, int alpha, int beta, bool maximizing, int aiSide, const DiagonalChessRule& rule);
    int GetPieceValue(DChessPieceType t) const;
    int GetPositionBonus(DChessPieceType t, int r, int c, int side) const;

    AIDifficulty difficulty;
    int aiDepth;
};
