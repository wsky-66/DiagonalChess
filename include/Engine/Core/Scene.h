#pragma once

class Scene {
public:
    virtual ~Scene() = default;
    virtual void Update(float dt) = 0;
    virtual void Render() = 0;
};
