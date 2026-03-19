#include "GameState.h"

using namespace IExeEngine;
using namespace IExeEngine::Graphics;
using namespace IExeEngine::Input;

void GameState::Initialize()
{
    // Camera setup
    mCamera.SetPosition({ 0.0f, 5.0f, -20.0f });
    mCamera.SetLookAt({ 0.0f, 0.0f, 0.0f });

    // Directional Light
    mDirectionalLight.direction = Math::Normalize({ 0.490f, -0.490f, -0.710f });
    mDirectionalLight.ambient = { 0.5f, 0.5f, 0.5f, 1.0f };
    mDirectionalLight.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
    mDirectionalLight.specular = { 0.9f, 0.9f, 0.9f, 1.0f };

    // Load Skybox
    mForestSkybox.meshBuffer.Initialize(MeshBuilder::CreateSkySpherePX(25, 50, 400.0f));
    mForestSkybox.diffuseMapId = TextureManager::Get()->LoadTexture(L"../../Assets/Textures/forest_skybox.jpg");

    // Load Terrain
    mTerrain.Initialize(L"../../Assets/Textures/terrain/heightmap_1024x1024.raw", 20.0f);
    mGround.meshBuffer.Initialize(mTerrain.mesh);
    mGround.diffuseMapId = TextureManager::Get()->LoadTexture(L"../../Assets/Textures/terrain/grass_2048.jpg");
    mGround.specMapId = TextureManager::Get()->LoadTexture(L"../../Assets/Textures/terrain/dirt_seamless.jpg");
    mGround.transform.position = { -250.0f, -10.0f, -250.0f };

    // Load Models
    IExeEngine::Graphics::ModelManager* mm = IExeEngine::Graphics::ModelManager::Get();

    // Guard 1 
    mGuard_01.Initialize("../../Assets/Models/Guard_01/Model/Guard_01.model");
    mGuard_01.transform.position = { -2.0f, 0.0f, 0.0f };
    mGuard_01.transform.rotation = Math::Quaternion::CreateFromAxisAngle(Math::Vector3::YAxis, Math::Constants::Pi); 
    mGuard_01.animator = &mGuard_01Animator;
    mm->AddAnimation(mGuard_01.modelId, L"../../Assets/Models/Guard_01/Animations/Idle_Guard_01.animset");
    mm->AddAnimation(mGuard_01.modelId, L"../../Assets/Models/Guard_01/Animations/Looking_Guard_01.animset");
    mm->AddAnimation(mGuard_01.modelId, L"../../Assets/Models/Guard_01/Animations/LookBack_Guard_01.animset");
    mm->AddAnimation(mGuard_01.modelId, L"../../Assets/Models/Guard_01/Animations/Kill_Guard_01.animset");
    mGuard_01Animator.Initialize(mGuard_01.modelId);
    mGuard_01Animator.PlayAnimation(0, true);

    // Guard 2
    mGuard_02.Initialize("../../Assets/Models/Guard_02/Model/Guard_02.model");
    mGuard_02.transform.position = { 2.0f, 0.0f, 0.0f };
    mGuard_02.transform.rotation = Math::Quaternion::CreateFromAxisAngle(Math::Vector3::YAxis, Math::Constants::Pi); 
    mGuard_02.animator = &mGuard_02Animator;
    mm->AddAnimation(mGuard_02.modelId, L"../../Assets/Models/Guard_02/Animations/Pointing_Guard_02.animset");
    mm->AddAnimation(mGuard_02.modelId, L"../../Assets/Models/Guard_02/Animations/Looking_Guard_02.animset");
    mm->AddAnimation(mGuard_02.modelId, L"../../Assets/Models/Guard_02/Animations/Shrugging_Guard_02.animset");
    mm->AddAnimation(mGuard_02.modelId, L"../../Assets/Models/Guard_02/Animations/Reacting_Guard_02.animset");
    mGuard_02Animator.Initialize(mGuard_02.modelId);
    mGuard_02Animator.PlayAnimation(0, true);

    // Knight
    mKnight.Initialize("../../Assets/Models/Knight/Model/Knight.model");
    mKnight.transform.position = { 0.0f, 0.0f, 50.0f }; 
    mKnight.animator = &mKnightAnimator;
    mm->AddAnimation(mKnight.modelId, L"../../Assets/Models/Knight/Animations/GoofyRun_Knight.animset");
    mm->AddAnimation(mKnight.modelId, L"../../Assets/Models/Knight/Animations/Punch_Knight.animset");
    mKnightAnimator.Initialize(mKnight.modelId);
    mKnightAnimator.PlayAnimation(0, true);

    // animation clip indices
    mGuardIdleClip = 0;
    mGuardLookingClip = 1;
    mGuardConfusedClip = 2;
    mGuardDeathClip = 3;      // Guard 1's death animation
    mGuardPointingClip = 3;   // Guard 2's reacting animation
    mKnightRunningClip = 0;
    mKnightPunchClip = 1;

    // Knight Transform Animation
    mKnightTransformAnimation = AnimationBuilder()
        .AddPositionKey({ 0.0f, 0.0f, 50.0f }, 0.0f)      // Behind guards
        .AddPositionKey({ 0.0f, 0.0f, 49.0f }, 18.0f)     // Barely closer
        .AddPositionKey({ 0.0f, 0.0f, 48.0f }, 26.0f)     // Still barely closer
        .AddPositionKey({ 0.0f, 0.0f, 47.0f }, 32.0f)     // 
        .AddPositionKey({ 0.0f, 0.0f, 46.0f }, 40.0f).AddPositionKey({ 0.0f, 0.0f, 46.0f }, 42.98f)     // 
        .AddPositionKey({ 4.0f, 0.0f, 0.5f }, 42.99f)      // Tp 
        .AddPositionKey({ -2.0f, 0.0f, 0.0f }, 43.0f)     // attack at 43.5
        .AddPositionKey({ -25.0f, 0.0f, 0.0f }, 45.0f)    // Exit left

        // Event key for blood splatter particle
        .AddEventKey([this]() {
        mBloodSplatter.SetPositon({ -2.0f, 1.5f, 0.0f }); // Guard 1
        mBloodSplatter.SpawnParticles();
            }, 47.2f)

        .Build();

    // Blood Splatter Particle System
    mParticleSystemEffect.Initialize();
    mParticleSystemEffect.SetCamera(mCamera);

    IExeEngine::Physics::ParticleSystemInfo bloodInfo;
    bloodInfo.textureId = TextureManager::Get()->LoadTexture(L"../../Assets/Textures/blood.png");
    bloodInfo.maxParticles = 200;
    bloodInfo.particlesPerEmit = { 30, 50 }; 
    bloodInfo.delay = 99999.0f;
    bloodInfo.lifeTime = 0.0f; 
    bloodInfo.timeBetweenEmit = { 0.01f, 0.02f };
    bloodInfo.spawnAngle = { -90.0f, 90.0f }; 
    bloodInfo.spawnSpeed = { 5.0f, 10.0f }; 
    bloodInfo.particleLifeTime = { 1.0f, 2.0f }; 
    bloodInfo.spawnDirection = Math::Vector3{ -1.0f, 0.5f, 0.0f }; 
    bloodInfo.spawnPosition = Math::Vector3::Zero;
    bloodInfo.startScale = { Math::Vector3::One * 1.0f, Math::Vector3::One * 1.5f }; 
    bloodInfo.endScale = { Math::Vector3::One * 0.3f, Math::Vector3::One * 0.5f };
    bloodInfo.startColour = { Colors::Red, Colors::Red };
    bloodInfo.endColour = { Colors::Red * 0.5f, Colors::DarkRed }; 
    mBloodSplatter.Initialize(bloodInfo);

    // Audio
    IExeEngine::Audio::SoundEffectManager* sm = IExeEngine::Audio::SoundEffectManager::Get();
    mAudioId = sm->Load("MontyPython.wav");
    sm->Play(mAudioId, false); // Play once

    // Effects
    std::filesystem::path shaderFile = L"../../Assets/Shaders/Standard.fx";
    mStandardEffect.Initialize(shaderFile);
    mStandardEffect.SetCamera(mCamera);
    mStandardEffect.SetDirectionalLight(mDirectionalLight);

    mShadowEffect.Initialize();
    mShadowEffect.SetDirectionalLight(mDirectionalLight);

    mTerrainEffect.Initialize();
    mTerrainEffect.SetCamera(mCamera);
    mTerrainEffect.SetDirectionalLight(mDirectionalLight);
}

