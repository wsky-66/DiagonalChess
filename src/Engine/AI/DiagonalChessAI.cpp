// ===================================================================
// DiagonalChessAI.cpp — 对角象棋 AI 的实现
// 算法：Minimax（极小化极大搜索）+ Alpha-Beta 剪枝
// 原理：假设双方都会选择对自己最有利的走法
//   - AI 方想要最大化局面评估分
//   - 对手想要最小化局面评估分
// Alpha-Beta 剪枝：在搜索过程中剪掉不可能比当前已知更好/更差的分支
// ===================================================================

#include "Engine/AI/DiagonalChessAI.h"
#include <cmath>
#include <algorithm>

// 构造函数：默认难度中等，搜索深度 3 层
DiagonalChessAI::DiagonalChessAI() : difficulty(AIDifficulty::MEDIUM), aiDepth(3) {}

// 根据难度更新搜索深度
// 简单=2层（能下棋但目光短浅）, 中等=3层, 困难=4层（较聪明但稍慢）
void DiagonalChessAI::UpdateDepth() {
    switch (difficulty) {
        case AIDifficulty::EASY:   aiDepth = 2; break;
        case AIDifficulty::MEDIUM: aiDepth = 3; break;
        case AIDifficulty::HARD:   aiDepth = 4; break;
    }
}

// ==================== AI 主入口：Think ====================
// 输入：原始棋盘、AI的阵营(0=红,1=黑)、规则对象
// 输出：评分最高的走法
// 红方 AI 选择 eval 最大的走法，黑方 AI 选择 eval 最小的走法
AIMove DiagonalChessAI::Think(const DiagonalChessBoard& orig, int aiSide,
                               const DiagonalChessRule& rule) {
    DiagonalChessBoard temp;                         // 工作棋盘（不破坏原始棋盘）
    orig.CopyTo(temp);
    auto moves = GenerateAllMoves(temp, aiSide, rule);
    if (moves.empty()) return {-1, -1, -1, -1, 0};  // 无合法走法 → 返回无效走法

    // 红方 AI 追求分数最大值，黑方 AI 追求分数最小值
    int bestScore = (aiSide == 0) ? -999999 : 999999;
    AIMove best = moves[0];

    for (auto& m : moves) {
        // 在临时棋盘上模拟这一步走棋
        auto captured = temp.At(m.toR, m.toC);       // 备份被吃掉的棋子
        auto moved = temp.At(m.fromR, m.fromC);       // 备份移动的棋子
        temp.At(m.toR, m.toC) = moved;                // 把棋子放到目标位置
        temp.OccupiedCell(m.toR, m.toC);
        temp.ClearCell(m.fromR, m.fromC);             // 清空原位置

        // Minimax 搜索：从对手的角度开始（maximizing = 非AI方）
        int score = Minimax(temp, aiDepth - 1, -999999, 999999,
                            aiSide != 0,              // 红方AI→对手黑方要最小化→maximizing=false
                            aiSide, rule);

        // 恢复棋盘（撤销模拟走棋）
        temp.At(m.fromR, m.fromC) = moved;
        temp.OccupiedCell(m.fromR, m.fromC);
        if (captured.IsAlive()) { temp.At(m.toR, m.toC) = captured; temp.OccupiedCell(m.toR, m.toC); }
        else { temp.ClearCell(m.toR, m.toC); }

        // 更新最佳走法（红方取最大，黑方取最小）
        if (aiSide == 0) { if (score > bestScore) { bestScore = score; best = m; } }
        else            { if (score < bestScore) { bestScore = score; best = m; } }
    }
    best.score = bestScore;
    return best;
}

// ==================== 生成所有合法走法 ====================
// 遍历棋盘上所有己方棋子，为每个棋子生成所有合法目标位置
std::vector<AIMove> DiagonalChessAI::GenerateAllMoves(
    const DiagonalChessBoard& b, int side, const DiagonalChessRule& rule) const {
    std::vector<AIMove> moves;
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (!b.IsOccupied(r, c) || b.At(r, c).GetSide() != side) continue;  // 只考虑己方棋子
            for (int tr = 0; tr < 9; tr++) {
                for (int tc = 0; tc < 9; tc++) {
                    if (tr == r && tc == c) continue;       // 不能走到自己的位置
                    if (b.IsOccupied(tr, tc) && b.At(tr, tc).GetSide() == side) continue; // 不能吃己方棋子
                    if (rule.IsValidMoveRaw(b, r, c, tr, tc)) {
                        if (rule.WouldBeInCheck(b, r, c, tr, tc, side)) continue; // 不能走出将军
                        moves.push_back({r, c, tr, tc, 0});
                    }
                }
            }
        }
    }
    return moves;
}

