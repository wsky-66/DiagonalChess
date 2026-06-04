#pragma once

// ===================================================================
// Rule.h — 规则类的抽象基类
// 所有棋类游戏（对角象棋、五子棋等）的规则都继承自此基类
// 定义了四个核心方法：是否合法走法、游戏是否结束、是否将军、是否有合法走法
// ===================================================================

#include "Engine/Common.h"           // 引入 Side, Board 等通用类型

class Board;                         // 前置声明

class Rule {
public:
    virtual ~Rule() = default;       // 虚析构函数

    // —— 纯虚函数（每个游戏必须实现）——

    // 判断从 (fr,fc) 走到 (tr,tc) 是否合法（不考虑将军，仅看走法规则）
    // 参数：棋盘对象、起始行列、目标行列
    // 返回：true=合法, false=不合法
    virtual bool IsValidMove(const Board& b, int fr, int fc, int tr, int tc) const = 0;

    // 判断游戏是否结束
    // 参数：棋盘、当前方、输出-是否平局、输出-胜者
    // 返回：true=游戏结束, false=继续
    virtual bool IsGameOver(const Board& b, Side side, bool& isDraw, Side& winner) const = 0;

    // 判断指定阵营是否被将军
    // 返回：true=当前方正在被将军
    virtual bool IsInCheck(const Board& b, Side side) const = 0;

    // 判断指定阵营是否有合法走法（能走出将军或能吃子）
    // 返回：true=有合法走法, false=无合法走法（困毙或绝杀）
    virtual bool HasLegalMoves(const Board& b, Side side) const = 0;
};
