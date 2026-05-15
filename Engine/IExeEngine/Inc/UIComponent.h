#pragma once

#include "Component.h"

namespace IExeEngine
{
    class UIComponent : public Component
    {
    public:
        virtual void Render() = 0;
    };
}