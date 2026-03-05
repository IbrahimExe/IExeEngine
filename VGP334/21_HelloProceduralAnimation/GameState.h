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

    IExeEngine::Graphics::StandardEffect mStandardEffect;

    // Ground info
    IExeEngine::Graphics::RenderObject mGroundObject;
    IExeEngine::Physics::CollisionShape mGroundShape;
    IExeEngine::Physics::RigidBody mGroundRigidBody;

    // Body for Procedural Generation
    struct Segment
    {
        IExeEngine::Graphics::RenderObject segment;
        float range = 0.0f;
    };
    std::vector<Segment> mSegments;
    IExeEngine::Physics::CollisionShape mBodyAnchorShape;
    IExeEngine::Physics::RigidBody mBodyAnchorRB;

};