#pragma once
#include "Engine/AI/AIPlayer.h"
#include "Engine/Rule.h"
#include "Engine/Rule/DiagonalChessRule.h"

class DiagonalChessAI : public AIPlayer {
public:
    DiagonalChessAI();

    AIMove Think(const Piece b[9][9], Side aiSide, const Rule& rule) override;
    void SetDifficulty(AIDifficulty d) override { difficulty = d; UpdateDepth(); }
    AIDifficulty GetDifficulty() const { return difficulty; }

private:
    void UpdateDepth();
    std::vector<AIMove> GenerateAllMoves(const Piece b[9][9], Side side, const Rule& rule) const;
    int Evaluate(const Piece b[9][9]) const;
    int Minimax(Piece b[9][9], int depth, int alpha, int beta, bool isMaximizing, Side aiSide, const Rule& rule);
    void MakeMoveAI(Piece b[9][9], const AIMove& move, Piece& captured) const;
    void UndoMoveAI(Piece b[9][9], const AIMove& move, const Piece& captured, const Piece& moved) const;
    int GetPieceValue(PieceType type) const;
    int GetPositionBonus(PieceType type, int r, int c, Side side) const;

    AIDifficulty difficulty;
    int aiDepth;
};
