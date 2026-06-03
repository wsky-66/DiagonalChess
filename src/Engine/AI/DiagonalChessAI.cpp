#include "Engine/AI/DiagonalChessAI.h"
#include <cmath>
#include <algorithm>

DiagonalChessAI::DiagonalChessAI() : difficulty(AIDifficulty::MEDIUM), aiDepth(3) {}

void DiagonalChessAI::UpdateDepth() {
    switch (difficulty) {
        case AIDifficulty::EASY: aiDepth = 2; break;
        case AIDifficulty::MEDIUM: aiDepth = 3; break;
        case AIDifficulty::HARD: aiDepth = 4; break;
    }
}

AIMove DiagonalChessAI::Think(const Piece b[9][9], Side aiSide, const Rule& rule) {
    Piece tempBoard[9][9];
    for (int r = 0; r < 9; r++)
        for (int c = 0; c < 9; c++)
            tempBoard[r][c] = b[r][c];

    auto moves = GenerateAllMoves(tempBoard, aiSide, rule);
    if (moves.empty()) return {-1, -1, -1, -1, 0};

    int bestScore = (aiSide == Side::RED) ? -999999 : 999999;
    AIMove best = moves[0];

    for (auto& move : moves) {
        Piece captured;
        MakeMoveAI(tempBoard, move, captured);
        Piece moved = tempBoard[move.toR][move.toC];

        int score = Minimax(tempBoard, aiDepth - 1, -999999, 999999,
                           aiSide == Side::BLACK, aiSide, rule);

        UndoMoveAI(tempBoard, move, captured, moved);

        if (aiSide == Side::RED) {
            if (score > bestScore) { bestScore = score; best = move; }
        } else {
            if (score < bestScore) { bestScore = score; best = move; }
        }
    }
    best.score = bestScore;
    return best;
}

std::vector<AIMove> DiagonalChessAI::GenerateAllMoves(const Piece b[9][9], Side side, const Rule& rule) const {
    std::vector<AIMove> moves;
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (!b[r][c].alive || b[r][c].side != side) continue;
            for (int tr = 0; tr < 9; tr++) {
                for (int tc = 0; tc < 9; tc++) {
                    if (tr == r && tc == c) continue;
                    if (b[tr][tc].alive && b[tr][tc].side == side) continue;
                    if (rule.IsValidMove(b, r, c, tr, tc)) {
                        if (static_cast<const DiagonalChessRule&>(rule).WouldBeInCheck(b, r, c, tr, tc, b[r][c].side))
                            continue;
                        AIMove m;
                        m.fromR = r; m.fromC = c;
                        m.toR = tr; m.toC = tc;
                        m.score = 0;
                        moves.push_back(m);
                    }
                }
            }
        }
    }
    return moves;
}

int DiagonalChessAI::Evaluate(const Piece b[9][9]) const {
    bool redGeneral = false, blackGeneral = false;
    int score = 0;

    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (!b[r][c].alive) continue;
            if (b[r][c].type == PieceType::GENERAL) {
                if (b[r][c].side == Side::RED) redGeneral = true;
                else blackGeneral = true;
            }
            int val = GetPieceValue(b[r][c].type) + GetPositionBonus(b[r][c].type, r, c, b[r][c].side);
            if (b[r][c].side == Side::RED) score += val;
            else score -= val;
        }
    }

    if (!redGeneral) return -100000;
    if (!blackGeneral) return 100000;
    return score;
}

int DiagonalChessAI::Minimax(Piece b[9][9], int depth, int alpha, int beta, bool isMaximizing, Side aiSide, const Rule& rule) {
    if (depth == 0) return Evaluate(b);

    Side currentSide = isMaximizing ? Side::RED : Side::BLACK;
    auto moves = GenerateAllMoves(b, currentSide, rule);

    if (moves.empty()) {
        return isMaximizing ? -100000 : 100000;
    }

    if (isMaximizing) {
        int maxEval = -999999;
        for (const auto& move : moves) {
            Piece captured;
            MakeMoveAI(b, move, captured);
            Piece moved = b[move.toR][move.toC];
            int eval = Minimax(b, depth - 1, alpha, beta, false, aiSide, rule);
            UndoMoveAI(b, move, captured, moved);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) break;
        }
        return maxEval;
    } else {
        int minEval = 999999;
        for (const auto& move : moves) {
            Piece captured;
            MakeMoveAI(b, move, captured);
            Piece moved = b[move.toR][move.toC];
            int eval = Minimax(b, depth - 1, alpha, beta, true, aiSide, rule);
            UndoMoveAI(b, move, captured, moved);
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) break;
        }
        return minEval;
    }
}

void DiagonalChessAI::MakeMoveAI(Piece b[9][9], const AIMove& move, Piece& captured) const {
    captured = b[move.toR][move.toC];
    b[move.toR][move.toC] = b[move.fromR][move.fromC];
    b[move.fromR][move.fromC] = {PieceType::NONE, Side::RED, false};
}

void DiagonalChessAI::UndoMoveAI(Piece b[9][9], const AIMove& move, const Piece& captured, const Piece& moved) const {
    b[move.fromR][move.fromC] = moved;
    b[move.toR][move.toC] = captured;
}

int DiagonalChessAI::GetPieceValue(PieceType type) const {
    switch (type) {
        case PieceType::GENERAL: return 10000;
        case PieceType::CHARIOT: return 900;
        case PieceType::CANNON: return 450;
        case PieceType::HORSE: return 400;
        case PieceType::ELEPHANT: return 200;
        case PieceType::ADVISOR: return 200;
        case PieceType::SOLDIER: return 100;
        default: return 0;
    }
}

int DiagonalChessAI::GetPositionBonus(PieceType type, int r, int c, Side side) const {
    int dist = std::abs(r - 4) + std::abs(c - 4);
    switch (type) {
        case PieceType::SOLDIER:
            if (side == Side::RED) return r * 10;
            else return (8 - r) * 10;
        case PieceType::HORSE:
            if (dist <= 3) return 30;
            return 0;
        case PieceType::CHARIOT:
            if (dist <= 4) return 20;
            return 0;
        case PieceType::CANNON:
            if (dist <= 4) return 10;
            return 0;
        default:
            return 0;
    }
}
