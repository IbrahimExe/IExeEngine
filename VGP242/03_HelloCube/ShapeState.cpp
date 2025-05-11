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
	mMeshbuffer.Initialize(mVertices.data(), sizeof(VertexPC), mVertices.size());

	std::filesystem::path shaderFilePath = L"../../Assets/Shaders/DoTransformColor.fx";
	mVertexShader.Initialize<VertexPC>(shaderFilePath);
	mPixelShader.Initialize(shaderFilePath);
}

void ShapeState::Terminate()
{
	mVertices.clear();
	mTransformBuffer.Terminate();
	mPixelShader.Terminate();
	mVertexShader.Terminate();
	mMeshbuffer.Terminate();
}

void ShapeState::Update(float deltaTime)
{
	InputSystem* input = InputSystem::Get();
	const float moveSpeed = input->IsKeyDown(KeyCode::LSHIFT) ? 10.0f : 3.0f;
	const float turnSpeed = 0.3f;

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
	const float hs = 0.5f;

	// Front
	mVertices.push_back({ { -hs, -hs, -hs }, Colors::Red });
	mVertices.push_back({ { -hs,  hs, -hs }, Colors::Orange });
	mVertices.push_back({ {  hs,  hs, -hs }, Colors::Gold });

	mVertices.push_back({ { -hs, -hs, -hs }, Colors::Red });
	mVertices.push_back({ {  hs,  hs, -hs }, Colors::Orange });
	mVertices.push_back({ {  hs, -hs, -hs }, Colors::Gold });

	// Back
	mVertices.push_back({ { -hs,  hs, -hs }, Colors::Red });
	mVertices.push_back({ { -hs, -hs, -hs }, Colors::Orange });
	mVertices.push_back({ {  hs,  hs, -hs }, Colors::Gold });

	mVertices.push_back({ { -hs, -hs, -hs }, Colors::Red });
	mVertices.push_back({ {  hs, -hs, -hs }, Colors::Orange });
	mVertices.push_back({ {  hs,  hs, -hs }, Colors::Gold });

	// Right
	mVertices.push_back({ {  hs, -hs, -hs }, Colors::Red });
	mVertices.push_back({ {  hs,  hs, -hs }, Colors::Orange });
	mVertices.push_back({ {  hs,  hs,  hs }, Colors::Gold });

	mVertices.push_back({ {  hs, -hs, -hs }, Colors::Red });
	mVertices.push_back({ {  hs,  hs,  hs }, Colors::Orange });
	mVertices.push_back({ {  hs, -hs,  hs }, Colors::Gold });

	// Left
	mVertices.push_back({ { -hs, -hs,  hs }, Colors::Red });
	mVertices.push_back({ { -hs,  hs,  hs }, Colors::Orange });
	mVertices.push_back({ {  hs,  hs, -hs }, Colors::Gold });

	mVertices.push_back({ { -hs, -hs,  hs }, Colors::Red });
	mVertices.push_back({ {  hs,  hs, -hs }, Colors::Orange });
	mVertices.push_back({ { -hs, -hs,  hs }, Colors::Gold });

	// Top
	mVertices.push_back({ { -hs,  hs, -hs }, Colors::Red });
	mVertices.push_back({ { -hs,  hs,  hs }, Colors::Orange });
	mVertices.push_back({ {  hs,  hs,  hs }, Colors::Gold });

	mVertices.push_back({ { -hs,  hs, -hs }, Colors::Red });
	mVertices.push_back({ {  hs,  hs,  hs }, Colors::Orange });
	mVertices.push_back({ {  hs,  hs, -hs }, Colors::Gold });

	// Bottom
	mVertices.push_back({ { -hs, -hs, -hs }, Colors::Red });
	mVertices.push_back({ {  hs, -hs,  hs }, Colors::Orange });
	mVertices.push_back({ { -hs, -hs,  hs }, Colors::Gold });

	mVertices.push_back({ { -hs, -hs, -hs }, Colors::Red });
	mVertices.push_back({ {  hs, -hs, -hs }, Colors::Orange });
	mVertices.push_back({ {  hs, -hs,  hs }, Colors::Gold });
}