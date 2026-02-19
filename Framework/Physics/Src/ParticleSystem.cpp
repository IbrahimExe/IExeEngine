#include "Precompiled.h"
#include "ParticleSystem.h"

using namespace IExeEngine;
using namespace IExeEngine::Physics;

void ParticleSystem::Initialize(const ParticleSystemInfo& info)
{
    mInfo = info;
    mNextAvailableParticleIndex = 0;
    mNextSpawnTime = info.delay;
    mLifeTime = info.lifeTime;

    InitializeParticles(info.maxParticles);
}

void ParticleSystem::Terminate()
{
    for (auto& particle : mParticles)
    {
        particle->Terminate();
        particle.reset();
    }
    mParticles.clear();
}

void ParticleSystem::Update(float deltaTime)
{
    if (IsActive())
    {
        mLifeTime -= deltaTime;
        mNextSpawnTime -= deltaTime;
        if (mNextSpawnTime <= 0.0f && mLifeTime > 0.0f)
        {
            SpawnParticles();
        }
        for (auto& particle : mParticles)
        {
            particle->Update(deltaTime);
        }
    }
}

bool ParticleSystem::IsActive()
{
    if (mLifeTime > 0.0f)
    {
        return true;
    }
    for (auto& particle : mParticles)
    {
        if (particle->IsActive())
        {
            return true;
        }
    }
    return false;
}

void ParticleSystem::DebugUI()
{

}

void ParticleSystem::SpawnParticles()
{

}

void ParticleSystem::Render()
{

}

void ParticleSystem::InitializeParticles(uint32_t maxParticles)
{

}

void ParticleSystem::SpawnSingleParticle()
{

}