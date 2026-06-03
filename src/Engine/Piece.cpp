#include "Engine/Piece.h"

std::wstring DiagonalChessPiece::GetSymbol() const {
    return DChessGetSymbol(type, side);
}

int DiagonalChessPiece::GetValue() const {
    return DChessGetValue(type);
}

std::wstring DChessGetSymbol(DChessPieceType type, int side) {
    bool isRed = (side == 0);
    switch (type) {
        case DChessPieceType::GENERAL:  return isRed ? L"\u5E05" : L"\u5C06";
        case DChessPieceType::ADVISOR:  return isRed ? L"\u4ED5" : L"\u58EB";
        case DChessPieceType::ELEPHANT: return isRed ? L"\u76F8" : L"\u8C61";
        case DChessPieceType::HORSE:    return L"\u9A6C";
        case DChessPieceType::CHARIOT:  return L"\u8F66";
        case DChessPieceType::CANNON:   return L"\u70AE";
        case DChessPieceType::SOLDIER:  return isRed ? L"\u5175" : L"\u5352";
        default: return L"?";
    }
}

int DChessGetValue(DChessPieceType type) {
    switch (type) {
        case DChessPieceType::GENERAL:  return 10000;
        case DChessPieceType::CHARIOT:  return 900;
        case DChessPieceType::CANNON:   return 450;
        case DChessPieceType::HORSE:    return 400;
        case DChessPieceType::ELEPHANT: return 200;
        case DChessPieceType::ADVISOR:  return 200;
        case DChessPieceType::SOLDIER:  return 100;
        default: return 0;
    }
}
