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
	struct Planet
	{
		float radius;
		IExeEngine::Math::Matrix4 centerObject;
		float distanceFromCenter;
		float orbitTranslation;
		float rotationOnAxis;
		float orbitAngle = 0.0f;
		float selfRotationAngle = 0.0f;
		IExeEngine::Graphics::SimpleTextureEffect::RenderData renderData;
	};

	struct Asteroid
	{
        float size;
        IExeEngine::Math::Matrix4 centerObject;
        float distanceFromCenter;
        float orbitTranslation;
        float selfRotationAngle = 0.0f;
        IExeEngine::Graphics::SimpleTextureEffect::RenderData renderData;
	};

	void UpdateCamera(float deltaTime);

    // Functions for Animating Planets & Moon
	void UpdateRenderTargetCamera(float deltaTime, Math::Vector3 targetPosition, float objectRadius);
	void UpdatePlanet(Planet& planet, float deltaTime);
	void UpdateMoon(Planet& planet, const Planet& earth, float deltaTime);

	IExeEngine::Graphics::Camera mCamera;
	IExeEngine::Graphics::Camera mRenderTargetCamera;

	// GPU communication
	IExeEngine::Graphics::SimpleTextureEffect mSimpleTextureEffect;

	// Render Objects
	IExeEngine::Graphics::SimpleTextureEffect::RenderData mSpace;
	IExeEngine::Graphics::SimpleTextureEffect::RenderData mSun;

	// Render Planets & Moon 
	Planet mMercury;
	Planet mVenus;
	Planet mEarth;
	Planet mMoon;
	Planet mMars;
	Planet mJupiter;
	Planet mSaturn;
	Planet mUranus;
	Planet mNeptune;
	Planet mPluto;

	// Render Target
	IExeEngine::Graphics::RenderTarget mRenderTarget;

	// Render ImGui Orbit Rings
	bool orbitRings = true;
};