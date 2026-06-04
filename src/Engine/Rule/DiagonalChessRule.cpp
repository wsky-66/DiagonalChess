// ===================================================================
// DiagonalChessRule.cpp — 对角象棋规则的实现
// 实现了所有棋子的走法规则、将军检测、胜负判定
// ===================================================================

#include "Engine/Rule/DiagonalChessRule.h"
#include <cmath>
#include <algorithm>

// 辅助函数：将抽象 Board 安全转换为 DiagonalChessBoard
static const DiagonalChessBoard* CastBoard(const Board& b) {
    return dynamic_cast<const DiagonalChessBoard*>(&b);
}

// ==================== Rule 抽象接口实现（接受抽象 Board）====================
// 这些方法是对外接口，内部都先做类型转换然后调用 Raw 版本

bool DiagonalChessRule::IsValidMove(const Board& b, int fr, int fc, int tr, int tc) const {
    auto* db = CastBoard(b);
    return db ? IsValidMoveRaw(*db, fr, fc, tr, tc) : false;
}

bool DiagonalChessRule::IsGameOver(const Board& b, Side side, bool& isDraw, Side& winner) const {
    auto* db = CastBoard(b);
    return db ? IsGameOverRaw(*db, side, isDraw, winner) : false;
}

bool DiagonalChessRule::IsInCheck(const Board& b, Side side) const {
    auto* db = CastBoard(b);
    return db ? IsInCheckRaw(*db, static_cast<int>(side)) : false;
}

bool DiagonalChessRule::HasLegalMoves(const Board& b, Side side) const {
    auto* db = CastBoard(b);
    return db ? HasLegalMovesRaw(*db, static_cast<int>(side)) : false;
}

// ==================== 走法合法性判断（原始版）====================
// 先做基本检查（边界、空位、同色），然后根据棋子类型调用对应的走法判断
bool DiagonalChessRule::IsValidMoveRaw(const DiagonalChessBoard& b,
                                        int fr, int fc, int tr, int tc) const {
    if (!b.IsInside(fr, fc) || !b.IsInside(tr, tc)) return false;  // 越界
    if (!b.IsOccupied(fr, fc)) return false;                         // 无棋子可移
    if (!b.At(fr, fc).IsAlive()) return false;                       // 棋子已死亡

    // 目标位置有己方棋子 → 不能吃自己的棋子
    if (b.IsOccupied(tr, tc) && b.At(tr, tc).GetSide() == b.At(fr, fc).GetSide())
        return false;

    switch (b.At(fr, fc).GetDType()) {
        case DChessPieceType::CHARIOT:  return CanChariotMove(b, fr, fc, tr, tc);
        case DChessPieceType::HORSE:    return CanHorseMove(b, fr, fc, tr, tc);
        case DChessPieceType::ELEPHANT: return CanElephantMove(b, fr, fc, tr, tc);
        case DChessPieceType::ADVISOR:  return CanAdvisorMove(b, fr, fc, tr, tc);
        case DChessPieceType::GENERAL:  return CanGeneralMove(b, fr, fc, tr, tc);
        case DChessPieceType::CANNON:   return CanCannonMove(b, fr, fc, tr, tc);
        case DChessPieceType::SOLDIER:  return CanSoldierMove(b, fr, fc, tr, tc, b.At(fr, fc).GetSide());
        default: return false;
    }
}

// ==================== 车：横竖直线，不可越子 ====================
// 车走直线（同行或同列），且路径上不能有其他棋子
bool DiagonalChessRule::CanChariotMove(const DiagonalChessBoard& b,
                                        int fr, int fc, int tr, int tc) const {
    if (fr != tr && fc != tc) return false;              // 必须同行或同列
    return CountPiecesBetween(b, fr, fc, tr, tc) == 0;   // 路径上无阻挡
}

