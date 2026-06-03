#include "Engine/Common.h"
#include <string>

std::wstring GetPieceName(PieceType type, Side side) {
    if (side == Side::RED) {
        switch (type) {
            case PieceType::GENERAL: return L"\u5e05";
            case PieceType::ADVISOR: return L"\u4ed5";
            case PieceType::ELEPHANT: return L"\u76f8";
            case PieceType::HORSE: return L"\u9a6c";
            case PieceType::CHARIOT: return L"\u8f66";
            case PieceType::CANNON: return L"\u70ae";
            case PieceType::SOLDIER: return L"\u5175";
            default: return L"?";
        }
    } else {
        switch (type) {
            case PieceType::GENERAL: return L"\u5c06";
            case PieceType::ADVISOR: return L"\u58eb";
            case PieceType::ELEPHANT: return L"\u8c61";
            case PieceType::HORSE: return L"\u9a6c";
            case PieceType::CHARIOT: return L"\u8f66";
            case PieceType::CANNON: return L"\u70ae";
            case PieceType::SOLDIER: return L"\u5352";
            default: return L"?";
        }
    }
}
