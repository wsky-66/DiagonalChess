#pragma once

// ===================================================================
// ParticleSystem.h — 粒子特效系统
// 在游戏胜利或和棋时产生烟花粒子效果
// 每个粒子有位置、速度、颜色、生命值，受重力影响向下加速
// ===================================================================

#include "Engine/Common.h"           // 引入 Particle 结构体

class ParticleSystem {
public:
    // 每帧更新所有粒子的位置和生命值
    // dt = 帧间隔时间（秒）
    void Update(float dt);

    // 将所有存活的粒子绘制到窗口上
    void Draw(sf::RenderWindow& window);

    // 创建胜利粒子特效（红色/金色调 或 灰/蓝色调）
    // winner: 胜者阵营（影响粒子颜色）
    void CreateWinParticles(Side winner);

    // 创建和棋粒子特效（黄色调）
    void CreateDrawParticles();

    // 清除所有粒子
    void Clear() { particles.clear(); }

private:
    std::vector<Particle> particles;  // 所有活跃粒子的列表
};
