#pragma once

namespace IExeEngine::Graphics
{
    struct Keyframe
    {
        float timePos;
        Math::Vector3 translation;
        Math::Quaternion rotation;
        Math::Vector3 scale;
    };
}