#include "GameState.h"

using namespace IExeEngine;
using namespace IExeEngine::Graphics;
using namespace IExeEngine::Input;

enum class CurrentObject
{
    Earth,
    Metal,
    Wood,
    Water
};

const char* gObjectNames[] = { "Earth", "Metal", "Wood", "Water"};

CurrentObject gCurrentObject = CurrentObject::Earth;

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

    // Object 1 - Earth Sphere
	Mesh earth = MeshBuilder::CreateSphere(30, 30, 0.25f);
	mRenderObject_Earth.meshBuffer.Initialize(earth);

    TextureManager* tm = TextureManager::Get();
	mRenderObject_Earth.diffuseMapId = tm->LoadTexture(L"../../Assets/Textures/earth.jpg");
	mRenderObject_Earth.specMapId = tm->LoadTexture(L"../../Assets/Textures/earth_spec.jpg");
	mRenderObject_Earth.normalMapId = tm->LoadTexture(L"../../Assets/Textures/earth_normal.jpg");
	mRenderObject_Earth.bumpMapId = tm->LoadTexture(L"../../Assets/Textures/earth_bump.jpg");

    // Object 2 - Metal Sphere
    Mesh metal = MeshBuilder::CreateSphere(100, 100, 1.0f);
    mRenderObject_Metal.meshBuffer.Initialize(metal);

    TextureManager* tm2 = TextureManager::Get();
    mRenderObject_Metal.diffuseMapId = tm2->LoadTexture(L"../../Assets/Textures/metal/diffuse.jpg");
    mRenderObject_Metal.specMapId = tm2->LoadTexture(L"../../Assets/Textures/metal/spec.jpg");
    mRenderObject_Metal.normalMapId = tm2->LoadTexture(L"../../Assets/Textures/metal/normal.jpg");
    mRenderObject_Metal.bumpMapId = tm2->LoadTexture(L"../../Assets/Textures/metal/bump.jpg");

    // Object 3 - Wood Plane
    Mesh wood = MeshBuilder::CreatePlane(5, 5, 5.0f);
    mRenderObject_Wood.meshBuffer.Initialize(wood);

    TextureManager* tm3 = TextureManager::Get();
    mRenderObject_Wood.diffuseMapId = tm3->LoadTexture(L"../../Assets/Textures/wood/diffuse.jpg");
    mRenderObject_Wood.specMapId = tm3->LoadTexture(L"../../Assets/Textures/wood/spec.jpg");
    mRenderObject_Wood.normalMapId = tm3->LoadTexture(L"../../Assets/Textures/wood/normal.jpg");
    mRenderObject_Wood.bumpMapId = tm3->LoadTexture(L"../../Assets/Textures/wood/bump.jpg");

    // Object 4 - Water Ball
    Mesh water = MeshBuilder::CreateSphere(100, 100, 1.0f);
    mRenderObject_Water.meshBuffer.Initialize(water);

    TextureManager* tm4 = TextureManager::Get();
    mRenderObject_Water.diffuseMapId = tm4->LoadTexture(L"../../Assets/Textures/water/water_texture.jpg");
    mRenderObject_Water.specMapId = tm4->LoadTexture(L"../../Assets/Textures/water/water_spec.jpg");
    mRenderObject_Water.normalMapId = tm4->LoadTexture(L"../../Assets/Textures/water/water_normal.jpg");
    mRenderObject_Water.bumpMapId = tm4->LoadTexture(L"../../Assets/Textures/water/water_height.jpg");


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
        .AddRotationKey(Math::Quaternion::CreateFromAxisAngle(Math::Vector3::YAxis, Math::Constants::Pi * 1.1), 3.1f)
        .AddRotationKey(Math::Quaternion::CreateFromAxisAngle(Math::Vector3::YAxis, Math::Constants::TwoPi), 5.0f)
        .Build();
}

