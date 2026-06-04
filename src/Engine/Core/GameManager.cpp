// ===================================================================
// GameManager.cpp — 游戏管理器的实现
// 核心功能：存储游戏工厂列表 → 按名称查找工厂 → 创建游戏实例 → 释放当前游戏
// ===================================================================

#include "Engine/Core/GameManager.h"

// 注册游戏工厂（添加到内部列表中）
void GameManager::RegisterGame(std::unique_ptr<IGameFactory> factory) {
    factories.push_back(std::move(factory));              // move 转移所有权给管理器
}

// 根据游戏名称启动游戏
// 遍历所有已注册的工厂，找到名称匹配的那个，调用 Create() 创建游戏实例
void GameManager::StartGame(const std::string& gameName) {
    for (auto& f : factories) {
        if (f->GetName() == gameName) {                  // 匹配工厂名称
            currentGame = f->Create();                    // 通过工厂创建游戏实例
            return;
        }
    }
}

// 退出当前游戏（释放游戏对象，currentGame 变为 nullptr）
void GameManager::ExitGame() {
    currentGame.reset();                                 // unique_ptr::reset() 销毁原对象
}
