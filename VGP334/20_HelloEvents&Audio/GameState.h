#pragma once

#include <IExeEngine/Inc/IExeEngine.h>

class GameState : public IExeEngine::AppState
{
public:
	void Initialize() override;

	void Terminate() override;
	
	void Update(float deltaTime) override;

	void Render() override;

	void DebugUI() override;

private:

	void UpdateCamera(float deltaTime);

    void OnSpacePressedEvent(const IExeEngine::Core::Event& e);

	IExeEngine::Graphics::Camera mCamera;
    IExeEngine::Graphics::DirectionalLight mDirectionalLight;

    // Events
    IExeEngine::Core::EventListenerId mSpacePressedListenerId = 0;
    IExeEngine::Core::EventListenerId mEnterPressedListenerId = 0;

    // Particles
    IExeEngine::Graphics::ParticleSystemEffect mParticleSystemEffect;
    IExeEngine::Physics::ParticleSystem mParticleSystem;
    IExeEngine::Physics::ParticleSystem mFireworkParticles;

    // Animation
    IExeEngine::Graphics::Animation mFireworkAnimation;
    float mFireworkAnimationTime = 0.0f;

    // Ball info
    IExeEngine::Graphics::RenderObject mFootball;
	IExeEngine::Physics::CollisionShape mBallShape;
    IExeEngine::Physics::RigidBody mBallRigidBody;

    // Ground info
    IExeEngine::Graphics::RenderObject mGroundObject;
    IExeEngine::Physics::CollisionShape mGroundShape;
    IExeEngine::Physics::RigidBody mGroundRigidBody;

	struct BoxData
	{
        IExeEngine::Graphics::RenderObject box;
        IExeEngine::Physics::CollisionShape shape;
        IExeEngine::Physics::RigidBody rigidBody;
	};
    using Boxes = std::vector<BoxData>;
    Boxes mBoxes;

	IExeEngine::Graphics::StandardEffect mStandardEffect;

	// Cloth info
	IExeEngine::Graphics::RenderObject mCloth;
    IExeEngine::Graphics::Mesh mClothMesh;
    IExeEngine::Physics::SoftBody mClothSoftBody;

    // Sound
    IExeEngine::Audio::SoundId mLaunchSoundId = 0;
    IExeEngine::Audio::SoundId mExplosionSoundId = 0;
};