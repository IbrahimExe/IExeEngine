#include "Precompiled.h"
#include "RigidBody.h"
#include "CollisionShape.h"
#include "PhysicsWorld.h"

using namespace IExeEngine;
using namespace IExeEngine::Physics;

RigidBody::~RigidBody()
{
    ASSERT(mRigidBody == nullptr, "RigidBody: Terminate must be called!");
}

void RigidBody::Initialize(Graphics::Transform& graphicsTransform, const CollisionShape& shape, float mass, bool addToWorld)
{
	mGraphicsTransform = &graphicsTransform;
	mMass = mass;

	btVector3 localInertia = btVector3();
    //shape.mCollisionShape->calculateLocalInertia(mass, localInertia); // If you dont want it to tip over, set local inertia to 0,0,0

	mMotionState = new btDefaultMotionState(ConvertToBtTransform(graphicsTransform));
	mRigidBody = new btRigidBody(mMass, mMotionState, shape.mCollisionShape, localInertia);

    if (addToWorld)
    {
		PhysicsWorld::Get()->Register(this);
    }
}

void RigidBody::Terminate()
{
    PhysicsWorld::Get()->Unregister(this); // Must unregister before deleting the rigid body or it will cause a crash in the physics world update loop
	SafeDelete(mRigidBody);
	SafeDelete(mMotionState);
}

void RigidBody::Activate()
{
    PhysicsWorld::Get()->Register(this);
    mRigidBody->activate();
}

void RigidBody::Deactivate()
{
    PhysicsWorld::Get()->Unregister(this);
}

void RigidBody::SetCollisionFlags(int flags)
{
	mRigidBody->setCollisionFlags(flags);
}

void RigidBody::SetPosition(const Math::Vector3& position)
{
	mRigidBody->activate();
	mGraphicsTransform->position = position;
	mRigidBody->setWorldTransform(ConvertToBtTransform(*mGraphicsTransform));
}

void RigidBody::SetVelocity(const Math::Vector3& velocity)
{
	mRigidBody->activate();
	mRigidBody->setLinearVelocity(ToBtVector3(velocity));
}

const Math::Vector3 RigidBody::GetVelocity() const
{
	return ToVector3(mRigidBody->getLinearVelocity());
}

bool RigidBody::IsDynamic() const
{
	return mMass > 0.0f;
}

void RigidBody::SyncWithGraphics()
{
	const btTransform& worldTransform = mRigidBody->getWorldTransform();
	mGraphicsTransform->position = ToVector3(worldTransform.getOrigin());
	mGraphicsTransform->rotation = ToQuaternion(worldTransform.getRotation());
}

btRigidBody* RigidBody::GetRigidBody()
{
	return mRigidBody;
}
