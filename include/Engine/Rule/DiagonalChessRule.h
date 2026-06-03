#pragma once
#include "Engine/Rule.h"
#include "Engine/Board.h"
#include "Engine/Board/DiagonalChessBoard.h"
#include "Engine/Piece.h"
#include <vector>

class DiagonalChessRule : public Rule {
public:
    bool IsValidMove(const Board& b, int fr, int fc, int tr, int tc) const override;
    bool IsGameOver(const Board& b, Side side, bool& isDraw, Side& winner) const override;
    bool IsInCheck(const Board& b, Side side) const override;
    bool HasLegalMoves(const Board& b, Side side) const override;

    std::vector<sf::Vector2i> GetValidMoves(const DiagonalChessBoard& b, int r, int c) const;
    bool IsValidMoveRaw(const DiagonalChessBoard& b, int fr, int fc, int tr, int tc) const;
    bool IsInCheckRaw(const DiagonalChessBoard& b, int side) const;
    bool HasLegalMovesRaw(const DiagonalChessBoard& b, int side) const;
    bool IsGameOverRaw(const DiagonalChessBoard& b, Side side, bool& isDraw, Side& winner) const;
    bool WouldBeInCheck(const DiagonalChessBoard& b, int fr, int fc, int tr, int tc, int side) const;
    bool IsCheckmate(const DiagonalChessBoard& b, int side) const;
    bool HasInsufficientMaterial(const DiagonalChessBoard& b) const;
    bool IsInPalace(int r, int c, int side) const;

private:
    bool CanChariotMove(const DiagonalChessBoard& b, int fr, int fc, int tr, int tc) const;
    bool CanHorseMove(const DiagonalChessBoard& b, int fr, int fc, int tr, int tc) const;
    bool CanElephantMove(const DiagonalChessBoard& b, int fr, int fc, int tr, int tc) const;
    bool CanAdvisorMove(const DiagonalChessBoard& b, int fr, int fc, int tr, int tc) const;
    bool CanGeneralMove(const DiagonalChessBoard& b, int fr, int fc, int tr, int tc) const;
    bool CanCannonMove(const DiagonalChessBoard& b, int fr, int fc, int tr, int tc) const;
    bool CanSoldierMove(const DiagonalChessBoard& b, int fr, int fc, int tr, int tc, int side) const;

    bool IsBlockedHorse(const DiagonalChessBoard& b, int fr, int fc, int tr, int tc) const;
    bool IsBlockedElephant(const DiagonalChessBoard& b, int fr, int fc, int tr, int tc) const;
    int CountPiecesBetween(const DiagonalChessBoard& b, int fr, int fc, int tr, int tc) const;
};
