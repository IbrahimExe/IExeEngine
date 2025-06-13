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
	IExeEngine::Graphics::Camera mRenderTargetCamera;

	// GPU Communication
    IExeEngine::Graphics::SimpleTextureEffect mSimpleTextureEffect;

	// Render Object
	IExeEngine::Graphics::SimpleTextureEffect::RenderData mSpace;
	IExeEngine::Graphics::SimpleTextureEffect::RenderData mEarth;
	IExeEngine::Graphics::SimpleTextureEffect::RenderData mSun;
    IExeEngine::Graphics::SimpleTextureEffect::RenderData mAsteroid_1;
    IExeEngine::Graphics::SimpleTextureEffect::RenderData mSaturn;
    IExeEngine::Graphics::SimpleTextureEffect::RenderData mSword;

	// Render Target
    IExeEngine::Graphics::RenderTarget mRenderTarget;
};