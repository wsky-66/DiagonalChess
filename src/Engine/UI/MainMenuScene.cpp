#include "Engine/UI/MainMenuScene.h"
#include "Engine/Piece.h"

MainMenuScene::MainMenuScene(GameManager& gm)
    : gameManager(gm)
    , window(sf::VideoMode(800, 600), L"\u68CB\u7C7B\u6E38\u620F\u5927\u5168", sf::Style::Titlebar | sf::Style::Close)
    , fontLoaded(false), launching(false)
{
    window.setFramerateLimit(60);

    if (font.loadFromFile("C:/Windows/Fonts/simhei.ttf") ||
        font.loadFromFile("C:/Windows/Fonts/msyh.ttc") ||
        font.loadFromFile("C:/Windows/Fonts/simsun.ttc")) {
        fontLoaded = true;
    }

    float y = 160;
    for (auto& f : gameManager.GetFactories()) {
        std::string aname = f->GetName();
        std::wstring name(aname.begin(), aname.end());
        std::wstring display = L"\u25B6 " + name;
        tiles.push_back({sf::FloatRect(200, y, 400, 60), f->GetName(), display, false});
        y += 80;
    }
}

MainMenuScene::~MainMenuScene() {
    if (window.isOpen()) window.close();
}

void MainMenuScene::Update(float dt) {
    ProcessEvents();
    if (!launching && !window.isOpen()) return;
}

void MainMenuScene::Render() {
    window.clear(sf::Color(25, 20, 15));

    if (fontLoaded) {
        auto drawText = [this](const std::wstring& s, float x, float y, unsigned sz, sf::Color c, bool center = false) {
            sf::Text t;
            t.setFont(font);
            t.setString(s);
            t.setCharacterSize(sz);
            t.setFillColor(c);
            if (center) {
                auto b = t.getLocalBounds();
                t.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
            }
            t.setPosition(x, y);
            window.draw(t);
        };

        drawText(L"\u68CB\u7C7B\u6E38\u620F\u5927\u5168", 400, 50, 40, sf::Color(255, 215, 0), true);
        drawText(L"\u2500\u2500\u2500\u2500\u2500 \u9009\u62E9\u6E38\u620F \u2500\u2500\u2500\u2500\u2500", 400, 100, 18, sf::Color(180, 160, 130), true);

        for (auto& tile : tiles) {
            sf::RectangleShape rect(sf::Vector2f(tile.bounds.width, tile.bounds.height));
            rect.setPosition(tile.bounds.left, tile.bounds.top);
            sf::Color fill = tile.hovered ? sf::Color(80, 60, 40) : sf::Color(50, 38, 28);
            rect.setFillColor(fill);
            rect.setOutlineColor(sf::Color(120, 95, 65));
            rect.setOutlineThickness(2);
            window.draw(rect);

            sf::Color tc = tile.hovered ? sf::Color(255, 230, 180) : sf::Color(210, 190, 160);
            drawText(tile.displayName,
                tile.bounds.left + tile.bounds.width / 2.f,
                tile.bounds.top + tile.bounds.height / 2.f,
                24, tc, true);
        }

        drawText(L"v0.1  C++17 + SFML 2.6.2", 400, 560, 12, sf::Color(100, 90, 80), true);
    }

    window.display();
}

void MainMenuScene::ProcessEvents() {
    sf::Event e;
    while (window.pollEvent(e)) {
        if (e.type == sf::Event::Closed) { window.close(); return; }

        if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
            for (auto& tile : tiles) {
                if (tile.bounds.contains(e.mouseButton.x, e.mouseButton.y)) {
                    launching = true;
                    launchTarget = tile.name;
                    window.close();
                    return;
                }
            }
        }

        if (e.type == sf::Event::MouseMoved) {
            for (auto& tile : tiles) {
                tile.hovered = tile.bounds.contains(e.mouseMove.x, e.mouseMove.y);
            }
        }
    }
}
