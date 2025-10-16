#pragma once

#include "MeshBuffer.h"
#include "Transform.h"
#include "Material.h"

namespace IExeEngine::Graphics
{
    class RenderObject
    {
    public:
        void Terminate();

        Transform transform;   // Location/ Orientation
        MeshBuffer meshBuffer; // Shape

        Material material;    // Light data
    };
}