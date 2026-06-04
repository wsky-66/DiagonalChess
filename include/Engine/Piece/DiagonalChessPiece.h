#pragma once

// ===================================================================
// DiagonalChessPiece.h — 对角象棋棋子类
// 继承自 Piece，增加对角象棋特有的类型属性
// 包含棋子类型枚举、棋子价值和符号映射
// ===================================================================

#include "Engine/Piece.h"
#include <string>
#include <memory>

// —— 对角象棋棋子类型枚举 ——
// NONE=无, CHARIOT=车, HORSE=马, ELEPHANT=象, ADVISOR=士,
// GENERAL=将/帅, CANNON=炮, SOLDIER=兵/卒
enum class DChessPieceType { NONE, CHARIOT, HORSE, ELEPHANT, ADVISOR, GENERAL, CANNON, SOLDIER };

// ==================== 对角象棋棋子类 ====================
class DiagonalChessPiece : public Piece {
public:
    // 构造函数：指定棋子类型和阵营
    DiagonalChessPiece(DChessPieceType t = DChessPieceType::NONE, int s = 0)
        : Piece(s), type(t) {}

    DChessPieceType GetDType() const { return type; }  // 获取对角象棋棋子类型
    void SetType(DChessPieceType t) { type = t; }       // 设置棋子类型

    // —— 实现基类虚函数 ——
    int GetType() const override { return static_cast<int>(type); }   // 返回类型编号
    std::wstring GetSymbol() const override;          // 返回棋子汉字符号（实现在.cpp中）
    int GetValue() const override;                     // 返回棋子价值（实现在.cpp中）
    std::unique_ptr<Piece> Clone() const override {    // 克隆：创建一个一模一样的棋子
        return std::make_unique<DiagonalChessPiece>(*this);
    }

private:
    DChessPieceType type;              // 棋子类型（车/马/象/士/将/炮/兵）
};

// —— 全局辅助函数 ——
// 根据棋子类型和阵营返回对应的汉字符号（如红方将="帅"，黑方将="将"）
std::wstring DChessGetSymbol(DChessPieceType type, int side);
// 根据棋子类型返回基础价值（将=10000, 车=900, 炮=450, 马=400, 象/士=200, 兵=100）
int DChessGetValue(DChessPieceType type);
