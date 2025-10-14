#pragma once

#include "MeshBuffer.h"
#include "Transform.h"

namespace IExeEngine::Graphics
{
    class RenderObject
    {
    public:
        void Terminate();

        Transform transform;
        MeshBuffer meshBuffer;
    };
}