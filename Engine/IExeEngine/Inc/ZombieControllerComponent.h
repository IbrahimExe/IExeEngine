#pragma once

#include "Component.h"

namespace IExeEngine
{
    class ZombieControllerComponent : public Component
    {
    public:
        SET_TYPE_ID(ComponentId::ZombieController);
    };
}