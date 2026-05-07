#include "Precompiled.h"
#include "PhysicsService.h"
#include "RigidBodyComponent.h"

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