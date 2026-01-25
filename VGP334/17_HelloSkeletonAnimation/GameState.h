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

    IExeEngine::Graphics::RenderGroup mCharacter;
	IExeEngine::Graphics::RenderGroup parasite;
    IExeEngine::Graphics::RenderGroup zombie;

    IExeEngine::Graphics::Animator mCharacterAnimator;
    IExeEngine::Graphics::Animator mparasiteAnimator;
    IExeEngine::Graphics::Animator mzombieAnimator;

    IExeEngine::Graphics::StandardEffect mStandardEffect;

    int mClipIndex = -1;
    float mAnimationSpeed = 1.0f;
	bool mDrawSkeleton = false;
};