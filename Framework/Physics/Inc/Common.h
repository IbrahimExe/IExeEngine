#pragma once

// ANything thats not part of this engine that we wanna include

// Framework Headers:
#include <Math/Inc/DWMath.h>
#include <Core/Inc/Core.h>
#include <Graphics/Inc/Graphics.h>

// Bullet Headers
#include <Bullet/btBulletCollisionCommon.h>
#include <Bullet/btBulletDynamicsCommon.h>

// Softbody Headers
#include <Bullet/BulletSoftBody/btSoftRigidDynamicsWorld.h>
#include <Bullet/BulletSoftBody/btSoftBodyHelpers.h>
#include <Bullet/BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h>
#include <Bullet/BulletSoftBody/btSoftBodySolvers.h>
// If we want to use softbody code, we enable this, comment it out if we dont
#define USE_SOFT_BODY

// Helper Functions
template<class T>
inline void SafeDelete(T*& ptr)
{
    if (ptr)
    {
        delete ptr;
        ptr = nullptr;
    }
}

inline btVector3 ToBtVector3(const Math::Vector3& v)
{
    return btVector3(v.x, v.y, v.z);
}

inline Math::Vector3 ToVector3(const btVector3& v)
{
    return {
        static_cast<float>(v.getX()),
        static_cast<float>(v.getY()),
        static_cast<float>(v.getZ())
    };
}

inline btQuaternion ToBtQuaternion(const Math::Quaternion& q)
{
    return btQuaternion(q.x, q.y, q.z, q.w);
}

inline Math::Quaternion ToQuaternion(const btQuaternion& q)
{
    return {
        static_cast<float>(q.getX()),
        static_cast<float>(q.getY()),
        static_cast<float>(q.getZ()),
        static_cast<float>(q.getW())
    };
}

inline Graphics::Color ToColor(const btVector3& c)
{
    return {
        static_cast<float>(c.getX()),
        static_cast<float>(c.getY()),
        static_cast<float>(c.getZ()),
        1.0f
    };
}

inline btTransform ConvertToBtTransform(const Graphics::Transform& t)
{
    return btTransform(
        ToBtQuaternion(t.rotation),
        ToBtVector3(t.position)
    );
}