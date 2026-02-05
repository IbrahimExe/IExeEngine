#pragma once

// An object that connects to the physics world
// Must be inherited to register with the world

namespace IExeEngine::Physics
{
    class PhysicsObject
    {
    public:
        PhysicsObject() = default;
        virtual ~PhysicsObject() = default;

    protected:
        friend class PhysicsWorld;

        // Abstract Function (
        virtual void SyncWithGraphics() = 0;
        virtual btRigidBody* GetRigidBody() { return nullptr; } // Not all objects will have rigid bodies
    };
}