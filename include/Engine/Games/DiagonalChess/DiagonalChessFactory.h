#pragma once
#include "Engine/Core/IGameFactory.h"
#include "Engine/Games/DiagonalChess/DiagonalChessGame.h"

class DiagonalChessFactory : public IGameFactory {
public:
    std::string GetName() const override { return "DiagonalChess"; }
    std::unique_ptr<Game> Create() override {
        return std::make_unique<DiagonalChessGame>();
    }
};
