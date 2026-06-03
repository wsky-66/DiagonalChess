#include "Engine/Rule/DiagonalChessRule.h"
#include <cmath>
#include <algorithm>

bool DiagonalChessRule::IsInPalace(int r, int c, Side side) const {
    if (side == Side::RED) {
        return r >= 0 && r <= 2 && c >= 6 && c <= 8;
    } else {
        return r >= 6 && r <= 8 && c >= 0 && c <= 2;
    }
}

bool DiagonalChessRule::IsValidMove(const Piece b[9][9], int fromR, int fromC, int toR, int toC) const {
    if (fromR < 0 || fromR >= 9 || fromC < 0 || fromC >= 9) return false;
    if (toR < 0 || toR >= 9 || toC < 0 || toC >= 9) return false;

    const Piece& piece = b[fromR][fromC];
    if (!piece.alive) return false;
    if (b[toR][toC].alive && b[toR][toC].side == piece.side) return false;

    switch (piece.type) {
        case PieceType::CHARIOT: return CanChariotMove(b, fromR, fromC, toR, toC);
        case PieceType::HORSE: return CanHorseMove(b, fromR, fromC, toR, toC);
        case PieceType::ELEPHANT: return CanElephantMove(b, fromR, fromC, toR, toC);
        case PieceType::ADVISOR: return CanAdvisorMove(b, fromR, fromC, toR, toC);
        case PieceType::GENERAL: return CanGeneralMove(b, fromR, fromC, toR, toC);
        case PieceType::CANNON: return CanCannonMove(b, fromR, fromC, toR, toC);
        case PieceType::SOLDIER: return CanSoldierMove(b, fromR, fromC, toR, toC, piece.side);
        default: return false;
    }
}

bool DiagonalChessRule::CanChariotMove(const Piece b[9][9], int fr, int fc, int tr, int tc) const {
    if (fr != tr && fc != tc) return false;
    return CountPiecesBetween(b, fr, fc, tr, tc) == 0;
}

bool DiagonalChessRule::CanHorseMove(const Piece b[9][9], int fr, int fc, int tr, int tc) const {
    int dr = std::abs(tr - fr);
    int dc = std::abs(tc - fc);
    if (!((dr == 2 && dc == 1) || (dr == 1 && dc == 2))) return false;
    return !IsBlockedHorse(b, fr, fc, tr, tc);
}

bool DiagonalChessRule::IsBlockedHorse(const Piece b[9][9], int fr, int fc, int tr, int tc) const {
    int dr = tr - fr;
    int dc = tc - fc;
    if (std::abs(dr) == 2) {
        int blockR = fr + (dr > 0 ? 1 : -1);
        if (b[blockR][fc].alive) return true;
    } else {
        int blockC = fc + (dc > 0 ? 1 : -1);
        if (b[fr][blockC].alive) return true;
    }
    return false;
}

bool DiagonalChessRule::CanElephantMove(const Piece b[9][9], int fr, int fc, int tr, int tc) const {
    int dr = std::abs(tr - fr);
    int dc = std::abs(tc - fc);
    if (dr != 2 || dc != 2) return false;
    return !IsBlockedElephant(b, fr, fc, tr, tc);
}

bool DiagonalChessRule::IsBlockedElephant(const Piece b[9][9], int fr, int fc, int tr, int tc) const {
    int midR = (fr + tr) / 2;
    int midC = (fc + tc) / 2;
    return b[midR][midC].alive;
}

bool DiagonalChessRule::CanAdvisorMove(const Piece b[9][9], int fr, int fc, int tr, int tc) const {
    if (!IsInPalace(tr, tc, b[fr][fc].side)) return false;
    int dr = std::abs(tr - fr);
    int dc = std::abs(tc - fc);
    return (dr <= 1 && dc <= 1 && (dr + dc > 0));
}

bool DiagonalChessRule::CanGeneralMove(const Piece b[9][9], int fr, int fc, int tr, int tc) const {
    if (!IsInPalace(tr, tc, b[fr][fc].side)) return false;
    int dr = std::abs(tr - fr);
    int dc = std::abs(tc - fc);
    return (dr + dc == 1);
}

bool DiagonalChessRule::CanCannonMove(const Piece b[9][9], int fr, int fc, int tr, int tc) const {
    if (fr != tr && fc != tc) return false;
    int count = CountPiecesBetween(b, fr, fc, tr, tc);
    if (b[tr][tc].alive) {
        return count == 1;
    } else {
        return count == 0;
    }
}

int DiagonalChessRule::CountPiecesBetween(const Piece b[9][9], int fr, int fc, int tr, int tc) const {
    int count = 0;
    if (fr == tr) {
        int minC = std::min(fc, tc);
        int maxC = std::max(fc, tc);
        for (int c = minC + 1; c < maxC; c++) {
            if (b[fr][c].alive) count++;
        }
    } else if (fc == tc) {
        int minR = std::min(fr, tr);
        int maxR = std::max(fr, tr);
        for (int r = minR + 1; r < maxR; r++) {
            if (b[r][fc].alive) count++;
        }
    }
    return count;
}

