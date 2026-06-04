// ===================================================================
// GomokuPiece.cpp — 五子棋棋子类的实现
// 棋子只有黑(1)和白(2)两种，由 side 区分
// ===================================================================

#include "Engine/Piece/GomokuPiece.h"

// 构造函数：初始化阵营，默认存活
GomokuPiece::GomokuPiece(int s) : Piece(s) {}

// 五子棋不用文本符号，画圆点表示
std::wstring GomokuPiece::GetSymbol() const {
    return L"";
}

// side 即类型：1=黑子, 2=白子
int GomokuPiece::GetType() const {
    return side;
}

// 克隆：创建一个一模一样的棋子
std::unique_ptr<Piece> GomokuPiece::Clone() const {
    return std::make_unique<GomokuPiece>(*this);
}
