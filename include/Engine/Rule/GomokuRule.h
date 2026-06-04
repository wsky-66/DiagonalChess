#pragma once

// ===================================================================
// GomokuRule.h — 五子棋规则类
// 实现五子棋的核心规则：落子合法性、五连胜负判定、平局判定
// 五子棋没有"将军"和"移动"概念，IsInCheck 始终返回 false
// ===================================================================

#include "Engine/Rule.h"
#include "Engine/Board.h"
#include "Engine/Board/GomokuBoard.h"

class GomokuRule : public Rule {
public:
    // —— 实现 Rule 抽象基类接口 ——

    // 判断落子是否合法：五子棋无"移动"概念，fr/fc 忽略（设为 -1），
    // 仅检查 tr/tc 是否在棋盘内且为空位
    bool IsValidMove(const Board& b, int fr, int fc, int tr, int tc) const override;

    // 判断游戏是否结束：某方五连获胜 或 棋盘已满（平局）
    bool IsGameOver(const Board& b, Side side, bool& isDraw, Side& winner) const override;

    // 五子棋没有将军概念，始终返回 false
    bool IsInCheck(const Board& b, Side side) const override { return false; }

    // 是否有合法落子处（即棋盘上是否还有空位）
    bool HasLegalMoves(const Board& b, Side side) const override;

    // —— 五子棋专用接口 ——

    // 判断在 (r,c) 落子是否合法（在棋盘内且为空位）
    bool IsValidPlace(const GomokuBoard& b, int r, int c) const;

    // 判断棋盘是否已满（所有 15×15 = 225 个位置都有棋子）
    bool IsBoardFull(const GomokuBoard& b) const;

    // 检测在 (r,c) 落子后，side 方是否形成五连
    // 同时检查四个方向：水平、垂直、主对角线、副对角线
    bool CheckWin(const GomokuBoard& b, int r, int c, int side) const;

    // 检测 side 方在当前棋盘上是否已有五连（遍历所有已有棋子）
    bool CanWin(const GomokuBoard& b, int side) const;

private:
    // 从 (r,c) 出发，沿方向 (dr,dc) 统计连续的相同棋子数（不含自身）
    // dr,dc 为方向向量，如 {0,1} 表示向右, {1,1} 表示右下
    int CountDir(const GomokuBoard& b, int r, int c, int dr, int dc, int side) const;
};
