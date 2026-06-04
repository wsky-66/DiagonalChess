#include "Engine/Rule/GomokuRule.h"

// —— 辅助：将抽象 Board 转为 GomokuBoard ——
static const GomokuBoard* CastBoard(const Board& b) {
    return dynamic_cast<const GomokuBoard*>(&b);
}

// —— Rule 抽象接口实现 ——

bool GomokuRule::IsValidMove(const Board& b, int fr, int fc, int tr, int tc) const {
    // 五子棋无"移动"概念：fr/fc 无意义，仅检查 tr/tc 是否可落子
    auto* gb = CastBoard(b);
    if (!gb) return false;
    (void)fr; (void)fc;
    return gb->IsInside(tr, tc) && gb->IsEmpty(tr, tc);
}

bool GomokuRule::IsGameOver(const Board& b, Side side, bool& isDraw, Side& winner) const {
    auto* gb = CastBoard(b);
    if (!gb) return false;

    int s = (side == Side::RED) ? 1 : 2; // 映射：RED(0)→黑(1), BLACK(1)→白(2)

    // 检查是否已满（平局）
    if (IsBoardFull(*gb)) {
        isDraw = true;
        winner = Side::RED;
        return true;
    }

    // 检查上次落子者是否五连
    if (CanWin(*gb, s)) {
        isDraw = false;
        winner = side;
        return true;
    }

    return false;
}

bool GomokuRule::HasLegalMoves(const Board& b, Side side) const {
    auto* gb = CastBoard(b);
    if (!gb) return false;
    (void)side;
    return !IsBoardFull(*gb);
}

// —— Gomoku 专用实现 ——

bool GomokuRule::IsValidPlace(const GomokuBoard& b, int r, int c) const {
    return b.IsInside(r, c) && b.IsEmpty(r, c);
}

bool GomokuRule::IsBoardFull(const GomokuBoard& b) const {
    for (int r = 0; r < GomokuBoard::SIZE; r++)
        for (int c = 0; c < GomokuBoard::SIZE; c++)
            if (b.IsEmpty(r, c)) return false;
    return true;
}

bool GomokuRule::CheckWin(const GomokuBoard& b, int r, int c, int side) const {
    // 四个方向：水平、垂直、主对角线、副对角线
    const int dirs[4][2] = {
        {0, 1},  // 水平 →
        {1, 0},  // 垂直 ↓
        {1, 1},  // 主对角线 ↘
        {1, -1}  // 副对角线 ↙
    };

    for (int d = 0; d < 4; d++) {
        int dr = dirs[d][0];
        int dc = dirs[d][1];
        // 正向计数 + 反向计数 + 自身 = 连续数
        int count = 1
                  + CountDir(b, r, c, dr, dc, side)     // 正方向
                  + CountDir(b, r, c, -dr, -dc, side);   // 反方向
        if (count >= 5) return true;
    }
    return false;
}

bool GomokuRule::CanWin(const GomokuBoard& b, int side) const {
    // 遍历所有已落子位置，检测是否已有五连
    for (int r = 0; r < GomokuBoard::SIZE; r++)
        for (int c = 0; c < GomokuBoard::SIZE; c++)
            if (b.GetCell(r, c) == side)
                if (CheckWin(b, r, c, side))
                    return true;
    return false;
}

int GomokuRule::CountDir(const GomokuBoard& b, int r, int c, int dr, int dc, int side) const {
    int count = 0;
    int cr = r + dr;
    int cc = c + dc;
    while (b.IsInside(cr, cc) && b.GetCell(cr, cc) == side) {
        count++;
        cr += dr;
        cc += dc;
    }
    return count;
}
