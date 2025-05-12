#pragma once

#include "MeshTypes.h"

namespace IExeEngine::Graphics
{
    class MeshBuilder
    {
    public:
        // Cube
        static MeshPC CreateCubePC(float size, const Color& color);
        static MeshPC CreateCubePC(float size);
    };
}