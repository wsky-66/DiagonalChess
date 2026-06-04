#pragma once

// ===================================================================
// ChessPlatform.h — 游戏平台（最高层控制）
// 这是整个程序的最外层控制器
// 负责：注册游戏 → 显示主菜单 → 启动选中游戏 → 返回主菜单（循环）
// ===================================================================

#include "Engine/Core/GameManager.h"
#include "Engine/Core/Scene.h"
#include <memory>
#include <string>

class ChessPlatform {
public:
    ChessPlatform();

    // 启动整个程序的主循环
    void Run();

    // 注册一个游戏类型（通过工厂）
    void RegisterGame(std::unique_ptr<IGameFactory> factory);

    // 获取游戏管理器（供主菜单读取注册的游戏列表）
    GameManager& GetGameManager() { return gameManager; }

private:
    // 启动指定名称的游戏
    void LaunchGame(const std::string& name);

    GameManager gameManager;         // 游戏管理器（管理工厂和当前游戏）
};
