#include "Engine/Core/GameManager.h"

void GameManager::RegisterGame(std::unique_ptr<IGameFactory> factory) {
    factories.push_back(std::move(factory));
}

void GameManager::StartGame(const std::string& gameName) {
    for (auto& f : factories) {
        if (f->GetName() == gameName) {
            currentGame = f->Create();
            return;
        }
    }
}

void GameManager::ExitGame() {
    currentGame.reset();
}
