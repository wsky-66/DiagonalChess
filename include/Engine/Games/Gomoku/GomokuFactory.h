#pragma once
#include "Engine/Core/IGameFactory.h"
#include "Engine/Games/Gomoku/GomokuGame.h"

// 五子棋工厂：供 GameManager 注册和创建游戏实例
class GomokuFactory : public IGameFactory {
public:
    std::string GetName() const override { return "Gomoku"; }
    std::unique_ptr<Game> Create() override {
        return std::make_unique<GomokuGame>();
    }
};
