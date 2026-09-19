#include "GameState.h"

using namespace IExeEngine;
using namespace IExeEngine::Graphics;
using namespace IExeEngine::Input;
using namespace IExeEngine::Physics;

// "Hey IExeEngine, I made something custom, so you need to acknowledge it..."
Service* MakeCustomService(const std::string& serviceName, GameWorld& gameWorld)
{

	return nullptr;
}

Component* MakeCustomComponent(const std::string& componentName, GameObject& gameObject)
{

	return nullptr;
}

Component* GetCustomComponent(const std::string& componentName, GameObject& gameObject)
{

	return nullptr;
}

void GameState::Initialize()
{
	mLevelFile = L"../../Assets/Templates/Levels/level_mp.json";

	// set a callback to try make a custom service (any service that is NoT part of the engine and unique to the project)
	GameWorld::SetCustomService(MakeCustomService);
	// sets callbacks to try make/get a custom component (any component that is NOT part of the engine)
	GameObjectFactory::SetCustomMake(MakeCustomComponent);
	GameObjectFactory::SetCustomGet(GetCustomComponent);

	mGameWorld.LoadLevel(mLevelFile);
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
}