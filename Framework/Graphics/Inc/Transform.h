#pragma once

namespace IExeEngine::Graphics
{
    struct Transform
    {
        Math::Vector3 position = Math::Vector3::Zero;
        Math::Quaternion rotation = Math::Quaternion::Identity;
        Math::Vector3 scale = Math::Vector3::One;

        Math::Matrix4 GetMatrix4() const
        {   
            // S * R * P
            return {
                Math::Matrix4::Scaling(scale) *
                Math::Matrix4::MatrixRotationQuaternion(rotation) *
                Math::Matrix4::Translation(position)
            };
        }
    };
}
