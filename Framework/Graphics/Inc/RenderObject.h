#pragma once

#include "MeshBuffer.h"
#include "Transform.h"
#include "Material.h"
#include "TextureManager.h"
#include "ModelManager.h"

namespace IExeEngine::Graphics
{
    struct Skeleton; // Forward declaration of the skeleton as we dont use the skeleton yet only its data.

    class RenderObject
    {
    public:
        void Terminate();

        Transform transform;   // Location/ Orientation
        MeshBuffer meshBuffer; // Shape

        Material material;    // Light data

        TextureId diffuseMapId;  // Diffuse texture for an object
        TextureId specMapId;   // Specular map for an object
        TextureId normalMapId;   // Normal texture for an object
        TextureId bumpMapId;   // Height texture for an object
    };

    class RenderGroup
    {
    public:
        void Initialize(const std::filesystem::path& modelFilePath);
        void Terminate();

        ModelId modelId; // Model Identifier
        Transform transform; // Root Transform (Other objects may have other transforms)
        std::vector<RenderObject> renderObjects; // All objects to render

        const Skeleton* skeleton; // Skeleton for animation
    };
}