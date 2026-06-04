#pragma once

// ===================================================================
// DiagonalChessRule.h — 对角象棋规则类
// 实现象棋规则：每种棋子的走法、将军检测、胜负判定
// 棋盘坐标是 45 度斜放的，但走法逻辑不变（车走直线、马走日字等）
// ===================================================================

#include "Engine/Rule.h"
#include "Engine/Board.h"
#include "Engine/Board/DiagonalChessBoard.h"
#include "Engine/Piece/DiagonalChessPiece.h"
#include <vector>

class DiagonalChessRule : public Rule {
public:
    // —— 实现 Rule 基类虚函数（接受抽象 Board，内部转换为 DiagonalChessBoard）——
    bool IsValidMove(const Board& b, int fr, int fc, int tr, int tc) const override;
    bool IsGameOver(const Board& b, Side side, bool& isDraw, Side& winner) const override;
    bool IsInCheck(const Board& b, Side side) const override;
    bool HasLegalMoves(const Board& b, Side side) const override;

    // —— 对角象棋专用方法（直接接受 DiagonalChessBoard，效率更高）——

    // 获取某个棋子的所有合法走法列表（排除会让自己被将军的走法）
    std::vector<sf::Vector2i> GetValidMoves(const DiagonalChessBoard& b,
                                             int r, int c) const;

    // 判断走法是否合法（原始版，不检查会不会让自己被将军）
    bool IsValidMoveRaw(const DiagonalChessBoard& b, int fr, int fc,
                        int tr, int tc) const;

    // 判断某方是否被将军
    bool IsInCheckRaw(const DiagonalChessBoard& b, int side) const;

    // 判断某方是否有合法走法
    bool HasLegalMovesRaw(const DiagonalChessBoard& b, int side) const;

    // 判断游戏是否结束（含子力不足和棋判定）
    bool IsGameOverRaw(const DiagonalChessBoard& b, Side side,
                       bool& isDraw, Side& winner) const;

    // 判断某个走法会不会导致自己暴露将军（用于过滤非法走法）
    bool WouldBeInCheck(const DiagonalChessBoard& b, int fr, int fc,
                        int tr, int tc, int side) const;

    // 判断某方是否被绝杀（被将军 + 无合法走法）
    bool IsCheckmate(const DiagonalChessBoard& b, int side) const;

    // 判断是否"子力不足"和棋（双方都只剩将和士，无攻击力）
    bool HasInsufficientMaterial(const DiagonalChessBoard& b) const;

    // 判断某方是否完全无攻击力（只有将和士，无车马炮象兵）
    bool SideHasNoAttack(const DiagonalChessBoard& b, int side) const;

    // 判断某个坐标是否在九宫格内
    // 红方九宫：行 0~2, 列 6~8；黑方九宫：行 6~8, 列 0~2
    bool IsInPalace(int r, int c, int side) const;

private:
    // —— 各棋子的走法规则 ——
    bool CanChariotMove(const DiagonalChessBoard& b, int fr, int fc,
                        int tr, int tc) const;  // 车：横竖直线无阻挡
    bool CanHorseMove(const DiagonalChessBoard& b, int fr, int fc,
                      int tr, int tc) const;    // 马：日字，注意蹩马脚
    bool CanElephantMove(const DiagonalChessBoard& b, int fr, int fc,
                         int tr, int tc) const;  // 象：田字 2×2 对角，注意塞象眼
    bool CanAdvisorMove(const DiagonalChessBoard& b, int fr, int fc,
                        int tr, int tc) const;  // 士：八个斜方向一步，不出九宫
    bool CanGeneralMove(const DiagonalChessBoard& b, int fr, int fc,
                        int tr, int tc) const;  // 将：横竖一步，不出九宫
    bool CanCannonMove(const DiagonalChessBoard& b, int fr, int fc,
                       int tr, int tc) const;   // 炮：移动如车，吃子须隔一子（炮架）
    bool CanSoldierMove(const DiagonalChessBoard& b, int fr, int fc,
                        int tr, int tc, int side) const; // 兵：只能斜前一步，不可后退

    // —— 阻塞检测辅助 ——
    bool IsBlockedHorse(const DiagonalChessBoard& b, int fr, int fc,
                        int tr, int tc) const;  // 检测是否蹩马脚
    bool IsBlockedElephant(const DiagonalChessBoard& b, int fr, int fc,
                           int tr, int tc) const; // 检测是否塞象眼
    int CountPiecesBetween(const DiagonalChessBoard& b, int fr, int fc,
                           int tr, int tc) const; // 统计两点之间（直线）有几个棋子
};
