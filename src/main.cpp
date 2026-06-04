// ===================================================================
// main.cpp — 程序入口
// 1. 创建 ChessPlatform（游戏平台）
// 2. 注册两款游戏：对角象棋(DiagonalChess) 和 五子棋(Gomoku)
// 3. 调用 Run() 启动主循环（主菜单 → 选择游戏 → 玩游戏 → 返回主菜单）
// ===================================================================

#include "Engine/Core/ChessPlatform.h"
#include "Engine/Games/DiagonalChess/DiagonalChessFactory.h"
#include "Engine/Games/Gomoku/GomokuFactory.h"

int main() {
    ChessPlatform platform;                                    // 创建游戏平台
    platform.RegisterGame(std::make_unique<DiagonalChessFactory>());  // 注册对角象棋
    platform.RegisterGame(std::make_unique<GomokuFactory>());        // 注册五子棋
    platform.Run();                                            // 启动主循环（永不返回直到窗口关闭）
    return 0;
}
