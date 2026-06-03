#pragma once
#include "Engine/Core/Game.h"
#include "Engine/Core/IGameFactory.h"
#include <memory>
#include <vector>
#include <string>

class GameManager {
public:
    void RegisterGame(std::unique_ptr<IGameFactory> factory);
    void StartGame(const std::string& gameName);
    void ExitGame();
    Game* CurrentGame() { return currentGame.get(); }

    const std::vector<std::unique_ptr<IGameFactory>>& GetFactories() const { return factories; }

private:
    std::vector<std::unique_ptr<IGameFactory>> factories;
    std::unique_ptr<Game> currentGame;
};