void GameState::Update(float deltaTime)
{
    mAnimationTimer += deltaTime;

    UpdateCamera(deltaTime);
    UpdateCharacterAnimations(mAnimationTimer);

    // Update animators
    mGuard_01Animator.Update(deltaTime);
    mGuard_02Animator.Update(deltaTime);
    mKnightAnimator.Update(deltaTime);

    // Update knight position based on animation
    auto knightTransform = mKnightTransformAnimation.GetTransform(mAnimationTimer);
    mKnight.transform.position = knightTransform.position;

    // Update particle system
    static bool particlesSpawned = false;
    if (mAnimationTimer >= 43.0f && mAnimationTimer < 43.5f && !particlesSpawned)
    {
        mBloodSplatter.SetPositon({ -2.0f, 1.5f, 0.0f });

        // Spawn multiple bursts
        for (int i = 0; i < 5; ++i)
        {
            mBloodSplatter.SpawnParticles();
        }

        particlesSpawned = true;
    }

    // Reset for restart
    if (mAnimationTimer < 1.0f)
    {
        particlesSpawned = false;
    }

    // Update particle system
    mBloodSplatter.Update(deltaTime);
}

void GameState::UpdateCamera(float deltaTime)
{
    Math::Vector3 cameraPosition;
    Math::Vector3 lookAtTarget;

    if (mAnimationTimer < 6.0f) // Shot 1: Both guards (0:00 - 0:06)
    {
        cameraPosition = { 0.0f, 2.5f, 10.0f };
        lookAtTarget = { 0.0f, 1.8f, 0.0f };
    }
    else if (mAnimationTimer < 8.0f) // Shot 2: Guard 2 close-up (0:06 - 0:08)
    {
        cameraPosition = { 2.5f, 1.8f, 1.5f };
        lookAtTarget = { 2.0f, 1.8f, 0.0f };
    }
    else if (mAnimationTimer < 11.50f) // Shot 3: Guard 1 close-up (0:08 - 00:11)
    {
        cameraPosition = { -2.5f, 1.8f, 1.5f };
        lookAtTarget = { -2.0f, 1.8f, 0.0f };
    }
    else if (mAnimationTimer < 16.90f) // Shot 4: Knight running - looking at knight (0:11 - 0:17)
    {
        cameraPosition = { 0.0f, 1.5f, 5.0f }; 
        lookAtTarget = { 0.0f, 1.0f, 50.0f };
    }
    else if (mAnimationTimer < 19.0f) // Shot 5: Guard 1 close-up (0:17 - 0:18)
    {
        cameraPosition = { -2.5f, 1.8f, 1.5f };
        lookAtTarget = { -2.0f, 1.8f, 0.0f };
    }
    else if (mAnimationTimer < 19.0f) // Shot 6: Guard 2 close-up (0:18 - 0:19)
    {
        cameraPosition = { 2.5f, 1.8f, 1.5f };
        lookAtTarget = { 2.0f, 1.8f, 0.0f };
    }
    else if (mAnimationTimer < 23.90f) // Shot 7: Knight running (0:19 - 0:24)
    {
        cameraPosition = { 0.0f, 1.5f, 5.0f };
        lookAtTarget = { 0.0f, 1.0f, 49.0f };
    }
    else if (mAnimationTimer < 24.90f) // Shot 8: Guard 2 close-up (0:24 - 0:24.9)
    {
        cameraPosition = { 2.5f, 1.8f, 1.5f };
        lookAtTarget = { 2.0f, 1.8f, 0.0f };
    }
    else if (mAnimationTimer < 26.20f) // Shot 9: Guard 1 close-up (0:25 - 0:26)
    {
        cameraPosition = { -2.5f, 1.8f, 1.5f };
        lookAtTarget = { -2.0f, 1.8f, 0.0f };
    }
    else if (mAnimationTimer < 29.0f) // Shot 10: Knight running (0:26.2 - 0:29)
    {
        cameraPosition = { 0.0f, 1.5f, 5.0f };
        lookAtTarget = { 0.0f, 1.0f, 48.0f };
    }
    else if (mAnimationTimer < 30.50f) // Shot 11: Guard 1 close-up (0:29 - 0:30.5)
    {
        cameraPosition = { -2.5f, 1.8f, 1.5f };
        lookAtTarget = { -2.0f, 1.8f, 0.0f };
    }
    else if (mAnimationTimer < 31.90f) // Shot 12: Guard 2 close-up (0:30.5 - 0:31.9)
    {
        cameraPosition = { 2.5f, 1.8f, 1.5f };
        lookAtTarget = { 2.0f, 1.8f, 0.0f };
    }
    else if (mAnimationTimer < 36.50f) // Shot 13: Knight running (0:31.9 - 0:36.5)
    {
        cameraPosition = { 0.0f, 1.5f, 5.0f };
        lookAtTarget = { 0.0f, 1.0f, 47.0f };
    }
    else if (mAnimationTimer < 38.90f) // Shot 14: Guard 2 close-up (0:36.5 - 0:37.2)
    {
        cameraPosition = { 2.5f, 1.8f, 1.5f };
        lookAtTarget = { 2.0f, 1.8f, 0.0f };
    }
    else if (mAnimationTimer < 40.00f) // Shot 15: Knight running (0:38.9 - 0:41.9)
    {
        cameraPosition = { 0.0f, 1.5f, 5.0f };
        lookAtTarget = { 0.0f, 1.0f, 46.0f };
    }
    else if (mAnimationTimer < 44.0f) // Shot 15: Guard 1 CU attack (0:40 - 0:435)
    {
        cameraPosition = { -2.5f, 1.8f, 1.5f }; // Cu 
        lookAtTarget = { -2.0f, 1.8f, 0.0f };
    }
    else // Shot 16: Both guards (0:44 - end)
    {
        cameraPosition = { 0.0f, 2.5f, 10.0f }; // Wide shot to see both guards
        lookAtTarget = { 0.0f, 1.8f, 0.0f };
    }

    // Apply hard cut
    mCamera.SetPosition(cameraPosition);
    mCamera.SetLookAt(lookAtTarget);
}

