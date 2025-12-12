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
	IExeEngine::Graphics::RenderObject mGround;

    IExeEngine::Graphics::RenderObject mScreenQuad;

    IExeEngine::Graphics::StandardEffect mStandardEffect;
    IExeEngine::Graphics::ShadowEffect mShadowEffect;

    IExeEngine::Graphics::HalftoneEffect mHalftoneEffect;
};