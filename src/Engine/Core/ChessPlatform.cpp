// ===================================================================
// ChessPlatform.cpp — 游戏平台的实现
// Run() 的主循环：显示主菜单 → 用户选择游戏 → 运行该游戏 → 游戏结束回到主菜单
// RunScene() 是一个通用的场景运行器，适用于任何 Scene（菜单或游戏）
// ===================================================================

#include "Engine/Core/ChessPlatform.h"
#include "Engine/UI/MainMenuScene.h"
#include "Engine/Core/Scene.h"
#include <SFML/Graphics.hpp>

// 构造函数（无特殊初始化）
ChessPlatform::ChessPlatform() {}

// 注册游戏工厂（转发给 GameManager）
void ChessPlatform::RegisterGame(std::unique_ptr<IGameFactory> factory) {
    gameManager.RegisterGame(std::move(factory));
}

// ==================== 运行一个场景的通用函数 ====================
// 参数 scene 可以是 MainMenuScene 或任何一个 Game 实例
// 使用 dynamic_cast 判断场景类型，从而调用正确的 IsRunning() 方法
static void RunScene(Scene& scene) {
    sf::Clock clock;                          // 高精度时钟（用于计算帧间隔 dt）

    Game* game = dynamic_cast<Game*>(&scene);           // 尝试转为 Game 类型
    MainMenuScene* menu = dynamic_cast<MainMenuScene*>(&scene); // 尝试转为 MainMenuScene 类型

    // 判断场景是否仍在运行的 lambda 函数
    auto loop = [&]() -> bool {
        if (game) return game->IsRunning();             // 游戏：检查窗口是否打开
        if (menu) return menu->IsRunning();             // 菜单：检查窗口是否打开
        return false;
    };

    // 主循环：只要场景还在运行就不断更新+渲染
    while (loop()) {
        float dt = clock.restart().asSeconds();          // 获取距离上一帧的时间（秒）
        if (dt > 0.05f) dt = 0.05f;                     // 防止卡顿导致 dt 过大（限制最高 0.05 秒）
        scene.Update(dt);                                // 更新逻辑
        if (!loop()) break;                              // 更新后可能关闭窗口
        scene.Render();                                  // 渲染画面
    }
}

// ==================== 主循环 ====================
// 外层 while(true) 实现"选游戏 → 玩游戏 → 回菜单"的循环
void ChessPlatform::Run() {
    while (true) {
        // 1. 创建并显示主菜单
        MainMenuScene menu(gameManager);
        RunScene(menu);

        // 2. 检查用户是否选择了游戏（还是直接关闭了窗口）
        if (!menu.HasPickedGame()) return;               // 关闭窗口 → 退出程序

        // 3. 启动用户选择的游戏
        gameManager.StartGame(menu.GetPickedGame());
        Game* game = gameManager.CurrentGame();
        if (!game) return;                               // 创建失败 → 退出

        // 4. 运行游戏（直到玩家关闭窗口或返回主菜单）
        RunScene(*game);
    }
}
