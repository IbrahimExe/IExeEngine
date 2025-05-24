#include "GameState.h"

using namespace IExeEngine;
using namespace IExeEngine::Graphics;
using namespace IExeEngine::Input;

void GameState::Initialize()
{
	mCamera.SetPosition({ 0.0f, 1.0f, -3.0f });
	mCamera.SetLookAt({ 0.0f, 0.0f, 0.0f });
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
Math::Vector3 gV0 = Math::Vector3::Zero;
Math::Vector3 gV1 = Math::Vector3::One;
Math::Vector3 gV2 = Math::Vector3::XAxis;
Color gColor = Colors::White;

enum class Shape
{
	None,
	AABB,
	AABBFilled,
	Sphere,
	GroundPlane,
	GroundCircle,
	Transform
};
const char* gShapeNames[] =
{
	"None",
	"AABB",
	"AABBFilled",
	"Sphere",
	"GroundPlane",
	"GroundCircle",
	"Transform"
};
Shape gCurrentShape = Shape::None;

void GameState::DebugUI()
{
	ImGui::Begin("Debug_01", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::Text("Im Skylar White, YO!");
	ImGui::Checkbox("My husband is Walter White, YO?", &gCheckValue);
	ImGui::DragFloat("How Bad are you Breaking?", &gFloatVal);
	ImGui::DragFloat3("[V0] So you do have a plan?", &gV0.x, 0.1f);
	ImGui::DragFloat3("[V1] Yeah, Mr.White!", &gV1.x, 0.1f);
	ImGui::DragFloat3("[V2] YEAH SCIENCE!", &gV2.x, 0.1f);
	ImGui::ColorEdit4("Walter...?", &gColor.r);

	int currentShape = (int)gCurrentShape;
	if (ImGui::Combo("Shape", &currentShape, gShapeNames, std::size(gShapeNames)))
	{
		gCurrentShape = (Shape)currentShape;
	}

	switch (gCurrentShape)
	{
	case Shape::None: break;

	case Shape::AABB:
	{
		SimpleDraw::AddAABB(gV0, gV1, gColor);
		break;
	}

	case Shape::AABBFilled:
	{
		SimpleDraw::AddFilledAABB(gV0, gV1, gColor);
		break;
	}

	case Shape::Sphere:
	{
		SimpleDraw::AddSphere(60, 60, gFloatVal, gColor, gV0);
		break;
	}
	case Shape::GroundPlane:
	{
		SimpleDraw::AddGroundPlane(gFloatVal, gColor);
		break;
	}

	case Shape::GroundCircle:
	{
		SimpleDraw::AddGroundCircle(40, gFloatVal, gColor, gV0);
		break;
	}

	case Shape::Transform:
	{
		SimpleDraw::AddTransform(Math::Matrix4::Identity);
		break;
	}
	}

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