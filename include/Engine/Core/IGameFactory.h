#pragma once
#include "Engine/Core/Game.h"
#include <memory>
#include <string>

class IGameFactory {
public:
    virtual ~IGameFactory() = default;
    virtual std::string GetName() const = 0;
    virtual std::unique_ptr<Game> Create() = 0;
};
