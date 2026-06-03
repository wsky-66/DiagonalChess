#pragma once
#include <string>
#include <memory>

class Board;

class Piece {
public:
    Piece(int s = 0) : side(s), alive(true) {}
    virtual ~Piece() = default;

    int GetSide() const { return side; }
    bool IsAlive() const { return alive; }
    void SetAlive(bool a) { alive = a; }
    bool IsSameSide(const Piece& other) const { return side == other.side; }

    virtual std::wstring GetSymbol() const = 0;
    virtual int GetType() const = 0;
    virtual int GetValue() const { return 0; }
    virtual std::unique_ptr<Piece> Clone() const = 0;

protected:
    int side;
    bool alive;
};

enum class DChessPieceType { NONE, CHARIOT, HORSE, ELEPHANT, ADVISOR, GENERAL, CANNON, SOLDIER };

class DiagonalChessPiece : public Piece {
public:
    DiagonalChessPiece(DChessPieceType t = DChessPieceType::NONE, int s = 0)
        : Piece(s), type(t) {}

    DChessPieceType GetDType() const { return type; }
    void SetType(DChessPieceType t) { type = t; }

    int GetType() const override { return static_cast<int>(type); }
    std::wstring GetSymbol() const override;
    int GetValue() const override;
    std::unique_ptr<Piece> Clone() const override {
        return std::make_unique<DiagonalChessPiece>(*this);
    }

private:
    DChessPieceType type;
};

std::wstring DChessGetSymbol(DChessPieceType type, int side);
int DChessGetValue(DChessPieceType type);
