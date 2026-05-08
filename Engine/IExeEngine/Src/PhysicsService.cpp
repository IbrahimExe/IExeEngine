#include "Precompiled.h"
#include "PhysicsService.h"
#include "RigidBodyComponent.h"
#include "SaveUtil.h"

using namespace IExeEngine;

void PhysicsService::Update(float deltaTime)
{
	if (mEnabled)
	{
		Physics::PhysicsWorld::Get()->Update(deltaTime);
	}
}

void PhysicsService::DebugUI()
{
	if (mEnabled)
	{
		Physics::PhysicsWorld::Get()->DebugUI();
	}
}

void PhysicsService::Deserialize(const rapidjson::Value& value)
{
	Physics::PhysicsWorld::Settings settings;
	int simSteps = settings.simulationSteps;
	SaveUtil::ReadVector3("Gravity", settings.gravity, value);
	SaveUtil::ReadInt("SimSteps", simSteps, value);
	SaveUtil::ReadFloat("FixedTimeStep", settings.fixedTimeStep, value);
	settings.simulationSteps = simSteps;
	Physics::PhysicsWorld::Get()->UpdateSettings(settings);
}

void PhysicsService::Register(RigidBodyComponent* rigidBodyComponent)
{
	Physics::PhysicsWorld::Get()->Register(&rigidBodyComponent->mRigidBody);
}

void PhysicsService::Unregister(RigidBodyComponent* rigidBodyComponent)
{
	Physics::PhysicsWorld::Get()->Unregister(&rigidBodyComponent->mRigidBody);
}

void PhysicsService::SetEnabled(bool enabled)
{
	mEnabled = enabled;
}