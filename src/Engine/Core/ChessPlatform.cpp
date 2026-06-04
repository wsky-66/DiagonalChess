#include "Engine/Core/ChessPlatform.h"
#include "Engine/UI/MainMenuScene.h"
#include "Engine/Core/Scene.h"
#include <SFML/Graphics.hpp>

ChessPlatform::ChessPlatform() {}

void ChessPlatform::RegisterGame(std::unique_ptr<IGameFactory> factory) {
    gameManager.RegisterGame(std::move(factory));
}

static void RunScene(Scene& scene) {
    sf::Clock clock;
    Game* game = dynamic_cast<Game*>(&scene);
    MainMenuScene* menu = dynamic_cast<MainMenuScene*>(&scene);

    auto loop = [&]() -> bool {
        if (game) return game->IsRunning();
        if (menu) return menu->IsRunning();
        return false;
    };

    while (loop()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f;
        scene.Update(dt);
        if (!loop()) break;
        scene.Render();
    }
}

void ChessPlatform::Run() {
    while (true) {
        MainMenuScene menu(gameManager);
        RunScene(menu);
        if (!menu.HasPickedGame()) return;
        gameManager.StartGame(menu.GetPickedGame());
        Game* game = gameManager.CurrentGame();
        if (!game) return;
        RunScene(*game);
    }
}
