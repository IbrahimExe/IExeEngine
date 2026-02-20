#include "Precompiled.h"
#include "Particle.h"

#include "PhysicsWorld.h"

using namespace IExeEngine;
using namespace IExeEngine::Physics;

void Particle::Initialize()
{ 
    mLifetime = 0.0f;
    mCollisionShape.InitializeEmpty();
    mRigidBody.Initialize(mTransform, mCollisionShape, 1.0f, false);
    mRigidBody.SetCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
}    
     
void Particle::Terminate()
{ 
    mRigidBody.Terminate();
    mCollisionShape.Terminate();
}  
     
void Particle::Activate(const ParticleInfo& info)
{ 
    mInfo = info;
    mLifetime = info.lifetime;
    if (info.lifetime > 0)
    {
        mRigidBody.Activate();
    }
    mRigidBody.SetPosition(info.position);
    mRigidBody.SetVelocity(info.velocity);
}    
     
void Particle::Update(float deltaTime)
{  
    if (IsActive())
    {
        mLifetime -= deltaTime;

        if (!IsActive())
        {
            mRigidBody.Deactivate();
        }
        else
        {
            const float t = 1.0f - Math::Clamp(mLifetime / mInfo.lifetime, 0.0f, 1.0f);
            mTransform.scale = Math::Lerp(mInfo.startScale, mInfo.endScale, t);
        }
    }
}    
     
bool Particle::IsActive() const
{
    return mLifetime > 0.0f;
}

const Graphics::Transform& Particle::GetTransform() const
{
    // TODO: insert return statement here
    return mTransform;
}

Graphics::Color Particle::GetColour() const
{
    const float t = 1.0f - Math::Clamp(mLifetime / mInfo.lifetime, 0.0f, 1.0f);
    return Math::Lerp(mInfo.startColour, mInfo.endColour, t);
}

const Math::Vector3& Particle::GetPosition() const
{
    // TODO: insert return statement here
    return mTransform.position;
}
