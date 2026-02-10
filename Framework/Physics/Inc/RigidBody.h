#pragma once

#include "PhysicsObject.h"

namespace IExeEngine::Physics
{
    class CollisionShape;

    class RigidBody final : public PhysicsObject
    {
    public:
        RigidBody() = default;
        ~RigidBody() override;

        void Initialize(Graphics::Transform& graphicsTransform, const CollisionShape& shape, float mass = 0.0f);
        void Terminate();

        void SetPosition(const Math::Vector3& position);

        void SetVelocity(const Math::Vector3& velocity);

        const Math::Vector3 GetVelocity() const;
    
        bool IsDynamic() const;

    private:
        void SyncWithGraphics() override;

        btRigidBody* GetRigidBody() override;

        btRigidBody* mRigidBody = nullptr;
        btDefaultMotionState* mMotionState = nullptr;
        float mMass = 0.0f;

        Graphics::Transform* mGraphicsTransform = nullptr;
    };
}