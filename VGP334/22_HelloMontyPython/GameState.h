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

	IExeEngine::Graphics::Camera mCamera;
    IExeEngine::Graphics::DirectionalLight mDirectionalLight;

    // Sky sphere
    IExeEngine::Graphics::RenderObject mForest;

    IExeEngine::Graphics::RenderGroup mGuard_01;
	IExeEngine::Graphics::RenderGroup mGuard_02;
    IExeEngine::Graphics::RenderGroup mKnight;
	IExeEngine::Graphics::RenderObject mGround;

    IExeEngine::Graphics::Animator mGuard_01Animator;
    IExeEngine::Graphics::Animator mGuard_02Animator;
    IExeEngine::Graphics::Animator mKnightAnimator;

    IExeEngine::Graphics::Terrain mTerrain;

    IExeEngine::Graphics::RenderObject mScreenQuad;

    IExeEngine::Graphics::StandardEffect mStandardEffect;
    IExeEngine::Graphics::ShadowEffect mShadowEffect;
    IExeEngine::Graphics::TerrainEffect mTerrainEffect;

    int mClipIndex = -1;
    float mAnimationSpeed = 1.0f;
    bool mDrawSkeleton = false;
};