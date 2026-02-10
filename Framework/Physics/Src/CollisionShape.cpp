#include "Precompiled.h"
#include "CollisionShape.h"

using namespace IExeEngine;
using namespace IExeEngine::Physics;

CollisionShape::~CollisionShape()
{
    ASSERT(mCollisionShape == nullptr, "CollisionShape: Terminate must be called!");
}

void CollisionShape::InitializeEmpty()
{
    ASSERT(mCollisionShape == nullptr, "CollisionShape: Terminate must be called!");
    mCollisionShape = new btEmptyShape();
}

void CollisionShape::InitializeSphere(float radius)
{
    ASSERT(mCollisionShape == nullptr, "CollisionShape: Terminate must be called!");
    mCollisionShape = new btSphereShape(radius);
}

void CollisionShape::InitializeCapsule(float radius, float height)
{
    ASSERT(mCollisionShape == nullptr, "CollisionShape: Terminate must be called!");
    mCollisionShape = new btCapsuleShape(radius, height);
}

void CollisionShape::InitializeBox(const Math::Vector3& halfExtents)
{
    ASSERT(mCollisionShape == nullptr, "CollisionShape: Terminate must be called!");
    mCollisionShape = new btBoxShape(ToBtVector3(halfExtents));
}

void CollisionShape::InitializeHull(const Math::Vector3& halfExtents, const Math::Vector3& origin)
{
    ASSERT(mCollisionShape == nullptr, "CollisionShape: Terminate must be called!");
    btConvexHullShape* hullShape = new btConvexHullShape();
    std::vector<Math::Vector3> points =
    {
        {-halfExtents.x, -halfExtents.y, -halfExtents.z},
        {-halfExtents.x,  halfExtents.y, -halfExtents.z},
        { halfExtents.x,  halfExtents.y, -halfExtents.z},
        { halfExtents.x, -halfExtents.y, -halfExtents.z},
        {-halfExtents.x, -halfExtents.y,  halfExtents.z},
        {-halfExtents.x,  halfExtents.y,  halfExtents.z},
        { halfExtents.x,  halfExtents.y,  halfExtents.z},
        { halfExtents.x, -halfExtents.y,  halfExtents.z}
    };
    for (Math::Vector3& point : points)
    {
        hullShape->addPoint(ToBtVector3(point + origin), false);
    }
    hullShape->recalcLocalAabb();
    mCollisionShape = hullShape;
}

void CollisionShape::Terminate()
{
    SafeDelete(mCollisionShape);
}
