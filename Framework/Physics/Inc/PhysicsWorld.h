#pragma once

#include "PhysicsDebugDraw.h"

namespace IExeEngine::Physics
{
    class PhysicsObject;

    class PhysicsWorld final
    {
    public:
        struct Settings
        {
            // Gravity used for Physiscs Simulations
            Math::Vector3 gravity = { 0.0f, -9.81f, 0.0f };
            // If update loops are too slow, this is the maximum simulations physics will run
            uint32_t simulationSteps = 10;
            // Fixed rate (60 FPS) that the simulations will run to ensure consistent/ predictable outcomes
            float fixedTimeStep = 1.0f / 60.0f;
        };

        static void StaticInitialize(const Settings& settings);
        static void StaticTerminate();
        static PhysicsWorld* Get();

        PhysicsWorld() = default;
        ~PhysicsWorld();

        void Initialize(const Settings& settings);
        void Terminate();

        void Update(float deltaTime);
        void DebugUI();

        void SetGravity(const Math::Vector3& gravity);

        void Register(PhysicsObject* physicsObject);
        void Unregister(PhysicsObject* physicsObject);

    private:
        Settings mSettings;

        // Bullet Objects
        btBroadphaseInterface* mInterface = nullptr;
        btCollisionDispatcher* mDispatcher = nullptr;
        btDefaultCollisionConfiguration* mCollisionConfig = nullptr;
        btSequentialImpulseConstraintSolver* mSolver = nullptr;
        // This is the main physics world that runs the simulations
#ifdef USE_SOFT_BODY
        friend class SoftBody;
        btSoftRigidDynamicsWorld* mDynamicsWorld = nullptr;
        btSoftRigidDynamicsWorld* GetSoftBodyWorld() { return mDynamicsWorld; }
#else
        btDiscreteDynamicsWorld* mDynamicsWorld = nullptr;
        btSoftRigidDynamicsWorld* GetSoftBodyWorld() { return nullptr; }
#endif // USE_SOFT_BODY

        using PhysicsObjects = std::vector<PhysicsObject*>;
        PhysicsObjects mPhysicsObjects;

        PhysicsDebugDraw mPhysicsDebugDraw;
        bool mDebugDraw = false;
    };
}