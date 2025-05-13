#include "ShapeState.h"

using namespace IExeEngine;
using namespace IExeEngine::Math;
using namespace IExeEngine::Graphics;
using namespace IExeEngine::Input;


void ShapeState::Initialize()
{
	mCamera.SetPosition({ 0.0f, 1.0f, -3.0f });
	mCamera.SetLookAt({ 0.0f, 0.0f, 0.0f });

	mTransformBuffer.Initialize(sizeof(Math::Matrix4));

	// Creates a shape out of the vertices
	CreateShape();
	mMeshbuffer.Initialize(mMesh);

	std::filesystem::path shaderFilePath = L"../../Assets/Shaders/DoTransformColor.fx";
	mVertexShader.Initialize<VertexPC>(shaderFilePath);
	mPixelShader.Initialize(shaderFilePath);
}

void ShapeState::Terminate()
{
	mTransformBuffer.Terminate();
	mPixelShader.Terminate();
	mVertexShader.Terminate();
	mMeshbuffer.Terminate();
}

void ShapeState::Update(float deltaTime)
{
	// Camera Controls:
	InputSystem* input = InputSystem::Get();
	const float moveSpeed = input->IsKeyDown(KeyCode::LSHIFT) ? 10.0f : 4.0f;
	const float turnSpeed = 0.5f;

	if (input->IsKeyDown(KeyCode::W)) { mCamera.Walk(moveSpeed * deltaTime); }

	else if (input->IsKeyDown(KeyCode::S)) { mCamera.Walk(- moveSpeed * deltaTime); }

	else if (input->IsKeyDown(KeyCode::D)) { mCamera.Strafe(moveSpeed * deltaTime); }

	else if (input->IsKeyDown(KeyCode::A)) { mCamera.Strafe(- moveSpeed * deltaTime); }

	else if (input->IsKeyDown(KeyCode::E)) { mCamera.Rise(moveSpeed * deltaTime); }
 
	else if (input->IsKeyDown(KeyCode::Q)) { mCamera.Rise(- moveSpeed * deltaTime); }

	if (input->IsMouseDown(MouseButton::RBUTTON))
	{
		mCamera.Yaw(input->GetMouseMoveX() * turnSpeed * deltaTime);
		mCamera.Pitch(input->GetMouseMoveY() * turnSpeed * deltaTime);
	}

	// Scene Change Controls:
	if (Input::InputSystem::Get()->IsKeyPressed(Input::KeyCode::UP))
	{
		IExeEngine::MainApp().ChangeState("Pyramid");
	}

	if (Input::InputSystem::Get()->IsKeyPressed(Input::KeyCode::LEFT))
	{
		IExeEngine::MainApp().ChangeState("Cube");
	}

	if (Input::InputSystem::Get()->IsKeyPressed(Input::KeyCode::RIGHT))
	{
		IExeEngine::MainApp().ChangeState("Rectangle");
	}
}

void ShapeState::Render()
{
	// Prepare GPU
	mVertexShader.Bind();
	mPixelShader.Bind();

	// Sync Transform Buffer
	mTransformBuffer.BindVS(0);

	// Update the Buffer Data
	Math::Matrix4 matWorld = Math::Matrix4::Identity;
	Math::Matrix4 matView = mCamera.GetViewMatrix();
	Math::Matrix4 matProj = mCamera.GetProjectionMatrix();
	Math::Matrix4 matFinal = matWorld * matView * matProj; // = wvp
	Math::Matrix4 wvp = Math::Transpose(matFinal);
	mTransformBuffer.Update(&wvp);

	// Draw
	mMeshbuffer.Render();
}

void ShapeState::CreateShape()
{
	mMesh = MeshBuilder::CreateRectanglePC(2.0f, 1.0f, 2.0f);
	mMesh = MeshBuilder::CreateCubePC(4.0f);
	mMesh = MeshBuilder::CreatePyramid(5.0f);
}

void CubeState::Update(float deltaTime)
{
//====================================================================================================================================================================
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
//===========================================================================================================================================================================================

	if (Input::InputSystem::Get()->IsKeyPressed(Input::KeyCode::UP))
	{
		IExeEngine::MainApp().ChangeState("Pyramid");
	}

	if (Input::InputSystem::Get()->IsKeyPressed(Input::KeyCode::DOWN))
	{
		IExeEngine::MainApp().ChangeState("ShapeState");
	}

	if (Input::InputSystem::Get()->IsKeyPressed(Input::KeyCode::RIGHT))
	{
		IExeEngine::MainApp().ChangeState("Rectangle");
	}
}
void CubeState::CreateShape()
{
	mMesh = MeshBuilder::CreateCubePC(1.0f);
}

void PyramidState::Update(float deltaTime)
{
//===============================================================================================================================================
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
//===========================================================================================================================================================================================

	if (Input::InputSystem::Get()->IsKeyPressed(Input::KeyCode::DOWN))
	{
		IExeEngine::MainApp().ChangeState("ShapeState");
	}

	if (Input::InputSystem::Get()->IsKeyPressed(Input::KeyCode::LEFT))
	{
		IExeEngine::MainApp().ChangeState("Cube");
	}

	if (Input::InputSystem::Get()->IsKeyPressed(Input::KeyCode::RIGHT))
	{
		IExeEngine::MainApp().ChangeState("Rectangle");
	}
}
void PyramidState::CreateShape()
{
	mMesh = MeshBuilder::CreatePyramid(1.0f);
}

void RectangleState::Update(float deltaTime)
{
//===========================================================================================================================================================================================
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
//===========================================================================================================================================================================================

	if (Input::InputSystem::Get()->IsKeyPressed(Input::KeyCode::UP))
	{
		IExeEngine::MainApp().ChangeState("Pyramid");
	}

	if (Input::InputSystem::Get()->IsKeyPressed(Input::KeyCode::LEFT))
	{
		IExeEngine::MainApp().ChangeState("Cube");
	}

	if (Input::InputSystem::Get()->IsKeyPressed(Input::KeyCode::DOWN))
	{
		IExeEngine::MainApp().ChangeState("ShapeState");
	}
}
void RectangleState::CreateShape()
{
	mMesh = MeshBuilder::CreateRectanglePC(1.0f, 1.0f, 2.0f);
}