#pragma once

// ===================================================================
// Game.h — 游戏类的抽象基类
// Game 继承自 Scene（场景），代表一个具体的游戏实例
// 在 Scene 的基础上增加了 Reset、GetName、IsRunning
// ===================================================================

#include "Engine/Core/Scene.h"
#include <string>

class Game : public Scene {
public:
    virtual ~Game() = default;

    // 重置游戏（清除所有状态，回到初始状态）
    virtual void Reset() = 0;

    // 获取游戏名称（如 "DiagonalChess" 或 "Gomoku"）
    virtual std::string GetName() const = 0;

    // 游戏是否仍在运行？（窗口是否打开）
    // 返回 false 表示玩家关闭了窗口，应该退出此游戏
    virtual bool IsRunning() const = 0;
};
