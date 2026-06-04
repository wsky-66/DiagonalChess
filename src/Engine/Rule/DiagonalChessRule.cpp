#include "Engine/Rule/DiagonalChessRule.h"
#include <cmath>
#include <algorithm>

static const DiagonalChessBoard* CastBoard(const Board& b) {
    return dynamic_cast<const DiagonalChessBoard*>(&b);
}

bool DiagonalChessRule::IsValidMove(const Board& b, int fr, int fc, int tr, int tc) const {
    auto* db = CastBoard(b);
    return db ? IsValidMoveRaw(*db, fr, fc, tr, tc) : false;
}

bool DiagonalChessRule::IsGameOver(const Board& b, Side side, bool& isDraw, Side& winner) const {
    auto* db = CastBoard(b);
    return db ? IsGameOverRaw(*db, side, isDraw, winner) : false;
}

bool DiagonalChessRule::IsInCheck(const Board& b, Side side) const {
    auto* db = CastBoard(b);
    return db ? IsInCheckRaw(*db, static_cast<int>(side)) : false;
}

bool DiagonalChessRule::HasLegalMoves(const Board& b, Side side) const {
    auto* db = CastBoard(b);
    return db ? HasLegalMovesRaw(*db, static_cast<int>(side)) : false;
}

bool DiagonalChessRule::IsValidMoveRaw(const DiagonalChessBoard& b, int fr, int fc, int tr, int tc) const {
    if (!b.IsInside(fr, fc) || !b.IsInside(tr, tc)) return false;
    if (!b.IsOccupied(fr, fc)) return false;

    const auto& piece = b.At(fr, fc);
    if (!piece.IsAlive()) return false;
    if (b.IsOccupied(tr, tc) && b.At(tr, tc).GetSide() == piece.GetSide()) return false;

    switch (piece.GetDType()) {
        case DChessPieceType::CHARIOT:  return CanChariotMove(b, fr, fc, tr, tc);
        case DChessPieceType::HORSE:    return CanHorseMove(b, fr, fc, tr, tc);
        case DChessPieceType::ELEPHANT: return CanElephantMove(b, fr, fc, tr, tc);
        case DChessPieceType::ADVISOR:  return CanAdvisorMove(b, fr, fc, tr, tc);
        case DChessPieceType::GENERAL:  return CanGeneralMove(b, fr, fc, tr, tc);
        case DChessPieceType::CANNON:   return CanCannonMove(b, fr, fc, tr, tc);
        case DChessPieceType::SOLDIER:  return CanSoldierMove(b, fr, fc, tr, tc, piece.GetSide());
        default: return false;
    }
}

bool DiagonalChessRule::CanChariotMove(const DiagonalChessBoard& b, int fr, int fc, int tr, int tc) const {
    if (fr != tr && fc != tc) return false;
    return CountPiecesBetween(b, fr, fc, tr, tc) == 0;
}

bool DiagonalChessRule::CanHorseMove(const DiagonalChessBoard& b, int fr, int fc, int tr, int tc) const {
    int dr = std::abs(tr - fr);
    int dc = std::abs(tc - fc);
    if (!((dr == 2 && dc == 1) || (dr == 1 && dc == 2))) return false;
    return !IsBlockedHorse(b, fr, fc, tr, tc);
}

bool DiagonalChessRule::IsBlockedHorse(const DiagonalChessBoard& b, int fr, int fc, int tr, int tc) const {
    int dr = tr - fr;
    int dc = tc - fc;
    if (std::abs(dr) == 2) {
        int blockR = fr + (dr > 0 ? 1 : -1);
        return b.IsOccupied(blockR, fc) && b.At(blockR, fc).IsAlive();
    }
    int blockC = fc + (dc > 0 ? 1 : -1);
    return b.IsOccupied(fr, blockC) && b.At(fr, blockC).IsAlive();
}

bool DiagonalChessRule::CanElephantMove(const DiagonalChessBoard& b, int fr, int fc, int tr, int tc) const {
    int dr = std::abs(tr - fr);
    int dc = std::abs(tc - fc);
    if (dr != 2 || dc != 2) return false;
    return !IsBlockedElephant(b, fr, fc, tr, tc);
}

bool DiagonalChessRule::IsBlockedElephant(const DiagonalChessBoard& b, int fr, int fc, int tr, int tc) const {
    int midR = (fr + tr) / 2;
    int midC = (fc + tc) / 2;
    return b.IsOccupied(midR, midC) && b.At(midR, midC).IsAlive();
}

bool DiagonalChessRule::CanAdvisorMove(const DiagonalChessBoard& b, int fr, int fc, int tr, int tc) const {
    if (!IsInPalace(tr, tc, b.At(fr, fc).GetSide())) return false;
    int dr = std::abs(tr - fr);
    int dc = std::abs(tc - fc);
    return (dr <= 1 && dc <= 1 && (dr + dc > 0));
}

bool DiagonalChessRule::CanGeneralMove(const DiagonalChessBoard& b, int fr, int fc, int tr, int tc) const {
    if (!IsInPalace(tr, tc, b.At(fr, fc).GetSide())) return false;
    int dr = std::abs(tr - fr);
    int dc = std::abs(tc - fc);
    return (dr + dc == 1);
}

bool DiagonalChessRule::CanCannonMove(const DiagonalChessBoard& b, int fr, int fc, int tr, int tc) const {
    if (fr != tr && fc != tc) return false;
    int count = CountPiecesBetween(b, fr, fc, tr, tc);
    if (b.IsOccupied(tr, tc)) return count == 1;
    return count == 0;
}

