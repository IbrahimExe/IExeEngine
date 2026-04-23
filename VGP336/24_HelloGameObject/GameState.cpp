#include "GameState.h"

using namespace IExeEngine;
using namespace IExeEngine::Graphics;
using namespace IExeEngine::Input;
using namespace IExeEngine::Physics;


void GameState::Initialize()
{
	mGameWorld.Initialize();

	// Camera
	GameObject* cameraGO = mGameWorld.CreateGameObject("Camera");
	mCameraComponent = cameraGO->AddComponent<CameraComponent>();
	cameraGO->AddComponent<FPSCameraComponent>();
	cameraGO->Initialize();

	// World Objects
	GameObject* playerGO = mGameWorld.CreateGameObject("Player");
	TransformComponent* playerTransform = playerGO->AddComponent<TransformComponent>();
	playerTransform->position.x = 2.0f;
	playerGO->Initialize();

	GameObject* transformGO = mGameWorld.CreateGameObject("Transform");
	transformGO->AddComponent<TransformComponent>();
	transformGO->Initialize();




}

void GameState::Terminate()
{
	mGameWorld.Terminate();
}

void GameState::Update(float deltaTime)
{
	mGameWorld.Update(deltaTime);
}

void GameState::Render()
{
	mGameWorld.Render();
}

void GameState::DebugUI()
{
	ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	mGameWorld.DebugUI();
	ImGui::End();

	SimpleDraw::Render(mCameraComponent->GetCamera());
}