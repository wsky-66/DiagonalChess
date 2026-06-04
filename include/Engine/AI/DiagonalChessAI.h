#pragma once

// ===================================================================
// DiagonalChessAI.h — 对角象棋 AI
// 使用 Minimax（极小化极大）算法 + Alpha-Beta 剪枝优化
// 搜索深度：简单=2层, 中等=3层, 困难=4层
// ===================================================================

#include "Engine/AI/AIPlayer.h"
#include "Engine/Piece/DiagonalChessPiece.h"
#include "Engine/Rule.h"
#include "Engine/Rule/DiagonalChessRule.h"
#include "Engine/Board/DiagonalChessBoard.h"

class DiagonalChessAI : public AIPlayer {
public:
    DiagonalChessAI();

    // AI 思考：给定棋盘和己方阵营，返回最优走法
    // 参数：棋盘、AI的阵营(0=红,1=黑)、规则对象
    // 返回：最优走法的起止坐标和评分
    AIMove Think(const DiagonalChessBoard& orig, int aiSide, const DiagonalChessRule& rule);

    // 设置 AI 难度并自动调整搜索深度
    void SetDifficulty(AIDifficulty d) override { difficulty = d; UpdateDepth(); }
    AIDifficulty GetDifficulty() const { return difficulty; }

private:
    // 根据难度更新搜索深度（简单=2, 中等=3, 困难=4）
    void UpdateDepth();

    // 生成某个阵营在当前棋盘上的所有合法走法
    std::vector<AIMove> GenerateAllMoves(const DiagonalChessBoard& b, int side,
                                          const DiagonalChessRule& rule) const;

    // 局面评估函数：给当前棋盘打分（正值对红方有利，负值对黑方有利）
    int Evaluate(const DiagonalChessBoard& b) const;

    // Minimax 搜索（带 Alpha-Beta 剪枝）
    // alpha: 当前已知最大值下界, beta: 当前已知最小值上界
    // maximizing: 当前层是否在最大化分数
    int Minimax(DiagonalChessBoard& b, int depth, int alpha, int beta,
                bool maximizing, int aiSide, const DiagonalChessRule& rule);

    // 棋子基础价值（将=10000, 车=900, 马=400, 炮=450, 象=200, 士=200, 兵=100）
    int GetPieceValue(DChessPieceType t) const;

    // 棋子位置附加分（如：兵越靠近对方底线分数越高，马越靠近中心分数越高）
    int GetPositionBonus(DChessPieceType t, int r, int c, int side) const;

    AIDifficulty difficulty;          // 当前难度
    int aiDepth;                      // 当前搜索深度
};