// ==================== 马：走日字（2×1 或 1×2），注意蹩马脚 ====================
// 马走"日"字形：横向2格+纵向1格，或横向1格+纵向2格
// 蹩马脚：如果马前进方向的相邻位置有棋子，则不能朝那个方向走
bool DiagonalChessRule::CanHorseMove(const DiagonalChessBoard& b,
                                      int fr, int fc, int tr, int tc) const {
    int dr = std::abs(tr - fr);
    int dc = std::abs(tc - fc);
    // 必须是日字形：|dr|=2 && |dc|=1 或 |dr|=1 && |dc|=2
    if (!((dr == 2 && dc == 1) || (dr == 1 && dc == 2))) return false;
    return !IsBlockedHorse(b, fr, fc, tr, tc);           // 没蹩马脚才能走
}

// 检测蹩马脚：如果 dr=2，看同列方向第一步的位置；如果 dc=2，看同行方向第一步的位置
bool DiagonalChessRule::IsBlockedHorse(const DiagonalChessBoard& b,
                                        int fr, int fc, int tr, int tc) const {
    int dr = tr - fr;
    int dc = tc - fc;
    if (std::abs(dr) == 2) {
        int blockR = fr + (dr > 0 ? 1 : -1);             // 马脚在纵方向的第一步
        return b.IsOccupied(blockR, fc) && b.At(blockR, fc).IsAlive();
    }
    int blockC = fc + (dc > 0 ? 1 : -1);                 // 马脚在横方向的第一步
    return b.IsOccupied(fr, blockC) && b.At(fr, blockC).IsAlive();
}

// ==================== 象：走田字（2×2 对角），注意塞象眼 ====================
// 象走"田"字形：行和列各走2格（即对角方向 2 步）
// 塞象眼：如果对角路径的正中央有棋子，则不能走
bool DiagonalChessRule::CanElephantMove(const DiagonalChessBoard& b,
                                         int fr, int fc, int tr, int tc) const {
    int dr = std::abs(tr - fr);
    int dc = std::abs(tc - fc);
    if (dr != 2 || dc != 2) return false;                // 必须是田字形
    return !IsBlockedElephant(b, fr, fc, tr, tc);
}

// 检测塞象眼：查看对角线中点是否有棋子
bool DiagonalChessRule::IsBlockedElephant(const DiagonalChessBoard& b,
                                           int fr, int fc, int tr, int tc) const {
    int midR = (fr + tr) / 2;                            // 对角线的中点行
    int midC = (fc + tc) / 2;                            // 对角线的中点列
    return b.IsOccupied(midR, midC) && b.At(midR, midC).IsAlive();
}

// ==================== 士：八个斜方向一步，不出九宫 ====================
// 士走斜线：|dr|<=1 && |dc|<=1 且 dr+dc>0（不能原地不动）
// 且不能出九宫
bool DiagonalChessRule::CanAdvisorMove(const DiagonalChessBoard& b,
                                        int fr, int fc, int tr, int tc) const {
    if (!IsInPalace(tr, tc, b.At(fr, fc).GetSide())) return false;
    int dr = std::abs(tr - fr);
    int dc = std::abs(tc - fc);
    return (dr <= 1 && dc <= 1 && (dr + dc > 0));
}

// ==================== 将/帅：横竖一步，不出九宫 ====================
// 将走一格：上下左右四个方向，不能出九宫
bool DiagonalChessRule::CanGeneralMove(const DiagonalChessBoard& b,
                                        int fr, int fc, int tr, int tc) const {
    if (!IsInPalace(tr, tc, b.At(fr, fc).GetSide())) return false;
    int dr = std::abs(tr - fr);
    int dc = std::abs(tc - fc);
    return (dr + dc == 1);                               // 只走一步（曼哈顿距离=1）
}

// ==================== 炮：移动如车，吃子须隔一子（炮架）====================
// 炮沿直线走：不吃子时路径必须为空；吃子时必须恰好隔一个棋子
bool DiagonalChessRule::CanCannonMove(const DiagonalChessBoard& b,
                                       int fr, int fc, int tr, int tc) const {
    if (fr != tr && fc != tc) return false;              // 必须同行或同列
    int count = CountPiecesBetween(b, fr, fc, tr, tc);   // 路径上棋子数
    if (b.IsOccupied(tr, tc)) return count == 1;          // 吃子：必须隔一子
    return count == 0;                                    // 移动：路径必须全空
}

