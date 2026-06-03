#include "Engine/Effects/ParticleSystem.h"
#include <random>

void ParticleSystem::Update(float dt) {
    for (auto it = particles.begin(); it != particles.end();) {
        it->pos += it->vel * dt;
        it->vel.y += 200.f * dt;
        it->life -= dt;

        if (it->life <= 0) {
            it = particles.erase(it);
        } else {
            ++it;
        }
    }
}

void ParticleSystem::Draw(sf::RenderWindow& window) {
    for (const auto& p : particles) {
        float alpha = (p.life / p.maxLife) * 255.f;
        sf::Color c = p.color;
        c.a = static_cast<sf::Uint8>(alpha);

        sf::CircleShape particle(p.size);
        particle.setOrigin(p.size, p.size);
        particle.setPosition(p.pos);
        particle.setFillColor(c);
        window.draw(particle);
    }
}

void ParticleSystem::CreateWinParticles(Side winner) {
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> xDist(100, 600);
    std::uniform_real_distribution<float> yDist(200, 600);
    std::uniform_real_distribution<float> velX(-150, 150);
    std::uniform_real_distribution<float> velY(-400, -200);
    std::uniform_real_distribution<float> size(3, 8);

    sf::Color color1, color2;
    if (winner == Side::RED) {
        color1 = sf::Color(255, 100, 100);
        color2 = sf::Color(255, 200, 50);
    } else {
        color1 = sf::Color(200, 200, 200);
        color2 = sf::Color(100, 150, 255);
    }

    for (int i = 0; i < 100; i++) {
        Particle p;
        p.pos = sf::Vector2f(xDist(rng), yDist(rng));
        p.vel = sf::Vector2f(velX(rng), velY(rng));
        p.color = (i % 2 == 0) ? color1 : color2;
        p.life = 3.0f;
        p.maxLife = 3.0f;
        p.size = size(rng);
        particles.push_back(p);
    }
}

void ParticleSystem::CreateDrawParticles() {
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> xDist(100, 600);
    std::uniform_real_distribution<float> yDist(200, 600);
    std::uniform_real_distribution<float> velX(-100, 100);
    std::uniform_real_distribution<float> velY(-300, -150);
    std::uniform_real_distribution<float> size(2, 6);

    for (int i = 0; i < 80; i++) {
        Particle p;
        p.pos = sf::Vector2f(xDist(rng), yDist(rng));
        p.vel = sf::Vector2f(velX(rng), velY(rng));
        p.color = sf::Color(200, 200, 100);
        p.life = 2.5f;
        p.maxLife = 2.5f;
        p.size = size(rng);
        particles.push_back(p);
    }
}
