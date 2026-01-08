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

    IExeEngine::Graphics::RenderObject mRenderObject_Earth;
    IExeEngine::Graphics::RenderObject mRenderObject_Metal;
    IExeEngine::Graphics::RenderObject mRenderObject_Wood;
    IExeEngine::Graphics::RenderObject mRenderObject_Water;

    IExeEngine::Graphics::StandardEffect mStandardEffect;

	IExeEngine::Graphics::Animation mAnimation;
    float mAnimationTime = 0.0f;
};