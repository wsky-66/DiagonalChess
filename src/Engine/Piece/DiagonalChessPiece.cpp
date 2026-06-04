// ===================================================================
// DiagonalChessPiece.cpp — 对角象棋棋子类的实现
// 主要实现：棋子符号映射（汉字）、棋子价值定义
// ===================================================================

#include "Engine/Piece/DiagonalChessPiece.h"

// ==================== DiagonalChessPiece 成员函数 ====================

// 获取棋子显示的汉字符号（如红帅、黑将、车、马等）
std::wstring DiagonalChessPiece::GetSymbol() const {
    return DChessGetSymbol(type, side);
}

// 获取棋子价值（用于 AI 评估，数值越大越重要）
int DiagonalChessPiece::GetValue() const {
    return DChessGetValue(type);
}

// ==================== 全局辅助函数 ====================

// 根据棋子类型和阵营返回对应的中文符号
// side=0=红方, side=1=黑方
// 红方将="帅", 黑方将="将"; 红方士="仕", 黑方士="士"; 红方象="相", 黑方象="象"
// 红方兵="兵", 黑方兵="卒"; 马/车/炮 红黑相同
std::wstring DChessGetSymbol(DChessPieceType type, int side) {
    bool isRed = (side == 0);
    switch (type) {
        case DChessPieceType::GENERAL:  return isRed ? L"\u5E05" : L"\u5C06"; // 帅 / 将
        case DChessPieceType::ADVISOR:  return isRed ? L"\u4ED5" : L"\u58EB"; // 仕 / 士
        case DChessPieceType::ELEPHANT: return isRed ? L"\u76F8" : L"\u8C61"; // 相 / 象
        case DChessPieceType::HORSE:    return L"\u9A6C";                     // 马
        case DChessPieceType::CHARIOT:  return L"\u8F66";                     // 车
        case DChessPieceType::CANNON:   return L"\u70AE";                     // 炮
        case DChessPieceType::SOLDIER:  return isRed ? L"\u5175" : L"\u5352"; // 兵 / 卒
        default: return L"?";                                                 // 未知类型
    }
}

// 根据棋子类型返回基础价值（用于 AI 的局面评估）
// 将=10000（最高，绝不能被吃）, 车=900, 炮=450, 马=400,
// 象/士=200（防御型棋子）, 兵=100（最弱但位置加分可提升）
int DChessGetValue(DChessPieceType type) {
    switch (type) {
        case DChessPieceType::GENERAL:  return 10000;   // 将/帅：最高价值
        case DChessPieceType::CHARIOT:  return 900;     // 车：强攻棋子
        case DChessPieceType::CANNON:   return 450;     // 炮：需炮架
        case DChessPieceType::HORSE:    return 400;     // 马：灵活
        case DChessPieceType::ELEPHANT: return 200;     // 象：防御
        case DChessPieceType::ADVISOR:  return 200;     // 士：护卫
        case DChessPieceType::SOLDIER:  return 100;     // 兵：数量多但弱
        default: return 0;
    }
}
