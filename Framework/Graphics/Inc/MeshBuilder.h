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

        // Pyramid
        static MeshPC CreatePyramid(float size);

        // Rectangle
        static MeshPC CreateRectanglePC(float width, float height, float depth);
    };
}