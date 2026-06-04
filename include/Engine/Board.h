#pragma once

// ===================================================================
// Board.h — 棋盘类的抽象基类
// 所有棋类游戏的棋盘都继承自此基类
// 提供统一的接口：获取行列数、获取/设置/移除棋子、清空、重置
// ===================================================================

#include "Engine/Piece.h"            // 棋子基类
#include <memory>                    // std::unique_ptr
#include <functional>                // std::function（回调函数）

class Board {
public:
    virtual ~Board() = default;

    // —— 棋盘基本属性（纯虚函数，子类必须实现）——
    virtual int GetRows() const = 0;    // 获取棋盘行数
    virtual int GetCols() const = 0;    // 获取棋盘列数

    // —— 边界检测 ——
    // 判断 (r,c) 是否在棋盘范围内
    bool IsInside(int r, int c) const {
        return r >= 0 && r < GetRows() && c >= 0 && c < GetCols();
    }

    // —— 棋子操作（纯虚函数）——
    virtual Piece* GetPiece(int r, int c) = 0;            // 获取棋子的可修改指针
    virtual const Piece* GetPiece(int r, int c) const = 0; // 获取棋子的只读指针

    // 在 (r,c) 放置一个棋子（转移所有权）
    virtual void SetPiece(int r, int c, std::unique_ptr<Piece> p) = 0;
    // 从 (r,c) 拿走棋子（返回所有权），该位置变空
    virtual std::unique_ptr<Piece> TakePiece(int r, int c) = 0;

    // —— 棋盘管理 ——
    virtual void Clear() = 0;        // 清空棋盘（所有位置变为空）
    virtual void Reset() = 0;        // 重置棋盘（清空后摆放初始棋子）

    // —— 遍历棋子 ——
    // 对棋盘上每一个存活的棋子调用回调函数 fn
    // 参数：棋子引用、行、列
    virtual void ForEachPiece(std::function<void(Piece&, int r, int c)> fn) = 0;
};