void GameState::UpdateCharacterAnimations(float time)
{
    // Guard 1 animations
    if (time >= 0.0f && time < 6.0f)
    {
        if (mCurrentGuard01Clip != mGuardIdleClip)
        {
            mGuard_01Animator.PlayAnimation(mGuardIdleClip, true);
            mCurrentGuard01Clip = mGuardIdleClip;
        }
    }
    else if (time >= 8.0f && time < 13.0f)
    {
        if (mCurrentGuard01Clip != mGuardLookingClip)
        {
            mGuard_01Animator.PlayAnimation(mGuardLookingClip, true);
            mCurrentGuard01Clip = mGuardLookingClip;
        }
    }
    else if (time >= 18.0f && time < 20.0f)
    {
        if (mCurrentGuard01Clip != mGuardLookingClip)
        {
            mGuard_01Animator.PlayAnimation(mGuardLookingClip, true);
            mCurrentGuard01Clip = mGuardLookingClip;
        }
    }
    else if (time >= 27.0f && time < 29.0f)
    {
        if (mCurrentGuard01Clip != mGuardConfusedClip)
        {
            mGuard_01Animator.PlayAnimation(mGuardConfusedClip, true);
            mCurrentGuard01Clip = mGuardConfusedClip;
        }
    }
    else if (time >= 32.0f && time < 34.0f)
    {
        if (mCurrentGuard01Clip != mGuardConfusedClip)
        {
            mGuard_01Animator.PlayAnimation(mGuardConfusedClip, true);
            mCurrentGuard01Clip = mGuardConfusedClip;
        }
    }
    else if (time >= 41.0f && time < 43.5f)
    {
        if (mCurrentGuard01Clip != mGuardLookingClip)
        {
            mGuard_01Animator.PlayAnimation(mGuardLookingClip, true);
            mCurrentGuard01Clip = mGuardLookingClip;
        }
    }
    else if (time >= 43.5f) // Death at 43.5s
    {
        if (mCurrentGuard01Clip != mGuardDeathClip)
        {
            mGuard_01Animator.PlayAnimation(mGuardDeathClip, false);
            mCurrentGuard01Clip = mGuardDeathClip;
        }
    }

    // Guard 2 animations
    if (time >= 0.0f && time < 6.0f)
    {
        if (mCurrentGuard02Clip != mGuardIdleClip)
        {
            mGuard_02Animator.PlayAnimation(mGuardIdleClip, true);
            mCurrentGuard02Clip = mGuardIdleClip;
        }
    }
    else if (time >= 6.0f && time < 8.0f)
    {
        if (mCurrentGuard02Clip != mGuardLookingClip)
        {
            mGuard_02Animator.PlayAnimation(mGuardLookingClip, true);
            mCurrentGuard02Clip = mGuardLookingClip;
        }
    }
    else if (time >= 20.0f && time < 22.0f)
    {
        if (mCurrentGuard02Clip != mGuardConfusedClip)
        {
            mGuard_02Animator.PlayAnimation(mGuardConfusedClip, true);
            mCurrentGuard02Clip = mGuardConfusedClip;
        }
    }
    else if (time >= 26.0f && time < 27.0f)
    {
        if (mCurrentGuard02Clip != mGuardConfusedClip)
        {
            mGuard_02Animator.PlayAnimation(mGuardConfusedClip, true);
            mCurrentGuard02Clip = mGuardConfusedClip;
        }
    }
    else if (time >= 34.0f && time < 36.0f)
    {
        if (mCurrentGuard02Clip != mGuardConfusedClip)
        {
            mGuard_02Animator.PlayAnimation(mGuardConfusedClip, true);
            mCurrentGuard02Clip = mGuardConfusedClip;
        }
    }
    else if (time >= 40.0f && time < 41.0f)
    {
        if (mCurrentGuard02Clip != mGuardConfusedClip)
        {
            mGuard_02Animator.PlayAnimation(mGuardConfusedClip, true);
            mCurrentGuard02Clip = mGuardConfusedClip;
        }
    }
    else if (time >= 43.5f) // Pointing/reacting at 43.5s
    {
        if (mCurrentGuard02Clip != mGuardPointingClip)
        {
            mGuard_02Animator.PlayAnimation(mGuardPointingClip, true);
            mCurrentGuard02Clip = mGuardPointingClip;
        }
    }

    // Knight animations
    if (time >= 0.0f && time < 43.0f) // Running
    {
        if (mCurrentKnightClip != mKnightRunningClip)
        {
            mKnightAnimator.PlayAnimation(mKnightRunningClip, true);
            mCurrentKnightClip = mKnightRunningClip;
        }
    }
    else if (time >= 43.0f && time < 44.0f) // Punch
    {
        if (mCurrentKnightClip != mKnightPunchClip)
        {
            mKnightAnimator.PlayAnimation(mKnightPunchClip, false);
            mCurrentKnightClip = mKnightPunchClip;
        }
    }
    else if (time >= 44.0f) // Running away
    {
        if (mCurrentKnightClip != mKnightRunningClip)
        {
            mKnightAnimator.PlayAnimation(mKnightRunningClip, true);
            mCurrentKnightClip = mKnightRunningClip;
        }
    }
}