// 统计两点之间（直线）有多少个棋子（不包括起点和终点）
int DiagonalChessRule::CountPiecesBetween(const DiagonalChessBoard& b,
                                           int fr, int fc, int tr, int tc) const {
    int count = 0;
    if (fr == tr) {                                      // 同一行
        int minC = std::min(fc, tc), maxC = std::max(fc, tc);
        for (int c = minC + 1; c < maxC; c++)
            if (b.IsOccupied(fr, c)) count++;
    } else if (fc == tc) {                                // 同一列
        int minR = std::min(fr, tr), maxR = std::max(fr, tr);
        for (int r = minR + 1; r < maxR; r++)
            if (b.IsOccupied(r, fc)) count++;
    }
    return count;
}

// ==================== 兵/卒：只能斜前一步，不可后退 ====================
// 红方(side=0)：兵只能向 dr=1（行增大）或 dc=-1（列减小）方向走
// 黑方(side=1)：卒只能向 dr=-1（行减小）或 dc=1（列增大）方向走
bool DiagonalChessRule::CanSoldierMove(const DiagonalChessBoard& b,
                                        int fr, int fc, int tr, int tc, int side) const {
    int dr = tr - fr;
    int dc = tc - fc;
    if (std::abs(dr) + std::abs(dc) != 1) return false;  // 只能走一步
    if (side == 0) return (dr == 1 && dc == 0) || (dr == 0 && dc == -1);  // 红兵方向
    return (dr == -1 && dc == 0) || (dr == 0 && dc == 1);                 // 黑卒方向
}

// ==================== 获取所有合法走法 ====================
// 对指定棋子的所有可能目标位置做两层过滤：
// 1. 走法本身合法（IsValidMoveRaw）
// 2. 走完后不会让自己被将军（WouldBeInCheck）
std::vector<sf::Vector2i> DiagonalChessRule::GetValidMoves(
    const DiagonalChessBoard& b, int r, int c) const {
    std::vector<sf::Vector2i> moves;
    if (!b.IsInside(r, c) || !b.IsOccupied(r, c)) return moves;

    int side = b.At(r, c).GetSide();
    for (int tr = 0; tr < 9; tr++) {
        for (int tc = 0; tc < 9; tc++) {
            if (tr == r && tc == c) continue;
            if (b.IsOccupied(tr, tc) && b.At(tr, tc).GetSide() == side) continue;
            if (IsValidMoveRaw(b, r, c, tr, tc)) {
                if (!WouldBeInCheck(b, r, c, tr, tc, side)) {  // 不能造成自己被将军
                    moves.push_back(sf::Vector2i(tr, tc));
                }
            }
        }
    }
    return moves;
}

// ==================== 检测是否被将/将军 ====================
// 规则：找出己方的将/帅，然后看对方是否有棋子能攻击到它
bool DiagonalChessRule::IsInCheckRaw(const DiagonalChessBoard& b, int side) const {
    // 第一步：找到己方将/帅的位置
    int gr = -1, gc = -1;
    for (int r = 0; r < 9 && gr < 0; r++)
        for (int c = 0; c < 9 && gr < 0; c++)
            if (b.IsOccupied(r, c) && b.At(r, c).GetDType() == DChessPieceType::GENERAL
                && b.At(r, c).GetSide() == side)
                { gr = r; gc = c; }
    if (gr < 0) return false;                            // 没有将（不应该出现）

    // 第二步：看对方棋子是否能走到将/帅的位置
    int opp = 1 - side;                                  // 对方阵营
    for (int r = 0; r < 9; r++)
        for (int c = 0; c < 9; c++)
            if (b.IsOccupied(r, c) && b.At(r, c).GetSide() == opp)
                if (IsValidMoveRaw(b, r, c, gr, gc))    // 对方棋子能攻击将/帅 → 被将军
                    return true;
    return false;
}

// ==================== 判断是否在九宫格内 ====================
// 红方九宫：行 0~2, 列 6~8（棋盘右下角区域）
// 黑方九宫：行 6~8, 列 0~2（棋盘左上角区域）
bool DiagonalChessRule::IsInPalace(int r, int c, int side) const {
    if (side == 0) return r >= 0 && r <= 2 && c >= 6 && c <= 8;
    return r >= 6 && r <= 8 && c >= 0 && c <= 2;
}

