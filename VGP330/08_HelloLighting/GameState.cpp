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

    // Object 1 - Earth
	Mesh earth = MeshBuilder::CreateSphere(100, 100, 1.0f);
	mRenderObject_Earth.meshBuffer.Initialize(earth);

    TextureManager* tm = TextureManager::Get();
	mRenderObject_Earth.diffuseMapId = tm->LoadTexture(L"../../Assets/Textures/earth.jpg");
	mRenderObject_Earth.specMapId = tm->LoadTexture(L"../../Assets/Textures/earth_spec.jpg");
	mRenderObject_Earth.normalMapId = tm->LoadTexture(L"../../Assets/Textures/earth_normal.jpg");
	mRenderObject_Earth.bumpMapId = tm->LoadTexture(L"../../Assets/Textures/earth_bump.jpg");

    // Object 2 - Metal
    Mesh metal = MeshBuilder::CreateSphere(100, 100, 1.0f);
    mRenderObject_Metal.meshBuffer.Initialize(metal);

    TextureManager* tm2 = TextureManager::Get();
    mRenderObject_Metal.diffuseMapId = tm2->LoadTexture(L"../../Assets/Textures/metal/diffuse.jpg");
    mRenderObject_Metal.specMapId = tm2->LoadTexture(L"../../Assets/Textures/metal/spec.jpg");
    mRenderObject_Metal.normalMapId = tm2->LoadTexture(L"../../Assets/Textures/metal/normal.jpg");
    mRenderObject_Metal.bumpMapId = tm2->LoadTexture(L"../../Assets/Textures/metal/bump.jpg");

    // Object 3 - Wood
    Mesh wood = MeshBuilder::CreatePlane(5, 5, 5.0f);
    mRenderObject_Wood.meshBuffer.Initialize(wood);

    TextureManager* tm3 = TextureManager::Get();
    mRenderObject_Wood.diffuseMapId = tm3->LoadTexture(L"../../Assets/Textures/wood/diffuse.jpg");
    mRenderObject_Wood.specMapId = tm3->LoadTexture(L"../../Assets/Textures/wood/spec.jpg");
    mRenderObject_Wood.normalMapId = tm3->LoadTexture(L"../../Assets/Textures/wood/normal.jpg");
    mRenderObject_Wood.bumpMapId = tm3->LoadTexture(L"../../Assets/Textures/wood/bump.jpg");


    std::filesystem::path shaderFile = L"../../Assets/Shaders/Standard.fx";
    mStandardEffect.Initialize(shaderFile);
    mStandardEffect.SetCamera(mCamera);
    mStandardEffect.SetDirectionalLight(mDirectionalLight);
}

void GameState::Terminate()
{
	mRenderObject_Earth.Terminate();
    mRenderObject_Metal.Terminate();
    mRenderObject_Wood.Terminate();
    mStandardEffect.Terminate();
}

void GameState::Update(float deltaTime)
{
	UpdateCamera(deltaTime);
}

void GameState::Render()
{
	SimpleDraw::AddGroundPlane(20.0f, Colors::Wheat);
	SimpleDraw::Render(mCamera);

    mStandardEffect.Begin();

		//mStandardEffect.Render(mRenderObject_Earth);
        //mStandardEffect.Render(mRenderObject_Metal);
        mStandardEffect.Render(mRenderObject_Wood);

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

	if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::ColorEdit4("Emissive#Material", &mRenderObject_Earth.material.emissive.r);
		ImGui::ColorEdit4("Ambient#Material", &mRenderObject_Earth.material.ambient.r);
		ImGui::ColorEdit4("Diffuse#Material", &mRenderObject_Earth.material.diffuse.r);
		ImGui::ColorEdit4("Specular#Material", &mRenderObject_Earth.material.specular.r);
        ImGui::DragFloat("Shininess#Material", &mRenderObject_Earth.material.shininess, 0.01f, 0.0f, 10000.0f);

		ImGui::ColorEdit4("Emissive#Material", &mRenderObject_Metal.material.emissive.r);
		ImGui::ColorEdit4("Ambient#Material", &mRenderObject_Metal.material.ambient.r);
		ImGui::ColorEdit4("Diffuse#Material", &mRenderObject_Metal.material.diffuse.r);
		ImGui::ColorEdit4("Specular#Material", &mRenderObject_Metal.material.specular.r);
		ImGui::DragFloat("Shininess#Material", &mRenderObject_Metal.material.shininess, 0.01f, 0.0f, 10000.0f);

		ImGui::ColorEdit4("Emissive#Material", &mRenderObject_Wood.material.emissive.r);
		ImGui::ColorEdit4("Ambient#Material", &mRenderObject_Wood.material.ambient.r);
		ImGui::ColorEdit4("Diffuse#Material", &mRenderObject_Wood.material.diffuse.r);
		ImGui::ColorEdit4("Specular#Material", &mRenderObject_Wood.material.specular.r);
		ImGui::DragFloat("Shininess#Material", &mRenderObject_Wood.material.shininess, 0.01f, 0.0f, 10000.0f);
	}

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