#pragma once
#include "Engine/Core/GameManager.h"
#include "Engine/Core/Scene.h"
#include <memory>
#include <string>

class ChessPlatform {
public:
    ChessPlatform();
    void Run();

    void RegisterGame(std::unique_ptr<IGameFactory> factory);
    GameManager& GetGameManager() { return gameManager; }

private:
    void LaunchGame(const std::string& name);

    GameManager gameManager;
};
