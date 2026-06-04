#pragma once

// ===================================================================
// GameManager.h — 游戏管理器
// 负责管理所有已注册的游戏类型（工厂），并能启动/退出指定游戏
// 使用"工厂模式"：先注册工厂，需要时通过工厂创建游戏实例
// ===================================================================

#include "Engine/Core/Game.h"
#include "Engine/Core/IGameFactory.h"
#include <memory>
#include <vector>
#include <string>

class GameManager {
public:
    // 注册一个游戏工厂（如 DiagonalChessFactory、GomokuFactory）
    // 工厂用于创建对应的游戏实例
    void RegisterGame(std::unique_ptr<IGameFactory> factory);

    // 根据游戏名称启动游戏（通过遍历已注册的工厂找到对应工厂并创建）
    void StartGame(const std::string& gameName);

    // 退出当前游戏（释放游戏实例）
    void ExitGame();

    // 获取当前正在运行的游戏实例（可能为 nullptr）
    Game* CurrentGame() { return currentGame.get(); }

    // 获取所有已注册的工厂列表（只读）
    const std::vector<std::unique_ptr<IGameFactory>>& GetFactories() const { return factories; }

private:
    std::vector<std::unique_ptr<IGameFactory>> factories;  // 已注册的游戏工厂列表
    std::unique_ptr<Game> currentGame;                     // 当前游戏实例
};
