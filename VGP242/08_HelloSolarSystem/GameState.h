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

	IExeEngine::Graphics::SimpleTextureEffect::RenderData mSun;
	IExeEngine::Graphics::SimpleTextureEffect::RenderData mMercury;
    IExeEngine::Graphics::SimpleTextureEffect::RenderData mVenus;
	IExeEngine::Graphics::SimpleTextureEffect::RenderData mEarth;
    IExeEngine::Graphics::SimpleTextureEffect::RenderData mMars;
    IExeEngine::Graphics::SimpleTextureEffect::RenderData mJupiter;
    IExeEngine::Graphics::SimpleTextureEffect::RenderData mSaturn;
    IExeEngine::Graphics::SimpleTextureEffect::RenderData mUranus;
    IExeEngine::Graphics::SimpleTextureEffect::RenderData mNeptune;

	IExeEngine::Graphics::SimpleTextureEffect::RenderData mAsteroid;

	// Render Target
	IExeEngine::Graphics::RenderTarget mRenderTarget;
};