#include "Precompiled.h"
#include "PhysicsWorld.h"
#include "PhysicsObject.h"

using namespace IExeEngine;
using namespace IExeEngine::Physics;

namespace
{
    std::unique_ptr<PhysicsWorld> sPhysicsWorld;
}

void PhysicsWorld::StaticInitialize(const Settings& settings)
{
    ASSERT(sPhysicsWorld == nullptr, "PhysicsWorld: Already Initialized!");
    sPhysicsWorld = std::make_unique<PhysicsWorld>();
    sPhysicsWorld->Initialize(settings);
}

void PhysicsWorld::StaticTerminate()
{
    if (sPhysicsWorld != nullptr)
    {
        sPhysicsWorld->Terminate();
        sPhysicsWorld.reset();
    }
}

PhysicsWorld* PhysicsWorld::Get()
{
    ASSERT(sPhysicsWorld != nullptr, "PhysicsWorld: Must be Initialized!");
    return sPhysicsWorld.get();
}

PhysicsWorld::~PhysicsWorld()
{
    ASSERT(mDynamicsWorld == nullptr, "PhysicsWorld: Must be Terminated!");
}

void PhysicsWorld::Initialize(const Settings& settings)
{
    mSettings = settings;
    mInterface = new btDbvtBroadphase();
    mSolver = new btSequentialImpulseConstraintSolver();

#ifdef USE_SOFT_BODY
    mCollisionConfig = new btSoftBodyRigidBodyCollisionConfiguration();
    mDispatcher = new btCollisionDispatcher(mCollisionConfig);
    mDynamicsWorld = new btSoftRigidDynamicsWorld
    (
        mDispatcher,
        mInterface,
        mSolver,
        mCollisionConfig
    );
#else
    mCollisionConfig = new btDefaultCollisionConfiguration();
    mDispatcher = new btCollisionDispatcher(mCollisionConfig);
    mDynamicsWorld = new btDiscreteDynamicsWorld
    (
        mDispatcher,
        mInterface,
        mSolver,
        mCollisionConfig
    );
#endif // USE_SOFT_BODY

    mDynamicsWorld->setGravity(ToBtVector3(settings.gravity));

    mDynamicsWorld->setDebugDrawer(&mPhysicsDebugDraw);
}

void PhysicsWorld::Terminate()
{
    // First In Last Out
    SafeDelete(mDynamicsWorld);
    SafeDelete(mDispatcher);
    SafeDelete(mCollisionConfig);
    SafeDelete(mSolver);
    SafeDelete(mInterface);
}

void PhysicsWorld::Update(float deltaTime)
{
    mDynamicsWorld->stepSimulation(deltaTime, mSettings.simulationSteps, mSettings.fixedTimeStep);
    for (PhysicsObject* obj : mPhysicsObjects)
    {
        obj->SyncWithGraphics();
    }
}

void PhysicsWorld::DebugUI()
{
    if (ImGui::CollapsingHeader("Physics", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::DragFloat3("Gravity", &mSettings.gravity.x, 0.1f))
        {
            mDynamicsWorld->setGravity(ToBtVector3(mSettings.gravity));
        }

        ImGui::Checkbox("DebugDraw", &mDebugDraw);

        if (mDebugDraw)
        {
            ImGui::Indent();

            int debugMode = mPhysicsDebugDraw.getDebugMode();
            bool drawWireframe = (debugMode & btIDebugDraw::DBG_DrawWireframe);
            if (ImGui::Checkbox("DrawWireframe", &drawWireframe))
            {
                debugMode = (drawWireframe) ? (debugMode | btIDebugDraw::DBG_DrawWireframe) : (debugMode & ~btIDebugDraw::DBG_DrawWireframe);
            }

            bool drawAabb = (debugMode & btIDebugDraw::DBG_DrawAabb);
            if (ImGui::Checkbox("DrawAabb", &drawAabb))
            {
                debugMode = (drawAabb) ? (debugMode | btIDebugDraw::DBG_DrawAabb) : (debugMode & ~btIDebugDraw::DBG_DrawAabb);
            }

            bool drawContactPoints = (debugMode & btIDebugDraw::DBG_DrawContactPoints);
            if (ImGui::Checkbox("DrawContactPoints", &drawContactPoints))
            {
                debugMode = (drawContactPoints) ? (debugMode | btIDebugDraw::DBG_DrawContactPoints) : (debugMode & ~btIDebugDraw::DBG_DrawContactPoints);
            }

            mPhysicsDebugDraw.setDebugMode(debugMode);
            mDynamicsWorld->debugDrawWorld();

            ImGui::Unindent();
        }
    }
}

void PhysicsWorld::SetGravity(const Math::Vector3& gravity)
{
    mSettings.gravity = gravity;
    mDynamicsWorld->setGravity(ToBtVector3(gravity));
}

void PhysicsWorld::Register(PhysicsObject* physicsObject)
{
    auto iter = std::find(mPhysicsObjects.begin(), mPhysicsObjects.end(), physicsObject);
    // If iter is the end, it is NOT in the list, register means we want to add it, so it is safe to add now
    if (iter == mPhysicsObjects.end())
    {
        mPhysicsObjects.push_back(physicsObject);
#ifdef USE_SOFT_BODY
        if (physicsObject->GetSoftBody() != nullptr)
        {
            mDynamicsWorld->addSoftBody(physicsObject->GetSoftBody());
        }
#endif // USE_SOFT_BODY

        if (physicsObject->GetRigidBody() != nullptr)
        {
            mDynamicsWorld->addRigidBody(physicsObject->GetRigidBody());
        }
    }
}

void PhysicsWorld::Unregister(PhysicsObject* physicsObject)
{
    auto iter = std::find(mPhysicsObjects.begin(), mPhysicsObjects.end(), physicsObject);
    // If iter is NOT the end, it IS in the list, unregister means we want to remove it, so it is safe to remove now
    if (iter != mPhysicsObjects.end())
    {
#ifdef USE_SOFT_BODY
        if (physicsObject->GetSoftBody() != nullptr)
        {
            mDynamicsWorld->removeSoftBody(physicsObject->GetSoftBody());
        }
#endif // USE_SOFT_BODY
        if (physicsObject->GetRigidBody() != nullptr)
        {
            mDynamicsWorld->removeRigidBody(physicsObject->GetRigidBody());
        }
        mPhysicsObjects.erase(iter);
    }
}