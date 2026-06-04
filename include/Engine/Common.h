#pragma once                         // 防止头文件被重复包含

// ===================================================================
// Common.h — 全项目共享的类型定义、枚举、结构体、常量
// 所有其他文件都会（直接或间接）包含此文件
// ===================================================================

#include <SFML/Graphics.hpp>           // SFML图形库（窗口、绘制）
#include <SFML/Audio.hpp>              // SFML音频库
#include <SFML/Network.hpp>            // SFML网络库
#include <vector>                      // 动态数组（如棋子列表）
#include <string>                      // 字符串
#include <stack>                       // 栈（悔棋功能）

// —— 阵营枚举 —— 只有红方和黑方两个阵营
// RED = 0 表示红方，BLACK = 1 表示黑方
enum class Side { RED = 0, BLACK = 1 };

// —— AI 难度枚举 ——
// EASY=简单(搜索2层), MEDIUM=中等(搜索3层), HARD=困难(搜索4层)
enum class AIDifficulty { EASY, MEDIUM, HARD };

// —— 网络连接状态枚举 ——
// OFFLINE=离线, HOST_WAITING=等待连接(房主), CONNECTING=正在连接, CONNECTED=已连接
enum class NetState { OFFLINE, HOST_WAITING, CONNECTING, CONNECTED };

// —— 走棋记录结构体 ——
// 每一步棋的信息，用于悔棋功能和走棋日志
struct MoveRecord {
    int fromR, fromC;                  // 起始位置（行, 列）
    int toR, toC;                      // 目标位置（行, 列）
    int movedType, movedSide;          // 移动的棋子类型和阵营
    int capturedType, capturedSide;    // 被吃掉的棋子类型和阵营（-1表示没吃子）
    Side side;                         // 走棋方的阵营
    int prevMovesWithoutCapture;       // 走棋前"无吃子步数"计数器（用于和棋判断）
};

// —— AI 走法结构体 ——
// AI 搜索后返回的一个候选走法及其评分
struct AIMove {
    int fromR, fromC;                  // 起始位置
    int toR, toC;                      // 目标位置
    int score;                         // 评分（分数越高对AI越有利）
};

// —— UI 按钮结构体 ——
// 描述一个界面按钮的基本属性
struct UIButton {
    sf::FloatRect bounds;              // 按钮的矩形区域（位置+大小）
    std::wstring label;                // 按钮上显示的文字
    bool hovered;                      // 鼠标是否悬停在按钮上
    bool disabled;                     // 按钮是否被禁用（灰色不可点击）
};

// —— 粒子结构体 ——
// 描述一个粒子（用于胜利/和棋的烟花特效）
struct Particle {
    sf::Vector2f pos;                  // 粒子当前位置
    sf::Vector2f vel;                  // 粒子速度（x和y方向）
    sf::Color color;                   // 粒子颜色（带透明度）
    float life;                        // 剩余生命（秒）
    float maxLife;                     // 总生命时长（秒）
    float size;                        // 粒子大小（半径）
};

// —— 走法结构体（简化版）——
// 仅包含起止坐标，用于传递走棋信息的轻量结构
struct Move {
    int fromR, fromC, toR, toC;
};

// —— 全局常量 ——
static constexpr int DIAG = 40;        // 斜棋盘的格距（像素），即每个网格两条对角线的半长度
static constexpr float ORIGIN_X = 50.f;   // 棋盘左上角的屏幕 X 坐标
static constexpr float ORIGIN_Y = 410.f;  // 棋盘左上角的屏幕 Y 坐标
static constexpr int WIN_W = 1150;     // 窗口宽度（像素）
static constexpr int WIN_H = 820;      // 窗口高度（像素）
static constexpr int PIECE_R = 20;     // 棋子圆的半径（像素）
static constexpr int MAX_LOG = 20;     // 走棋日志最大显示条数（超过则删除旧记录）
static constexpr int DRAW_LIMIT = 120; // 和棋步数限制：连续120步无吃子则自动和棋
