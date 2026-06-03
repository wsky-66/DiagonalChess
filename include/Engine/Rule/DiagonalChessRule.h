#pragma once
#include "Engine/Rule.h"
#include "Engine/Board.h"
#include "Engine/Board/DiagonalChessBoard.h"
#include <vector>

class DiagonalChessRule : public Rule {
public:
    bool IsValidMove(const Piece b[9][9], int fromR, int fromC, int toR, int toC) const override;
    bool IsGameOver(const Piece b[9][9], Side currentTurn, bool& isDraw, Side& winner) const override;
    bool IsInCheck(const Piece b[9][9], Side side) const override;
    bool HasLegalMoves(const Piece b[9][9], Side side) const override;

    std::vector<sf::Vector2i> GetValidMoves(const Piece b[9][9], int r, int c) const;
    bool WouldBeInCheck(const Piece b[9][9], int fromR, int fromC, int toR, int toC, Side side) const;
    bool IsCheckmate(const Piece b[9][9], Side side) const;
    bool HasInsufficientMaterial(const Piece b[9][9]) const;
    bool IsInPalace(int r, int c, Side side) const;

private:
    bool CanChariotMove(const Piece b[9][9], int fr, int fc, int tr, int tc) const;
    bool CanHorseMove(const Piece b[9][9], int fr, int fc, int tr, int tc) const;
    bool CanElephantMove(const Piece b[9][9], int fr, int fc, int tr, int tc) const;
    bool CanAdvisorMove(const Piece b[9][9], int fr, int fc, int tr, int tc) const;
    bool CanGeneralMove(const Piece b[9][9], int fr, int fc, int tr, int tc) const;
    bool CanCannonMove(const Piece b[9][9], int fr, int fc, int tr, int tc) const;
    bool CanSoldierMove(const Piece b[9][9], int fr, int fc, int tr, int tc, Side side) const;

    bool IsBlockedHorse(const Piece b[9][9], int fr, int fc, int tr, int tc) const;
    bool IsBlockedElephant(const Piece b[9][9], int fr, int fc, int tr, int tc) const;
    int CountPiecesBetween(const Piece b[9][9], int fr, int fc, int tr, int tc) const;

    bool IsInCheckB(const Piece b[9][9], Side side) const;
    bool WouldBeInCheckB(const Piece b[9][9], int fromR, int fromC, int toR, int toC, Side side) const;
};
