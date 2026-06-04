// ===================================================================
// MainMenuScene.cpp — 主菜单场景的实现
// 显示一个窗口，列出所有已注册的游戏（如对角象棋、五子棋）
// 用户点击某个游戏卡片后，返回游戏名给 ChessPlatform
// ===================================================================

#include "Engine/UI/MainMenuScene.h"

// 构造函数：创建窗口 + 加载字体 + 生成游戏卡片列表
MainMenuScene::MainMenuScene(GameManager& gm)
    : gameManager(gm)
    , window(sf::VideoMode(800, 600), L"\u68CB\u7C7B\u6E38\u620F\u5927\u5168",  // 窗口标题"棋类游戏大全"
             sf::Style::Titlebar | sf::Style::Close)  // 标题栏 + 关闭按钮
    , fontLoaded(false), launching(false)
{
    window.setFramerateLimit(60);                    // 限制帧率 60 FPS

    // 加载中文字体（按优先级尝试：黑体 → 微软雅黑 → 宋体）
    if (font.loadFromFile("C:/Windows/Fonts/simhei.ttf") ||
        font.loadFromFile("C:/Windows/Fonts/msyh.ttc") ||
        font.loadFromFile("C:/Windows/Fonts/simsun.ttc")) {
        fontLoaded = true;
    }

    // 遍历所有已注册的游戏工厂，为每个游戏创建一个菜单卡片
    float y = 160;                                   // 第一个卡片的 Y 坐标
    for (auto& f : gameManager.GetFactories()) {
        std::string aname = f->GetName();             // 英文名（如 "DiagonalChess"）
        std::wstring name(aname.begin(), aname.end());
        std::wstring display = L"\u25B6 " + name;     // 显示名 "▶ DiagonalChess"
        tiles.push_back({sf::FloatRect(200, y, 400, 60), f->GetName(), display, false});
        y += 80;                                     // 每个卡片间隔 80 像素
    }
}

// 析构函数：关闭窗口
MainMenuScene::~MainMenuScene() {
    if (window.isOpen()) window.close();
}

// 每帧更新：仅在窗口仍打开时处理事件
void MainMenuScene::Update(float dt) {
    ProcessEvents();
    if (!launching && !window.isOpen()) return;       // 关闭窗口 → 提前退出
}

// 渲染主菜单
void MainMenuScene::Render() {
    window.clear(sf::Color(25, 20, 15));             // 深色背景

    if (fontLoaded) {
        // 绘制文本的局部 lambda 函数
        auto drawText = [this](const std::wstring& s, float x, float y,
                                unsigned sz, sf::Color c, bool center = false) {
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

        // 标题
        drawText(L"\u68CB\u7C7B\u6E38\u620F\u5927\u5168", 400, 50, 40, sf::Color(255, 215, 0), true);  // "棋类游戏大全"
        drawText(L"\u2500\u2500\u2500\u2500\u2500 \u9009\u62E9\u6E38\u620F \u2500\u2500\u2500\u2500\u2500",
                 400, 100, 18, sf::Color(180, 160, 130), true);  // "----- 选择游戏 -----"

        // 绘制每个游戏卡片
        for (auto& tile : tiles) {
            // 卡片背景（悬停时变亮）
            sf::RectangleShape rect(sf::Vector2f(tile.bounds.width, tile.bounds.height));
            rect.setPosition(tile.bounds.left, tile.bounds.top);
            sf::Color fill = tile.hovered ? sf::Color(80, 60, 40) : sf::Color(50, 38, 28);
            rect.setFillColor(fill);
            rect.setOutlineColor(sf::Color(120, 95, 65));
            rect.setOutlineThickness(2);
            window.draw(rect);

            // 卡片文字
            sf::Color tc = tile.hovered ? sf::Color(255, 230, 180) : sf::Color(210, 190, 160);
            drawText(tile.displayName,
                tile.bounds.left + tile.bounds.width / 2.f,
                tile.bounds.top + tile.bounds.height / 2.f,
                24, tc, true);
        }

        // 版本号
        drawText(L"v0.1  C++17 + SFML 2.6.2", 400, 560, 12, sf::Color(100, 90, 80), true);
    }

    window.display();                                // 将绘制的内容显示到屏幕上
}

// 处理窗口事件：关闭、点击、鼠标移动
void MainMenuScene::ProcessEvents() {
    sf::Event e;
    while (window.pollEvent(e)) {
        // 点击关闭按钮 → 关闭窗口
        if (e.type == sf::Event::Closed) { window.close(); return; }

        // 鼠标左键点击 → 检测是否点击了某个游戏卡片
        if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
            for (auto& tile : tiles) {
                if (tile.bounds.contains(e.mouseButton.x, e.mouseButton.y)) {
                    launching = true;                // 标记正在启动游戏
                    launchTarget = tile.name;         // 记录选中的游戏名
                    window.close();                   // 关闭菜单窗口
                    return;
                }
            }
        }

        // 鼠标移动 → 更新卡片的悬停状态
        if (e.type == sf::Event::MouseMoved) {
            for (auto& tile : tiles) {
                tile.hovered = tile.bounds.contains(e.mouseMove.x, e.mouseMove.y);
            }
        }
    }
}