void GameState::Render()
{
    IExeEngine::Graphics::SimpleDraw::Render(mCamera);

    //----------------------------------------------------------
    // First Pass: Render to Shadow Map
    //----------------------------------------------------------
    mShadowEffect.Begin();
        mShadowEffect.Render(mGuard_01);
        mShadowEffect.Render(mGuard_02);
        mShadowEffect.Render(mKnight);
    mShadowEffect.End();

    //----------------------------------------------------------
    // Second Pass: Render Terrain
    //----------------------------------------------------------
    mTerrainEffect.Begin();
        mTerrainEffect.Render(mGround);
    mTerrainEffect.End();

    //----------------------------------------------------------
    // Third Pass: Render Scene Objects
    //----------------------------------------------------------
    mStandardEffect.Begin();
        mStandardEffect.Render(mForestSkybox);
        // Render Characters
        mStandardEffect.Render(mGuard_01);
        mStandardEffect.Render(mGuard_02);
        mStandardEffect.Render(mKnight);
    mStandardEffect.End();

    //----------------------------------------------------------
    // Fourth Pass: Render Particles (Blood Splatter)
    //----------------------------------------------------------
    mParticleSystemEffect.Begin();
        mBloodSplatter.Render(mParticleSystemEffect);
    mParticleSystemEffect.End();
}