// ==================== 模拟走棋后是否会被将军 ====================
// 在临时棋盘上模拟走一步，然后检测是否被将军
// 用于过滤"走了之后自己会被将军"的非法走法
bool DiagonalChessRule::WouldBeInCheck(const DiagonalChessBoard& b,
                                        int fr, int fc, int tr, int tc, int side) const {
    DiagonalChessBoard temp;
    b.CopyTo(temp);                                      // 拷贝棋盘
    temp.At(tr, tc) = temp.At(fr, fc);                   // 在拷贝上模拟走棋
    temp.OccupiedCell(tr, tc);
    temp.ClearCell(fr, fc);
    return IsInCheckRaw(temp, side);                     // 结果是否被将军
}

// ==================== 是否有合法走法 ====================
// 遍历所有己方棋子，只要有任何一个棋子有合法走法就返回 true
bool DiagonalChessRule::HasLegalMovesRaw(const DiagonalChessBoard& b, int side) const {
    for (int r = 0; r < 9; r++)
        for (int c = 0; c < 9; c++)
            if (b.IsOccupied(r, c) && b.At(r, c).GetSide() == side)
                if (!GetValidMoves(b, r, c).empty()) return true;
    return false;
}

// ==================== 绝杀判定 ====================
// 绝杀 = 被将军 + 无合法走法（将死）
bool DiagonalChessRule::IsCheckmate(const DiagonalChessBoard& b, int side) const {
    return IsInCheckRaw(b, side) && !HasLegalMovesRaw(b, side);
}

// ==================== 子力不足和棋判定 ====================
// 双方都只剩下将和士（没有任何攻击性棋子）→ 子力不足自动和棋
bool DiagonalChessRule::HasInsufficientMaterial(const DiagonalChessBoard& b) const {
    bool redHasAttack = false, blackHasAttack = false;
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (!b.IsOccupied(r, c)) continue;
            auto t = b.At(r, c).GetDType();
            if (t == DChessPieceType::GENERAL || t == DChessPieceType::ADVISOR) continue;
            if (b.At(r, c).GetSide() == 0) redHasAttack = true;
            else blackHasAttack = true;
        }
    }
    return !redHasAttack && !blackHasAttack;              // 双方都无攻击力
}

// ==================== 检查某方是否完全无攻击力 ====================
// 如果一方只剩将和士（没有车马炮象兵），则无法进攻
bool DiagonalChessRule::SideHasNoAttack(const DiagonalChessBoard& b, int side) const {
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            if (!b.IsOccupied(r, c) || b.At(r, c).GetSide() != side) continue;
            auto t = b.At(r, c).GetDType();
            if (t != DChessPieceType::GENERAL && t != DChessPieceType::ADVISOR)
                return false;                            // 有攻击性棋子
        }
    }
    return true;
}

// ==================== 游戏结束判定 ====================
// 判定顺序：
// 1. 子力不足 → 和棋
// 2. 某方无攻击力 → 对方获胜（困毙）
// 3. 某方无合法走法 → 对方获胜（困毙）
bool DiagonalChessRule::IsGameOverRaw(const DiagonalChessBoard& b, Side side,
                                       bool& isDraw, Side& winner) const {
    int s = static_cast<int>(side);

    // 子力不足 → 和棋
    if (HasInsufficientMaterial(b)) {
        isDraw = true;
        winner = Side::RED;
        return true;
    }

    // 当前方完全无攻击力 → 困毙，对方获胜
    if (SideHasNoAttack(b, s)) {
        isDraw = false;
        winner = (side == Side::RED) ? Side::BLACK : Side::RED;
        return true;
    }

    // 当前方无合法走法 → 困毙（或绝杀），对方获胜
    if (!HasLegalMovesRaw(b, s)) {
        isDraw = false;
        winner = (side == Side::RED) ? Side::BLACK : Side::RED;
        return true;
    }

    return false;                                        // 游戏继续
}
