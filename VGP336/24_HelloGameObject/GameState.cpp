#include "GameState.h"

using namespace IExeEngine;
using namespace IExeEngine::Graphics;
using namespace IExeEngine::Input;
using namespace IExeEngine::Physics;

void GameState::Initialize()
{
    mGameWorld.Initialize();

    mGameWorld.CreateGameObject("Transform");
    mGameWorld.CreateGameObject("Camera");
    mGameWorld.CreateGameObject("Player");
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