#pragma once
#include "Engine/Common.h"

class ParticleSystem {
public:
    void Update(float dt);
    void Draw(sf::RenderWindow& window);
    void CreateWinParticles(Side winner);
    void CreateDrawParticles();
    void Clear() { particles.clear(); }

private:
    std::vector<Particle> particles;
};
