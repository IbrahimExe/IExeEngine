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
        static MeshPC CreatePyramidPC(float size);

        // Rectangle
        static MeshPC CreateRectanglePC(float width, float height, float depth);

        // Plane
        static MeshPC CreatePlanePC(int numRows, int numColums, float spacing, bool horizontal = true);

        // Cylinder
        static MeshPC CreateCylinderPC(int slices, int rings);

        // Sphere
        static MeshPC CreateSpherePC(int slices, int rings, float radius);
    };
}