#pragma once

// ===================================================================
// MainMenuScene.h — 主菜单场景
// 程序启动后显示的第一个界面
// 列出所有已注册的游戏（对角象棋、五子棋等），点击即可进入
// ===================================================================

#include "Engine/Core/Scene.h"
#include "Engine/Core/Game.h"
#include "Engine/Core/GameManager.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class MainMenuScene : public Scene {
public:
    // 构造函数：传入 GameManager 以获取注册的游戏列表
    MainMenuScene(GameManager& gm);
    ~MainMenuScene() override;

    void Update(float dt) override;      // 处理事件（鼠标点击、关闭窗口）
    void Render() override;              // 渲染主菜单界面

    bool IsRunning() const { return window.isOpen(); }    // 窗口是否打开
    bool HasPickedGame() const { return launching; }      // 玩家是否已选择游戏
    std::string GetPickedGame() const { return launchTarget; } // 获取选中的游戏名

private:
    void ProcessEvents();                // 处理鼠标事件

    GameManager& gameManager;            // 游戏管理器（引用，不拥有所有权）
    sf::RenderWindow window;             // SFML 渲染窗口
    sf::Font font;                       // 字体
    bool fontLoaded;                     // 字体是否加载成功
    bool launching;                      // 是否正在启动游戏
    std::string launchTarget;            // 要启动的游戏名称

    // 菜单卡片结构体：每个游戏对应一个可点击的矩形区域
    struct MenuTile {
        sf::FloatRect bounds;            // 矩形区域
        std::string name;                // 游戏英文名（内部标识）
        std::wstring displayName;        // 游戏显示名（中文）
        bool hovered;                    // 鼠标是否悬停
    };
    std::vector<MenuTile> tiles;         // 所有菜单卡片
};
