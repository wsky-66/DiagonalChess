#pragma once
#include "Engine/Core/GameManager.h"
#include "Engine/Core/Scene.h"
#include <memory>

class ChessPlatform {
public:
    ChessPlatform();
    void Run();

    void RegisterGame(std::unique_ptr<IGameFactory> factory);
    GameManager& GetGameManager() { return gameManager; }

private:
    GameManager gameManager;
    std::unique_ptr<Scene> currentScene;
};
