#pragma once

#include "Keyframe.h"
#include "Transform.h"

namespace IExeEngine::Graphics
{
    class Animation
    {
    public:
        Transform GetTransform(float time) const;
        float GetDuration() const;

    private:
        Math::Vector3 GetPosition(float time) const;
        Math::Quaternion GetRotation(float time) const;
        Math::Vector3 GetScale(float time) const;

        friend class AnimationBuilder;
        friend class AnimationIO; // Forward declaration to allow access to private members "Worry about it later"

        PositionKeys mPositionKeys;
        RotationKeys mRotationKeys;
        ScaleKeys mScaleKeys;
        float mDuration = 0.0f;
    };
}