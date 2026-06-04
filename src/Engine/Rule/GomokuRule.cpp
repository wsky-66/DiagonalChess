// ===================================================================
// GomokuRule.cpp — 五子棋规则的实现
// 核心功能：落子合法性判定、五连胜负检测、平局判定
// ===================================================================

#include "Engine/Rule/GomokuRule.h"

// 辅助函数：将抽象 Board 安全转换为 GomokuBoard
static const GomokuBoard* CastBoard(const Board& b) {
    return dynamic_cast<const GomokuBoard*>(&b);
}

// ==================== Rule 抽象接口实现 ====================

// 判断"走法"是否合法
// 五子棋没有"移动"概念，fr/fc 无意义（传 -1），仅判断目标位置是否可落子
bool GomokuRule::IsValidMove(const Board& b, int fr, int fc, int tr, int tc) const {
    auto* gb = CastBoard(b);
    if (!gb) return false;
    (void)fr; (void)fc;                              // 明确忽略这两个参数
    return gb->IsInside(tr, tc) && gb->IsEmpty(tr, tc);  // 在棋盘内且为空位
}

// 判断游戏是否结束
// 逻辑：1. 棋盘满 → 平局  2. side 方已有五连 → side 获胜
bool GomokuRule::IsGameOver(const Board& b, Side side, bool& isDraw, Side& winner) const {
    auto* gb = CastBoard(b);
    if (!gb) return false;

    int s = (side == Side::RED) ? 1 : 2;             // Side→五子棋棋子编号: RED=0→1(黑), BLACK=1→2(白)

    // 棋盘已满 → 平局
    if (IsBoardFull(*gb)) {
        isDraw = true;
        winner = Side::RED;
        return true;
    }

    // 检查是否存在五连
    if (CanWin(*gb, s)) {
        isDraw = false;
        winner = side;
        return true;
    }

    return false;                                    // 游戏继续
}

// 是否有合法落子处（即棋盘上是否还有空位）
bool GomokuRule::HasLegalMoves(const Board& b, Side side) const {
    auto* gb = CastBoard(b);
    if (!gb) return false;
    (void)side;                                      // 五子棋中 side 不影响（双方共用空位）
    return !IsBoardFull(*gb);
}

// ==================== 五子棋专用实现 ====================

// 判断在 (r,c) 落子是否合法
bool GomokuRule::IsValidPlace(const GomokuBoard& b, int r, int c) const {
    return b.IsInside(r, c) && b.IsEmpty(r, c);      // 在棋盘内 + 空位
}

// 棋盘是否已满（所有 225 个位置都有棋子）
bool GomokuRule::IsBoardFull(const GomokuBoard& b) const {
    for (int r = 0; r < GomokuBoard::SIZE; r++)
        for (int c = 0; c < GomokuBoard::SIZE; c++)
            if (b.IsEmpty(r, c)) return false;       // 找到空位 → 未满
    return true;
}

// 检测在 (r,c) 落子后，side 方是否形成五连
// 检查四个方向：水平(→)、垂直(↓)、主对角线(↘)、副对角线(↗)
bool GomokuRule::CheckWin(const GomokuBoard& b, int r, int c, int side) const {
    const int dirs[4][2] = {
        {0, 1},   // 水平 →  (同一行，列递增)
        {1, 0},   // 垂直 ↓  (同一列，行递增)
        {1, 1},   // 主对角线 ↘
        {1, -1}   // 副对角线 ↙（行递增、列递减）
    };

    for (int d = 0; d < 4; d++) {
        int dr = dirs[d][0];
        int dc = dirs[d][1];
        // 连续数 = 自身(1) + 正方向连续数 + 反方向连续数
        int count = 1
                  + CountDir(b, r, c, dr, dc, side)      // 正方向
                  + CountDir(b, r, c, -dr, -dc, side);    // 反方向
        if (count >= 5) return true;                     // 五连或更多 → 获胜
    }
    return false;
}

// 检测 side 方在当前棋盘上是否已有五连
// 遍历所有已有棋子，找到第一个五连即返回
bool GomokuRule::CanWin(const GomokuBoard& b, int side) const {
    for (int r = 0; r < GomokuBoard::SIZE; r++)
        for (int c = 0; c < GomokuBoard::SIZE; c++)
            if (b.GetCell(r, c) == side)                 // 找到己方棋子
                if (CheckWin(b, r, c, side))             // 以此为起点检测五连
                    return true;
    return false;
}

// 从 (r,c) 出发，沿方向 (dr,dc) 统计连续的相同棋子数（不含自身）
// dr,dc 是方向向量，如 {0,1} 表示每次向右移动一列
int GomokuRule::CountDir(const GomokuBoard& b, int r, int c,
                          int dr, int dc, int side) const {
    int count = 0;
    int cr = r + dr;
    int cc = c + dc;
    // 沿方向一直走，直到遇到不同色的棋子或出界
    while (b.IsInside(cr, cc) && b.GetCell(cr, cc) == side) {
        count++;
        cr += dr;
        cc += dc;
    }
    return count;
}
