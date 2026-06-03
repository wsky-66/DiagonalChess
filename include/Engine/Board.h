#pragma once
#include "Engine/Piece.h"
#include <memory>
#include <functional>

class Board {
public:
    virtual ~Board() = default;

    virtual int GetRows() const = 0;
    virtual int GetCols() const = 0;

    bool IsInside(int r, int c) const {
        return r >= 0 && r < GetRows() && c >= 0 && c < GetCols();
    }

    virtual Piece* GetPiece(int r, int c) = 0;
    virtual const Piece* GetPiece(int r, int c) const = 0;

    virtual void SetPiece(int r, int c, std::unique_ptr<Piece> p) = 0;
    virtual std::unique_ptr<Piece> TakePiece(int r, int c) = 0;

    virtual void Clear() = 0;
    virtual void Reset() = 0;

    virtual void ForEachPiece(std::function<void(Piece&, int r, int c)> fn) = 0;
};
