#pragma once

// ===================================================================
// Piece.h — 棋子基类
// 所有棋类游戏的棋子都继承自此基类
// 定义了棋子的基本属性和纯虚接口
// ===================================================================

#include <string>
#include <memory>                     // std::unique_ptr（智能指针）

class Board;                          // 前置声明，避免循环依赖

class Piece {
public:
    // 构造函数：初始化棋子的阵营（side=0红方, 1黑方），默认存活
    Piece(int s = 0) : side(s), alive(true) {}
    virtual ~Piece() = default;       // 虚析构函数（保证子类正确释放）

    // —— 基本状态查询 ——
    int GetSide() const { return side; }           // 获取阵营（0=红, 1=黑）
    bool IsAlive() const { return alive; }          // 是否存活
    void SetAlive(bool a) { alive = a; }            // 设置存活状态
    bool IsSameSide(const Piece& other) const {     // 判断是否同阵营
        return side == other.side;
    }

    // —— 纯虚函数（子类必须实现）——
    virtual std::wstring GetSymbol() const = 0;    // 获取棋子显示的符号（如 "帅","车","马"）
    virtual int GetType() const = 0;               // 获取棋子类型编号
    virtual int GetValue() const { return 0; }     // 获取棋子价值（AI评估用，默认0）
    virtual std::unique_ptr<Piece> Clone() const = 0; // 深拷贝（克隆）棋子

protected:
    int side;                          // 阵营：0=红方, 1=黑方
    bool alive;                        // 棋子是否存活
};
