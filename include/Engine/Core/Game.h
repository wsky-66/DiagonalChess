#pragma once
#include "Engine/Core/Scene.h"
#include <string>

class Game : public Scene {
public:
    virtual ~Game() = default;
    virtual void Reset() = 0;
    virtual std::string GetName() const = 0;
    virtual bool IsRunning() const = 0;
};
