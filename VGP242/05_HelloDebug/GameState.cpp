#include "GameState.h"

using namespace IExeEngine;
using namespace IExeEngine::Graphics;
using namespace IExeEngine::Input;

void GameState::Initialize()
{

}

void GameState::Terminate()
{

}

void GameState::Update(float deltaTime)
{
	UpdateCamera(deltaTime);
}

void GameState::Render()
{

}

bool gCheckValue = false;
float gFloatVal = 0.0f;
Math::Vector3 gVectorVal = Math::Vector3::Zero;
Color gColor = Colors::White;
void GameState::DebugUI()
{
	ImGui::Begin("Debug_01", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::Text("Im Skylar White, YO!");
	ImGui::Checkbox("My husband is Walter White, YO?", &gCheckValue);
	ImGui::DragFloat("How Walter are you?", &gFloatVal);
	ImGui::DragFloat3("YEAH SCIENCE!", &gVectorVal.x);
	ImGui::ColorEdit4("Walter...?", &gColor.r);

	ImGui::End();
}

void GameState::UpdateCamera(float deltaTime)
{
	// Camera Controls:
	InputSystem* input = InputSystem::Get();
	const float moveSpeed = input->IsKeyDown(KeyCode::LSHIFT) ? 10.0f : 4.0f;
	const float turnSpeed = 0.5f;

	if (input->IsKeyDown(KeyCode::W)) { mCamera.Walk(moveSpeed * deltaTime); }

	else if (input->IsKeyDown(KeyCode::S)) { mCamera.Walk(-moveSpeed * deltaTime); }

	else if (input->IsKeyDown(KeyCode::D)) { mCamera.Strafe(moveSpeed * deltaTime); }

	else if (input->IsKeyDown(KeyCode::A)) { mCamera.Strafe(-moveSpeed * deltaTime); }

	else if (input->IsKeyDown(KeyCode::E)) { mCamera.Rise(moveSpeed * deltaTime); }

	else if (input->IsKeyDown(KeyCode::Q)) { mCamera.Rise(-moveSpeed * deltaTime); }

	if (input->IsMouseDown(MouseButton::RBUTTON))
	{
		mCamera.Yaw(input->GetMouseMoveX() * turnSpeed * deltaTime);
		mCamera.Pitch(input->GetMouseMoveY() * turnSpeed * deltaTime);
	}
}