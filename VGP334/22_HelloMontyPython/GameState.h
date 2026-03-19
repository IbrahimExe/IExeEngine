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

    void UpdateCharacterAnimations(float time);

    IExeEngine::Graphics::Camera mCamera;
    IExeEngine::Graphics::DirectionalLight mDirectionalLight;

    // Forest Background
    IExeEngine::Graphics::RenderObject mForestSkybox;

    // Characters
    IExeEngine::Graphics::RenderGroup mGuard_01;
    IExeEngine::Graphics::RenderGroup mGuard_02;
    IExeEngine::Graphics::RenderGroup mKnight;

    IExeEngine::Graphics::RenderObject mGround;

    // Animators
    IExeEngine::Graphics::Animator mGuard_01Animator;
    IExeEngine::Graphics::Animator mGuard_02Animator;
    IExeEngine::Graphics::Animator mKnightAnimator;

    // Transform Animations
    IExeEngine::Graphics::Animation mCameraTransformAnimation;
    IExeEngine::Graphics::Animation mKnightTransformAnimation;

    // Terrain
    IExeEngine::Graphics::Terrain mTerrain;
    IExeEngine::Graphics::RenderObject mScreenQuad;

    // Effects
    IExeEngine::Graphics::StandardEffect mStandardEffect;
    IExeEngine::Graphics::ShadowEffect mShadowEffect;
    IExeEngine::Graphics::TerrainEffect mTerrainEffect;
    IExeEngine::Graphics::ParticleSystemEffect mParticleSystemEffect;

    // Particle Systems
    IExeEngine::Physics::ParticleSystem mBloodSplatter;

    // Audio
    IExeEngine::Audio::SoundId mAudioId = 0;

    // Animation Clip Indices (set these to your actual clip indices after loading)
    int mGuardIdleClip = 0;
    int mGuardLookingClip = 1;
    int mGuardConfusedClip = 2;
    int mGuardPointingClip = 3;
    int mGuardDeathClip = 4;
    int mKnightRunningClip = 0;
    int mKnightPunchClip = 1;

    // Animation state tracking
    int mCurrentGuard01Clip = -1;
    int mCurrentGuard02Clip = -1;
    int mCurrentKnightClip = -1;

    // Timer
    float mAnimationTimer = 0.0f;
    bool mPunchParticleSpawned = false;

    // Debug
    int mClipIndex = -1;
    float mAnimationSpeed = 1.0f;
    bool mDrawSkeleton = false;
};