int DiagonalChessRule::CountPiecesBetween(const DiagonalChessBoard& b, int fr, int fc, int tr, int tc) const {
    int count = 0;
    if (fr == tr) {
        int minC = std::min(fc, tc), maxC = std::max(fc, tc);
        for (int c = minC + 1; c < maxC; c++)
            if (b.IsOccupied(fr, c)) count++;
    } else if (fc == tc) {
        int minR = std::min(fr, tr), maxR = std::max(fr, tr);
        for (int r = minR + 1; r < maxR; r++)
            if (b.IsOccupied(r, fc)) count++;
    }
    return count;
}

bool DiagonalChessRule::CanSoldierMove(const DiagonalChessBoard& b, int fr, int fc, int tr, int tc, int side) const {
    int dr = tr - fr;
    int dc = tc - fc;
    if (std::abs(dr) + std::abs(dc) != 1) return false;
    if (side == 0) return (dr == 1 && dc == 0) || (dr == 0 && dc == -1);
    return (dr == -1 && dc == 0) || (dr == 0 && dc == 1);
}

std::vector<sf::Vector2i> DiagonalChessRule::GetValidMoves(const DiagonalChessBoard& b, int r, int c) const {
    std::vector<sf::Vector2i> moves;
    if (!b.IsInside(r, c) || !b.IsOccupied(r, c)) return moves;

    int side = b.At(r, c).GetSide();
    for (int tr = 0; tr < 9; tr++) {
        for (int tc = 0; tc < 9; tc++) {
            if (tr == r && tc == c) continue;
            if (b.IsOccupied(tr, tc) && b.At(tr, tc).GetSide() == side) continue;
            if (IsValidMoveRaw(b, r, c, tr, tc)) {
                if (!WouldBeInCheck(b, r, c, tr, tc, side)) {
                    moves.push_back(sf::Vector2i(tr, tc));
                }
            }
        }
    }
    return moves;
}

bool DiagonalChessRule::IsInCheckRaw(const DiagonalChessBoard& b, int side) const {
    int gr = -1, gc = -1;
    for (int r = 0; r < 9 && gr < 0; r++)
        for (int c = 0; c < 9 && gr < 0; c++)
            if (b.IsOccupied(r, c) && b.At(r, c).GetDType() == DChessPieceType::GENERAL && b.At(r, c).GetSide() == side)
                { gr = r; gc = c; }
    if (gr < 0) return false;

    int opp = 1 - side;
    for (int r = 0; r < 9; r++)
        for (int c = 0; c < 9; c++)
            if (b.IsOccupied(r, c) && b.At(r, c).GetSide() == opp)
                if (IsValidMoveRaw(b, r, c, gr, gc)) return true;
    return false;
}

bool DiagonalChessRule::IsInPalace(int r, int c, int side) const {
    if (side == 0) return r >= 0 && r <= 2 && c >= 6 && c <= 8;
    return r >= 6 && r <= 8 && c >= 0 && c <= 2;
}

bool DiagonalChessRule::WouldBeInCheck(const DiagonalChessBoard& b, int fr, int fc, int tr, int tc, int side) const {
    DiagonalChessBoard temp;
    b.CopyTo(temp);
    temp.At(tr, tc) = temp.At(fr, fc);
    temp.OccupiedCell(tr, tc);
    temp.ClearCell(fr, fc);
    return IsInCheckRaw(temp, side);
}

bool DiagonalChessRule::HasLegalMovesRaw(const DiagonalChessBoard& b, int side) const {
    for (int r = 0; r < 9; r++)
        for (int c = 0; c < 9; c++)
            if (b.IsOccupied(r, c) && b.At(r, c).GetSide() == side)
                if (!GetValidMoves(b, r, c).empty()) return true;
    return false;
}

bool DiagonalChessRule::IsCheckmate(const DiagonalChessBoard& b, int side) const {
    return IsInCheckRaw(b, side) && !HasLegalMovesRaw(b, side);
}

bool DiagonalChessRule::HasInsufficientMaterial(const DiagonalChessBoard& b) const {
    bool redHasAttack = false, blackHasAttack = false;
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (!b.IsOccupied(r, c)) continue;
            auto t = b.At(r, c).GetDType();
            if (t == DChessPieceType::GENERAL || t == DChessPieceType::ADVISOR) continue;
            if (b.At(r, c).GetSide() == 0) redHasAttack = true;
            else blackHasAttack = true;
        }
    }
    return !redHasAttack && !blackHasAttack;
}

bool DiagonalChessRule::SideHasNoAttack(const DiagonalChessBoard& b, int side) const {
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (!b.IsOccupied(r, c) || b.At(r, c).GetSide() != side) continue;
            auto t = b.At(r, c).GetDType();
            if (t != DChessPieceType::GENERAL && t != DChessPieceType::ADVISOR) return false;
        }
    }
    return true;
}

bool DiagonalChessRule::IsGameOverRaw(const DiagonalChessBoard& b, Side side, bool& isDraw, Side& winner) const {
    int s = static_cast<int>(side);
    if (HasInsufficientMaterial(b)) {
        isDraw = true;
        winner = Side::RED;
        return true;
    }
    if (SideHasNoAttack(b, s)) {
        isDraw = false;
        winner = (side == Side::RED) ? Side::BLACK : Side::RED;
        return true;
    }
    if (!HasLegalMovesRaw(b, s)) {
        isDraw = false;
        winner = (side == Side::RED) ? Side::BLACK : Side::RED;
        return true;
    }
    return false;
}
