#pragma once

#include "Component.h"

namespace IExeEngine
{
    class TransformComponent final : public Component, public Graphics::Transform
    {
    public:
        SET_TYPE_ID(ComponentId::Transform);
    };
}