void GameState::DebugUI()
{
    ImGui::Begin("Animation Controls");

    // Timer Display
    ImGui::Text("Animation Timer: %.2f seconds", mAnimationTimer);
    ImGui::Separator();

    // Restart Button
    if (ImGui::Button("Restart Animation"))
    {
        mAnimationTimer = 0.0f;

        // Reset audio
        IExeEngine::Audio::SoundEffectManager* sm = IExeEngine::Audio::SoundEffectManager::Get();
        sm->Stop(mAudioId);
        sm->Play(mAudioId, false);

        // Reset character animations
        mGuard_01Animator.PlayAnimation(mGuardIdleClip, true);
        mGuard_02Animator.PlayAnimation(mGuardIdleClip, true);
        mKnightAnimator.PlayAnimation(mKnightRunningClip, true);

        // Reset positions
        mGuard_01.transform.position = { -2.0f, 0.0f, 0.0f };
        mGuard_02.transform.position = { 2.0f, 0.0f, 0.0f };
        mKnight.transform.position = { 0.0f, 0.0f, 50.0f };

        // Reset animation state tracking
        mCurrentGuard01Clip = -1;
        mCurrentGuard02Clip = -1;
        mCurrentKnightClip = -1;
    }

    ImGui::Separator();

    // Debug Options
    ImGui::Checkbox("Draw Skeleton", &mDrawSkeleton);

    ImGui::Separator();

    // Current Shot Info
    int currentShot = 0;
    if (mAnimationTimer < 6.0f) currentShot = 1;
    else if (mAnimationTimer < 8.0f) currentShot = 2;
    else if (mAnimationTimer < 13.0f) currentShot = 3;
    else if (mAnimationTimer < 18.0f) currentShot = 4;
    else if (mAnimationTimer < 20.0f) currentShot = 5;
    else if (mAnimationTimer < 22.0f) currentShot = 6;
    else if (mAnimationTimer < 26.0f) currentShot = 7;
    else if (mAnimationTimer < 27.0f) currentShot = 8;
    else if (mAnimationTimer < 29.0f) currentShot = 9;
    else if (mAnimationTimer < 32.0f) currentShot = 10;
    else if (mAnimationTimer < 34.0f) currentShot = 11;
    else if (mAnimationTimer < 36.0f) currentShot = 12;
    else if (mAnimationTimer < 40.0f) currentShot = 13;
    else if (mAnimationTimer < 41.0f) currentShot = 14;
    else if (mAnimationTimer < 43.0f) currentShot = 15;
    else if (mAnimationTimer < 46.0f) currentShot = 16;
    else if (mAnimationTimer < 47.0f) currentShot = 17;
    else currentShot = 18;

    ImGui::Text("Current Shot: %d / 18", currentShot);

    ImGui::Separator();

    // Character Positions
    if (ImGui::CollapsingHeader("Character Positions"))
    {
        ImGui::Text("Guard 1: (%.2f, %.2f, %.2f)",
            mGuard_01.transform.position.x,
            mGuard_01.transform.position.y,
            mGuard_01.transform.position.z);

        ImGui::Text("Guard 2: (%.2f, %.2f, %.2f)",
            mGuard_02.transform.position.x,
            mGuard_02.transform.position.y,
            mGuard_02.transform.position.z);

        ImGui::Text("Knight: (%.2f, %.2f, %.2f)",
            mKnight.transform.position.x,
            mKnight.transform.position.y,
            mKnight.transform.position.z);
    }

    ImGui::Separator();

    // Effects Debug UI
    mStandardEffect.DebugUI();
    mShadowEffect.DebugUI();
    mTerrainEffect.DebugUI();
    mParticleSystemEffect.DebugUI();
    mBloodSplatter.DebugUI();

    ImGui::End();
}

void GameState::Terminate()
{
    // Stop audio
    IExeEngine::Audio::SoundEffectManager* sm = IExeEngine::Audio::SoundEffectManager::Get();
    sm->Stop(mAudioId);

    // Terminate particle systems
    mBloodSplatter.Terminate();
    mParticleSystemEffect.Terminate();

    // Terminate effects
    mStandardEffect.Terminate();
    mShadowEffect.Terminate();

    // Terminate terrain
    mTerrainEffect.Terminate();

    // Terminate render objects
    mGround.Terminate();
    mForestSkybox.Terminate();

    // Terminate characters
    mGuard_01.Terminate();
    mGuard_02.Terminate();
    mKnight.Terminate();
}