// ==================== 局面评估 ====================
// 给当前棋盘打分，正值对红方有利，负值对黑方有利
// 评分 = 所有棋子价值总和 + 位置奖励
// 如果某一方的将不存在，直接返回极值（-100000 或 +100000）
int DiagonalChessAI::Evaluate(const DiagonalChessBoard& b) const {
    bool redGen = false, blackGen = false;             // 双方将/帅存在标志
    int score = 0;
    bool redHasAttack = false, blackHasAttack = false;   // 双方是否有攻击力

    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (!b.IsOccupied(r, c)) continue;
            auto t = b.At(r, c).GetDType();

            // 检测将/帅是否存在
            if (t == DChessPieceType::GENERAL) {
                if (b.At(r, c).GetSide() == 0) redGen = true;
                else blackGen = true;
            }

            // 检测是否有攻击力（除了将和士，其他棋子都有攻击力）
            if (t != DChessPieceType::GENERAL && t != DChessPieceType::ADVISOR) {
                if (b.At(r, c).GetSide() == 0) redHasAttack = true;
                else blackHasAttack = true;
            }

            // 累加棋子价值 + 位置奖励
            int val = GetPieceValue(t) + GetPositionBonus(t, r, c, b.At(r, c).GetSide());
            if (b.At(r, c).GetSide() == 0) score += val;   // 红方加分
            else score -= val;                              // 黑方减分
        }
    }

    // 极端情况判断
    if (!redGen) return -100000;                       // 红方没有帅 → 红方输
    if (!blackGen) return 100000;                      // 黑方没有将 → 红方赢
    if (!redHasAttack && !blackHasAttack) return 0;    // 双方都无攻击力 → 和棋
    if (!redHasAttack) return -50000;                   // 红方无攻击力 → 劣势
    if (!blackHasAttack) return 50000;                  // 黑方无攻击力 → 优势

    return score;
}

// ==================== Minimax 搜索（带 Alpha-Beta 剪枝）====================
// depth: 剩余搜索深度
// alpha: 当前已知的"最大值下界"（最大化方至少能拿到的分数）
// beta:  当前已知的"最小值上界"（最小化方最多能让对手拿到的分数）
// maximizing: true=当前层是最大化方, false=当前层是最小化方
// 剪枝条件：beta <= alpha → 无需继续搜索（该分支不会影响最终选择）
int DiagonalChessAI::Minimax(DiagonalChessBoard& b, int depth, int alpha, int beta,
                              bool maximizing, int aiSide, const DiagonalChessRule& rule) {
    if (depth == 0) return Evaluate(b);                // 到达叶子节点 → 评估局面

    int currentSide = maximizing ? 0 : 1;              // 当前走棋方
    auto moves = GenerateAllMoves(b, currentSide, rule);
    if (moves.empty()) return maximizing ? -100000 : 100000; // 无子可走 → 该方失败

    if (maximizing) {
        // 最大化层：寻找分数最高的走法
        int maxEval = -999999;
        for (const auto& m : moves) {
            // 模拟走棋
            auto captured = b.At(m.toR, m.toC);
            auto moved = b.At(m.fromR, m.fromC);
            b.At(m.toR, m.toC) = moved;
            b.OccupiedCell(m.toR, m.toC);
            b.ClearCell(m.fromR, m.fromC);

            int eval = Minimax(b, depth - 1, alpha, beta, false, aiSide, rule);

            // 撤销走棋
            b.At(m.fromR, m.fromC) = moved;
            b.OccupiedCell(m.fromR, m.fromC);
            if (captured.IsAlive()) { b.At(m.toR, m.toC) = captured; b.OccupiedCell(m.toR, m.toC); }
            else { b.ClearCell(m.toR, m.toC); }

            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);               // 更新下界
            if (beta <= alpha) break;                    // Alpha-Beta 剪枝
        }
        return maxEval;
    } else {
        // 最小化层：寻找分数最低的走法
        int minEval = 999999;
        for (const auto& m : moves) {
            auto captured = b.At(m.toR, m.toC);
            auto moved = b.At(m.fromR, m.fromC);
            b.At(m.toR, m.toC) = moved;
            b.OccupiedCell(m.toR, m.toC);
            b.ClearCell(m.fromR, m.fromC);

            int eval = Minimax(b, depth - 1, alpha, beta, true, aiSide, rule);

            b.At(m.fromR, m.fromC) = moved;
            b.OccupiedCell(m.fromR, m.fromC);
            if (captured.IsAlive()) { b.At(m.toR, m.toC) = captured; b.OccupiedCell(m.toR, m.toC); }
            else { b.ClearCell(m.toR, m.toC); }

            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);                 // 更新上界
            if (beta <= alpha) break;                    // Alpha-Beta 剪枝
        }
        return minEval;
    }
}

// 获取棋子基础价值（委托给全局函数）
int DiagonalChessAI::GetPieceValue(DChessPieceType t) const {
    return DChessGetValue(t);
}

// 获取棋子位置奖励分（鼓励好的走位）
// - 兵/卒：越靠近对方底线分数越高（促进进攻）
// - 马：靠近棋盘中心奖励 30 分（马在中心更灵活）
// - 车/炮：靠近中心有少量加分
int DiagonalChessAI::GetPositionBonus(DChessPieceType t, int r, int c, int side) const {
    int dist = std::abs(r - 4) + std::abs(c - 4);       // 曼哈顿距离到中心(4,4)
    switch (t) {
        case DChessPieceType::SOLDIER:
            // 兵向前（红方行号增大，黑方行号减小）加分
            return (side == 0) ? r * 10 : (8 - r) * 10;
        case DChessPieceType::HORSE:
            return (dist <= 3) ? 30 : 0;                 // 距离中心 3 步以内加分
        case DChessPieceType::CHARIOT:
            return (dist <= 4) ? 20 : 0;
        case DChessPieceType::CANNON:
            return (dist <= 4) ? 10 : 0;
        default:
            return 0;                                    // 将/士/象 不需要位置分
    }
}
