#pragma once

namespace IExeEngine::Graphics
{
    struct Bone 
    {
        std::string name;
        int index = -1;

        Bone* parent = nullptr;
        int parentIndex = -1;

        std::vector<Bone*> children;
        std::vector<int> childrenIndices;

        Math::Matrix4 toParentTransform;
        Math::Matrix4 offsetTransform;
    };
}