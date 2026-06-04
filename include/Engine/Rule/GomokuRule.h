#pragma once
#include "Engine/Rule.h"
#include "Engine/Board.h"
#include "Engine/Board/GomokuBoard.h"

// 五子棋规则：落子合法性、胜负判定、平局判定
class GomokuRule : public Rule {
public:
    // —— Rule 抽象接口 ——
    // IsValidMove: 五子棋无"移动"概念，落子时 fr==fc==-1 表示新落子，
    //              tr/tc 为目标位置
    bool IsValidMove(const Board& b, int fr, int fc, int tr, int tc) const override;

    // IsGameOver: 检测 side 落子后是否获胜 / 棋盘是否已满（平局）
    bool IsGameOver(const Board& b, Side side, bool& isDraw, Side& winner) const override;

    // 五子棋无"将军"概念
    bool IsInCheck(const Board& b, Side side) const override { return false; }

    // 是否有合法落子处（即还有空位）
    bool HasLegalMoves(const Board& b, Side side) const override;

    // —— Gomoku 专用接口 ——
    // 落子是否合法：位置在棋盘内且为空
    bool IsValidPlace(const GomokuBoard& b, int r, int c) const;

    // 棋盘是否已满
    bool IsBoardFull(const GomokuBoard& b) const;

    // 检测 (r,c) 落子后 side 是否五连获胜
    bool CheckWin(const GomokuBoard& b, int r, int c, int side) const;

    // 检测 side 在当前棋盘上是否能获胜（遍历所有空位）
    bool CanWin(const GomokuBoard& b, int side) const;

private:
    // 从 (r,c) 出发，沿 (dr,dc) 方向统计连续相同棋子数（不含自身）
    int CountDir(const GomokuBoard& b, int r, int c, int dr, int dc, int side) const;
};
