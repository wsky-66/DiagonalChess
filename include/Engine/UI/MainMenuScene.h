#pragma once
#include "Engine/Core/Scene.h"
#include "Engine/Core/Game.h"
#include "Engine/Core/GameManager.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class MainMenuScene : public Scene {
public:
    MainMenuScene(GameManager& gm);
    ~MainMenuScene() override;

    void Update(float dt) override;
    void Render() override;

    bool IsRunning() const { return window.isOpen(); }
    bool HasPickedGame() const { return launching; }
    std::string GetPickedGame() const { return launchTarget; }

private:
    void ProcessEvents();

    GameManager& gameManager;
    sf::RenderWindow window;
    sf::Font font;
    bool fontLoaded;
    bool launching;
    std::string launchTarget;

    struct MenuTile {
        sf::FloatRect bounds;
        std::string name;
        std::wstring displayName;
        bool hovered;
    };
    std::vector<MenuTile> tiles;
};
