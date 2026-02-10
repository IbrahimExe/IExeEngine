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

    IExeEngine::Graphics::RenderObject mFootball;
	IExeEngine::Physics::CollisionShape mBallShape;
    IExeEngine::Physics::RigidBody mBallRigidBody;

    IExeEngine::Graphics::RenderObject mGroundObject;
    IExeEngine::Physics::CollisionShape mGroundShape;
    IExeEngine::Physics::RigidBody mGroundRigidBody;

	IExeEngine::Graphics::StandardEffect mStandardEffect;
};