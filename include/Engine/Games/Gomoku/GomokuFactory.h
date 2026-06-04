#pragma once

// ===================================================================
// GomokuFactory.h — 五子棋的工厂类
// 实现 IGameFactory 接口，用于创建 GomokuGame 实例
// 注册到 GameManager 后，主菜单就能显示"五子棋"选项
// ===================================================================

#include "Engine/Core/IGameFactory.h"
#include "Engine/Games/Gomoku/GomokuGame.h"

class GomokuFactory : public IGameFactory {
public:
    std::string GetName() const override { return "Gomoku"; }
    std::unique_ptr<Game> Create() override {
        return std::make_unique<GomokuGame>();
    }
};
