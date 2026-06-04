#pragma once

// ===================================================================
// IGameFactory.h — 游戏工厂接口
// 工厂模式的核心：每个具体游戏提供一个工厂类
// 工厂负责"创建"游戏实例，GameManager 负责"管理"这些工厂
// ===================================================================

#include "Engine/Core/Game.h"
#include <memory>
#include <string>

class IGameFactory {
public:
    virtual ~IGameFactory() = default;

    // 获取此工厂对应的游戏名称（如 "DiagonalChess"）
    virtual std::string GetName() const = 0;

    // 创建一个新的游戏实例（返回所有权给调用者）
    virtual std::unique_ptr<Game> Create() = 0;
};