void GameState::Terminate()
{
    mBasketball.Terminate();
	mRenderObject_Earth.Terminate();
    mRenderObject_Metal.Terminate();
    mRenderObject_Wood.Terminate();
    mRenderObject_Water.Terminate();
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

		if (gCurrentObject == CurrentObject::Earth)
		{
			mStandardEffect.Render(mRenderObject_Earth);
		}
		else if (gCurrentObject == CurrentObject::Metal)
		{
		    mStandardEffect.Render(mRenderObject_Metal);
		}
		else if (gCurrentObject == CurrentObject::Wood)
		{
			mStandardEffect.Render(mRenderObject_Wood);
		}
        else if (gCurrentObject == CurrentObject::Water)
        {
            mStandardEffect.Render(mRenderObject_Water);
        }

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

    int currentObject = static_cast<int>(gCurrentObject);
    if (ImGui::Combo("Current Object", &currentObject, gObjectNames, std::size(gObjectNames)))
    {
        gCurrentObject = static_cast<CurrentObject>(currentObject);
    }

    switch (gCurrentObject)
    {
    case CurrentObject::Earth:
        ImGui::Text("Current Object: Earth");
		if (ImGui::CollapsingHeader("Earth#Material", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::ColorEdit4("Emissive#MaterialOfEarth", &mRenderObject_Earth.material.emissive.r);
            ImGui::ColorEdit4("Ambient#MaterialOfEarth", &mRenderObject_Earth.material.ambient.r);
            ImGui::ColorEdit4("Diffuse#MaterialOfEarth", &mRenderObject_Earth.material.diffuse.r);
            ImGui::ColorEdit4("Specular#MaterialOfEarth", &mRenderObject_Earth.material.specular.r);
            ImGui::DragFloat("Shininess#MaterialOfEarth", &mRenderObject_Earth.material.shininess, 0.001f, 0.0f, 10000.0f);
        }
		break;
    case CurrentObject::Metal:
        ImGui::Text("Current Object: Metal");
        if (ImGui::CollapsingHeader("Metal#Material", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::ColorEdit4("Emissive#MaterialOfMetal", &mRenderObject_Metal.material.emissive.r);
            ImGui::ColorEdit4("Ambient#MaterialOfMetal", &mRenderObject_Metal.material.ambient.r);
            ImGui::ColorEdit4("Diffuse#MaterialOfMetal", &mRenderObject_Metal.material.diffuse.r);
            ImGui::ColorEdit4("Specular#MaterialOfMetal", &mRenderObject_Metal.material.specular.r);
            ImGui::DragFloat("Shininess#MaterialOfMetal", &mRenderObject_Metal.material.shininess, 0.001f, 0.0f, 10000.0f);
        }
        break;
    case CurrentObject::Wood:
		ImGui::Text("Current Object: Wood");
        if (ImGui::CollapsingHeader("Wood#Material", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::ColorEdit4("Emissive#MaterialOfWood", &mRenderObject_Wood.material.emissive.r);
            ImGui::ColorEdit4("Ambient#MaterialOfWood", &mRenderObject_Wood.material.ambient.r);
            ImGui::ColorEdit4("Diffuse#MaterialOfWood", &mRenderObject_Wood.material.diffuse.r);
            ImGui::ColorEdit4("Specular#MaterialOfWood", &mRenderObject_Wood.material.specular.r);
            ImGui::DragFloat("Shininess#MaterialOfWood", &mRenderObject_Wood.material.shininess, 0.001f, 0.0f, 10000.0f);
        }
        break;
    case CurrentObject::Water:
        ImGui::Text("Current Object: Water");
        if (ImGui::CollapsingHeader("Water#Material", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::ColorEdit4("Emissive#MaterialOfWater", &mRenderObject_Water.material.emissive.r);
            ImGui::ColorEdit4("Ambient#MaterialOfWater", &mRenderObject_Water.material.ambient.r);
            ImGui::ColorEdit4("Diffuse#MaterialOfWater", &mRenderObject_Water.material.diffuse.r);
            ImGui::ColorEdit4("Specular#MaterialOfWater", &mRenderObject_Water.material.specular.r);
            ImGui::DragFloat("Shininess#MaterialOfWater", &mRenderObject_Water.material.shininess, 0.001f, 0.0f, 10000.0f);
        }
        break;
    default:
        break;
    }

    /*
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
    */

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