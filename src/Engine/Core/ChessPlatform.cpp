#include "Engine/Core/ChessPlatform.h"
#include <SFML/Graphics.hpp>

ChessPlatform::ChessPlatform() {
}

void ChessPlatform::RegisterGame(std::unique_ptr<IGameFactory> factory) {
    gameManager.RegisterGame(std::move(factory));
}

void ChessPlatform::Run() {
    gameManager.StartGame("DiagonalChess");

    sf::Clock clock;
    Game* game = gameManager.CurrentGame();
    while (game && game->IsRunning()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f;

        game->Update(dt);
        game->Render();
    }
}