bool DiagonalChessRule::CanSoldierMove(const Piece b[9][9], int fr, int fc, int tr, int tc, Side side) const {
    int dr = tr - fr;
    int dc = tc - fc;
    if (std::abs(dr) + std::abs(dc) != 1) return false;
    if (side == Side::RED) {
        return (dr == 1 && dc == 0) || (dr == 0 && dc == -1);
    } else {
        return (dr == -1 && dc == 0) || (dr == 0 && dc == 1);
    }
}

std::vector<sf::Vector2i> DiagonalChessRule::GetValidMoves(const Piece b[9][9], int r, int c) const {
    std::vector<sf::Vector2i> moves;
    if (r < 0 || r >= 9 || c < 0 || c >= 9) return moves;
    if (!b[r][c].alive) return moves;

    const Piece& piece = b[r][c];
    for (int tr = 0; tr < 9; tr++) {
        for (int tc = 0; tc < 9; tc++) {
            if (tr == r && tc == c) continue;
            if (b[tr][tc].alive && b[tr][tc].side == piece.side) continue;
            if (IsValidMove(b, r, c, tr, tc)) {
                if (!WouldBeInCheck(b, r, c, tr, tc, piece.side)) {
                    moves.push_back(sf::Vector2i(tr, tc));
                }
            }
        }
    }
    return moves;
}

bool DiagonalChessRule::IsInCheck(const Piece b[9][9], Side side) const {
    return IsInCheckB(b, side);
}

bool DiagonalChessRule::IsInCheckB(const Piece b[9][9], Side side) const {
    int gr = -1, gc = -1;
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (b[r][c].alive && b[r][c].type == PieceType::GENERAL && b[r][c].side == side) {
                gr = r;
                gc = c;
                break;
            }
        }
    }
    if (gr < 0 || gc < 0) return false;

    Side opp = (side == Side::RED) ? Side::BLACK : Side::RED;
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (b[r][c].alive && b[r][c].side == opp) {
                if (IsValidMove(b, r, c, gr, gc)) return true;
            }
        }
    }
    return false;
}

bool DiagonalChessRule::WouldBeInCheck(const Piece b[9][9], int fromR, int fromC, int toR, int toC, Side side) const {
    Piece tempBoard[9][9];
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            tempBoard[r][c] = b[r][c];
        }
    }
    tempBoard[toR][toC] = tempBoard[fromR][fromC];
    tempBoard[fromR][fromC] = {PieceType::NONE, Side::RED, false};
    return IsInCheckB(tempBoard, side);
}

bool DiagonalChessRule::WouldBeInCheckB(const Piece b[9][9], int fromR, int fromC, int toR, int toC, Side side) const {
    Piece tempBoard[9][9];
    for (int r = 0; r < 9; r++)
        for (int c = 0; c < 9; c++)
            tempBoard[r][c] = b[r][c];
    tempBoard[toR][toC] = tempBoard[fromR][fromC];
    tempBoard[fromR][fromC] = {PieceType::NONE, Side::RED, false};
    return IsInCheckB(tempBoard, side);
}

bool DiagonalChessRule::HasLegalMoves(const Piece b[9][9], Side side) const {
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (b[r][c].alive && b[r][c].side == side) {
                auto moves = GetValidMoves(b, r, c);
                if (!moves.empty()) return true;
            }
        }
    }
    return false;
}

bool DiagonalChessRule::IsCheckmate(const Piece b[9][9], Side side) const {
    if (!IsInCheck(b, side)) return false;
    return !HasLegalMoves(b, side);
}

bool DiagonalChessRule::HasInsufficientMaterial(const Piece b[9][9]) const {
    int redPieces = 0, blackPieces = 0;
    bool redHasAttack = false, blackHasAttack = false;

    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (!b[r][c].alive) continue;
            if (b[r][c].side == Side::RED) {
                redPieces++;
                if (b[r][c].type != PieceType::GENERAL &&
                    b[r][c].type != PieceType::ADVISOR) {
                    redHasAttack = true;
                }
            } else {
                blackPieces++;
                if (b[r][c].type != PieceType::GENERAL &&
                    b[r][c].type != PieceType::ADVISOR) {
                    blackHasAttack = true;
                }
            }
        }
    }

    if (redPieces <= 1 && blackPieces <= 1) return true;
    if (!redHasAttack && !blackHasAttack) return true;
    return false;
}

bool DiagonalChessRule::IsGameOver(const Piece b[9][9], Side currentTurn, bool& isDraw, Side& winner) const {
    if (HasInsufficientMaterial(b)) {
        isDraw = true;
        return true;
    }

    if (!HasLegalMoves(b, currentTurn)) {
        if (IsInCheck(b, currentTurn)) {
            winner = (currentTurn == Side::RED) ? Side::BLACK : Side::RED;
            isDraw = false;
        } else {
            isDraw = true;
        }
        return true;
    }

    return false;
}
