#pragma once

// ===================================================================
// AIPlayer.h — AI 玩家接口（抽象基类）
// 所有棋类游戏的 AI 都继承自此接口
// 目前只有设置难度的方法，具体的"思考"逻辑由子类实现
// ===================================================================

#include "Engine/Common.h"           // 引入 AIDifficulty 枚举

class Board;                         // 前置声明
class Rule;                          // 前置声明

class AIPlayer {
public:
    virtual ~AIPlayer() = default;

    // 设置 AI 难度（EASY/MEDIUM/HARD）
    virtual void SetDifficulty(AIDifficulty d) = 0;
};
