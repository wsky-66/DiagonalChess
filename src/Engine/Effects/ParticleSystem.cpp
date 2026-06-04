// ===================================================================
// ParticleSystem.cpp — 粒子特效系统的实现
// 管理游戏结束时的烟花粒子效果
// 每个粒子有位置、速度、颜色、生命值，受重力影响
// ===================================================================

#include "Engine/Effects/ParticleSystem.h"
#include <random>                                     // 随机数生成器

// 每帧更新所有粒子
// dt = 距离上一帧的时间（秒）
void ParticleSystem::Update(float dt) {
    for (auto it = particles.begin(); it != particles.end();) {
        // 物理模拟：位置 += 速度 × 时间
        it->pos += it->vel * dt;
        // 重力效果：Y 方向速度增加（向下加速）
        it->vel.y += 200.f * dt;
        // 生命值衰减
        it->life -= dt;

        // 生命值耗尽 → 删除粒子
        if (it->life <= 0) {
            it = particles.erase(it);                // erase 返回下一个有效迭代器
        } else {
            ++it;
        }
    }
}

// 绘制所有存活的粒子
void ParticleSystem::Draw(sf::RenderWindow& window) {
    for (const auto& p : particles) {
        // 根据剩余生命比例计算透明度（越接近死亡越透明）
        float alpha = (p.life / p.maxLife) * 255.f;
        sf::Color c = p.color;
        c.a = static_cast<sf::Uint8>(alpha);         // 设置 Alpha 通道

        sf::CircleShape particle(p.size);             // 用圆形表示粒子
        particle.setOrigin(p.size, p.size);           // 设置中心点为原点
        particle.setPosition(p.pos);                  // 设置粒子位置
        particle.setFillColor(c);
        window.draw(particle);
    }
}

// 创建胜利粒子特效（100个粒子）
// winner 决定粒子颜色：红方胜=红色+金色, 黑方胜=灰色+蓝色
void ParticleSystem::CreateWinParticles(Side winner) {
    std::mt19937 rng(std::random_device{}());         // 梅森旋转随机数引擎
    std::uniform_real_distribution<float> xDist(100, 600);    // X 位置范围
    std::uniform_real_distribution<float> yDist(200, 600);    // Y 位置范围
    std::uniform_real_distribution<float> velX(-150, 150);   // X 方向速度范围
    std::uniform_real_distribution<float> velY(-400, -200);  // Y 方向速度范围（向上弹出）
    std::uniform_real_distribution<float> size(3, 8);        // 粒子大小范围

    sf::Color color1, color2;
    if (winner == Side::RED) {
        color1 = sf::Color(255, 100, 100);            // 红色调
        color2 = sf::Color(255, 200, 50);             // 金色调
    } else {
        color1 = sf::Color(200, 200, 200);            // 灰色调
        color2 = sf::Color(100, 150, 255);            // 蓝色调
    }

    for (int i = 0; i < 100; i++) {
        Particle p;
        p.pos = sf::Vector2f(xDist(rng), yDist(rng));  // 随机初始位置
        p.vel = sf::Vector2f(velX(rng), velY(rng));    // 随机初始速度
        p.color = (i % 2 == 0) ? color1 : color2;       // 交替使用两种颜色
        p.life = 3.0f;
        p.maxLife = 3.0f;
        p.size = size(rng);
        particles.push_back(p);
    }
}

// 创建和棋粒子特效（80个粒子，黄色调）
void ParticleSystem::CreateDrawParticles() {
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> xDist(100, 600);
    std::uniform_real_distribution<float> yDist(200, 600);
    std::uniform_real_distribution<float> velX(-100, 100);    // 较慢的 X 速度
    std::uniform_real_distribution<float> velY(-300, -150);
    std::uniform_real_distribution<float> size(2, 6);         // 较小的粒子

    for (int i = 0; i < 80; i++) {
        Particle p;
        p.pos = sf::Vector2f(xDist(rng), yDist(rng));
        p.vel = sf::Vector2f(velX(rng), velY(rng));
        p.color = sf::Color(200, 200, 100);             // 统一的黄色调
        p.life = 2.5f;
        p.maxLife = 2.5f;
        p.size = size(rng);
        particles.push_back(p);
    }
}
