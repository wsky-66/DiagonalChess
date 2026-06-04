#pragma once

// ===================================================================
// DiagonalChessFactory.h — 对角象棋的工厂类
// 实现 IGameFactory 接口，用于创建 DiagonalChessGame 实例
// 注册到 GameManager 后，主菜单就能显示"对角象棋"选项
// ===================================================================

#include "Engine/Core/IGameFactory.h"
#include "Engine/Games/DiagonalChess/DiagonalChessGame.h"

class DiagonalChessFactory : public IGameFactory {
public:
    std::string GetName() const override { return "DiagonalChess"; }
    std::unique_ptr<Game> Create() override {
        return std::make_unique<DiagonalChessGame>();
    }
};
