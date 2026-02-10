#pragma once

namespace IExeEngine::Physics
{
    class CollisionShape final
    {
    public:
        CollisionShape() = default;
        ~CollisionShape();

        void InitializeEmpty();

        void InitializeSphere(float radius);

        void InitializeCapsule(float radius, float height);

        void InitializeBox(const Math::Vector3& halfExtents);

        void InitializeHull(const Math::Vector3& halfExtents, const Math::Vector3& origin);

        void Terminate();

    private:
        friend class RigidBody;
        btCollisionShape* mCollisionShape = nullptr;
    };
}