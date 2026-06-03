#include "Engine/AI/DiagonalChessAI.h"
#include <cmath>
#include <algorithm>

DiagonalChessAI::DiagonalChessAI() : difficulty(AIDifficulty::MEDIUM), aiDepth(3) {}

void DiagonalChessAI::UpdateDepth() {
    switch (difficulty) {
        case AIDifficulty::EASY:   aiDepth = 2; break;
        case AIDifficulty::MEDIUM: aiDepth = 3; break;
        case AIDifficulty::HARD:   aiDepth = 4; break;
    }
}

AIMove DiagonalChessAI::Think(const DiagonalChessBoard& orig, int aiSide, const DiagonalChessRule& rule) {
    DiagonalChessBoard temp;
    orig.CopyTo(temp);
    auto moves = GenerateAllMoves(temp, aiSide, rule);
    if (moves.empty()) return {-1, -1, -1, -1, 0};

    int bestScore = (aiSide == 0) ? -999999 : 999999;
    AIMove best = moves[0];

    for (auto& m : moves) {
        auto captured = temp.At(m.toR, m.toC);
        auto moved = temp.At(m.fromR, m.fromC);
        temp.At(m.toR, m.toC) = moved;
        temp.At(m.fromR, m.fromC) = DiagonalChessPiece(DChessPieceType::NONE, 0);
        temp.At(m.fromR, m.fromC).SetAlive(false);

        int score = Minimax(temp, aiDepth - 1, -999999, 999999, aiSide != 0, aiSide, rule);

        temp.At(m.fromR, m.fromC) = moved;
        temp.At(m.toR, m.toC) = captured;

        if (aiSide == 0) { if (score > bestScore) { bestScore = score; best = m; } }
        else            { if (score < bestScore) { bestScore = score; best = m; } }
    }
    best.score = bestScore;
    return best;
}

std::vector<AIMove> DiagonalChessAI::GenerateAllMoves(const DiagonalChessBoard& b, int side, const DiagonalChessRule& rule) const {
    std::vector<AIMove> moves;
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (!b.IsOccupied(r, c) || b.At(r, c).GetSide() != side) continue;
            for (int tr = 0; tr < 9; tr++) {
                for (int tc = 0; tc < 9; tc++) {
                    if (tr == r && tc == c) continue;
                    if (b.IsOccupied(tr, tc) && b.At(tr, tc).GetSide() == side) continue;
                    if (rule.IsValidMoveRaw(b, r, c, tr, tc)) {
                        if (rule.WouldBeInCheck(b, r, c, tr, tc, side)) continue;
                        moves.push_back({r, c, tr, tc, 0});
                    }
                }
            }
        }
    }
    return moves;
}

int DiagonalChessAI::Evaluate(const DiagonalChessBoard& b) const {
    bool redGen = false, blackGen = false;
    int score = 0;
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (!b.IsOccupied(r, c)) continue;
            auto t = b.At(r, c).GetDType();
            if (t == DChessPieceType::GENERAL) {
                if (b.At(r, c).GetSide() == 0) redGen = true;
                else blackGen = true;
            }
            int val = GetPieceValue(t) + GetPositionBonus(t, r, c, b.At(r, c).GetSide());
            if (b.At(r, c).GetSide() == 0) score += val;
            else score -= val;
        }
    }
    if (!redGen) return -100000;
    if (!blackGen) return 100000;
    return score;
}

int DiagonalChessAI::Minimax(DiagonalChessBoard& b, int depth, int alpha, int beta, bool maximizing, int aiSide, const DiagonalChessRule& rule) {
    if (depth == 0) return Evaluate(b);

    int currentSide = maximizing ? 0 : 1;
    auto moves = GenerateAllMoves(b, currentSide, rule);
    if (moves.empty()) return maximizing ? -100000 : 100000;

    if (maximizing) {
        int maxEval = -999999;
        for (const auto& m : moves) {
            auto captured = b.At(m.toR, m.toC);
            auto moved = b.At(m.fromR, m.fromC);
            b.At(m.toR, m.toC) = moved;
            b.At(m.fromR, m.fromC) = DiagonalChessPiece(DChessPieceType::NONE, 0);
            b.At(m.fromR, m.fromC).SetAlive(false);

            int eval = Minimax(b, depth - 1, alpha, beta, false, aiSide, rule);

            b.At(m.fromR, m.fromC) = moved;
            b.At(m.toR, m.toC) = captured;

            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) break;
        }
        return maxEval;
    } else {
        int minEval = 999999;
        for (const auto& m : moves) {
            auto captured = b.At(m.toR, m.toC);
            auto moved = b.At(m.fromR, m.fromC);
            b.At(m.toR, m.toC) = moved;
            b.At(m.fromR, m.fromC) = DiagonalChessPiece(DChessPieceType::NONE, 0);
            b.At(m.fromR, m.fromC).SetAlive(false);

            int eval = Minimax(b, depth - 1, alpha, beta, true, aiSide, rule);

            b.At(m.fromR, m.fromC) = moved;
            b.At(m.toR, m.toC) = captured;

            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) break;
        }
        return minEval;
    }
}

int DiagonalChessAI::GetPieceValue(DChessPieceType t) const {
    return DChessGetValue(t);
}

int DiagonalChessAI::GetPositionBonus(DChessPieceType t, int r, int c, int side) const {
    int dist = std::abs(r - 4) + std::abs(c - 4);
    switch (t) {
        case DChessPieceType::SOLDIER:
            return (side == 0) ? r * 10 : (8 - r) * 10;
        case DChessPieceType::HORSE:
            return (dist <= 3) ? 30 : 0;
        case DChessPieceType::CHARIOT:
            return (dist <= 4) ? 20 : 0;
        case DChessPieceType::CANNON:
            return (dist <= 4) ? 10 : 0;
        default:
            return 0;
    }
}
