#include "GameState.h"

using namespace IExeEngine;
using namespace IExeEngine::Graphics;
using namespace IExeEngine::Input;

void GameState::Initialize()
{
	mCamera.SetPosition({ 0.0f, 1.0f, -3.0f });
	mCamera.SetLookAt({ 0.0f, 0.0f, 0.0f });

	mDirectionalLight.direction = Math::Normalize({ 1.0f, -1.0f, 1.0f });
    mDirectionalLight.ambient = { 0.4f, 0.4f, 0.4f, 1.0f };
    mDirectionalLight.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
    mDirectionalLight.specular = { 0.9f, 0.9f, 0.9f, 1.0f };

    // DragonBorn - Free Spline Animation Software

    // Basketball
    Mesh basketball = MeshBuilder::CreateSphere(50, 50, 0.5f);
    mBasketball.meshBuffer.Initialize(basketball);

    TextureManager* tm_basket = TextureManager::Get();
    mBasketball.diffuseMapId = tm_basket->LoadTexture(L"../../Assets/Textures/misc/basketball.jpg");

    std::filesystem::path shaderFile = L"../../Assets/Shaders/Standard.fx";
    mStandardEffect.Initialize(shaderFile);
    mStandardEffect.SetCamera(mCamera);
    mStandardEffect.SetDirectionalLight(mDirectionalLight);

    // Animation Constructor
    mAnimationTime = 0.0f;
    mAnimation = AnimationBuilder()
        .AddPositionKey({ 0.0f, 0.0f, 0.0f }, 0.0f)
        .AddPositionKey({ 0.0f,-2.0f, 0.0f }, 3.0f)
        .AddPositionKey({ 0.0f, 0.0f, 0.0f }, 5.0f)

        .AddRotationKey(Math::Quaternion::Identity, 0.0f)
        .AddRotationKey(Math::Quaternion::CreateFromAxisAngle(Math::Vector3::YAxis, 90.0f * Math::Constants::HalfPi), 3.0f)
        .AddRotationKey(Math::Quaternion::CreateFromAxisAngle(Math::Vector3::YAxis, 90.0f * Math::Constants::Pi * 1.1), 3.1f)
        .AddRotationKey(Math::Quaternion::CreateFromAxisAngle(Math::Vector3::YAxis, 90.0f * Math::Constants::TwoPi), 5.0f)

        .AddScaleKey({1.0f, 1.0f, 1.0f}, 0.0f)
        .AddScaleKey({ 1.0f, 2.0f, 1.0f }, 3.0f)
        .AddScaleKey({ 1.0f, 1.0f, 1.0f }, 5.0f)
        .Build();
}

void GameState::Terminate()
{
    mBasketball.Terminate();

    mStandardEffect.Terminate();
}

void GameState::Update(float deltaTime)
{
	UpdateCamera(deltaTime);

    mAnimationTime += deltaTime;
    if (mAnimationTime > mAnimation.GetDuration())
    {
        mAnimationTime -= mAnimation.GetDuration();
    }
}

void GameState::Render()
{
    mBasketball.transform = mAnimation.GetTransform(mAnimationTime);

	SimpleDraw::AddGroundPlane(20.0f, Colors::Wheat);
	SimpleDraw::Render(mCamera);

    mStandardEffect.Begin();

	    mStandardEffect.Render(mBasketball);

    mStandardEffect.End();

}

void GameState::DebugUI()
{
	ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::DragFloat3("Direction#Light", &mDirectionalLight.direction.x, 0.01f))
		{
			mDirectionalLight.direction = Math::Normalize(mDirectionalLight.direction);
		}

		ImGui::ColorEdit4("Ambient#Light", &mDirectionalLight.ambient.r);
		ImGui::ColorEdit4("Diffuse#Light", &mDirectionalLight.diffuse.r);
		ImGui::ColorEdit4("Specular#Light", &mDirectionalLight.specular.r);
	}

	ImGui::Separator();

	mStandardEffect.DebugUI();
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