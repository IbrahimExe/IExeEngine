#include "GameState.h"

using namespace IExeEngine;
using namespace IExeEngine::Graphics;
using namespace IExeEngine::Input;

void GameState::Initialize()
{
	mCamera.SetPosition({ 0.0f, 1.5f, -2.0f });
	mCamera.SetLookAt({ 0.0f, 1.0f, 0.0f });

	mDirectionalLight.direction = Math::Normalize({ 1.0f, -1.0f, 1.0f });
    mDirectionalLight.ambient = { 0.4f, 0.4f, 0.4f, 1.0f };
    mDirectionalLight.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
    mDirectionalLight.specular = { 0.9f, 0.9f, 0.9f, 1.0f };

    mTerrain.Initialize(L"../../Assets/Textures/terrain/heightmap_1024x1024.raw", 20.0f);
    mGround.meshBuffer.Initialize(mTerrain.mesh);
    mGround.diffuseMapId = TextureManager::Get()->LoadTexture(L"../../Assets/Textures/terrain/dirt_seamless.jpg");
    mGround.specMapId = TextureManager::Get()->LoadTexture(L"../../Assets/Textures/terrain/grass_2048.jpg");

    TextureManager* tm = TextureManager::Get();

    // BG Forrest
    MeshPX forest = MeshBuilder::CreateSkySpherePX(60, 60, 100.0f);
    mForest.diffuseMapId = tm->LoadTexture("darkerSpace.jpg");

    ModelManager* mm = ModelManager::Get();

	mGuard_01.Initialize("Guard_01/Model/Guard_01.model"); // Guard 1
    mGuard_01.transform.position = { 0.0f, 0.0f, 0.0f };
    mGuard_01.animator = &mGuard_01Animator;
    mm->AddAnimation(mGuard_01.modelId, L"../../Assets/Models/Guard_01/Animations/Kill_Guard_01.animset");
    //mm->AddAnimation(mGuard_01.modelId, L"../../Assets/Models/Guard_01/Animations/NotFamilyFriendly.animset");
    mGuard_01Animator.Initialize(mGuard_01.modelId);

    mGuard_02.Initialize("Guard_02/Model/Guard_02.model"); // Guard 2
    mGuard_02.transform.position = { -0.5f, 0.0f, 0.9f };
    mGuard_02.animator = &mGuard_02Animator;
    mm->AddAnimation(mGuard_02.modelId, L"../../Assets/Models/Guard_02/Animations/Reacting_Guard_02.animset");
    //mm->AddAnimation(mGuard_01.modelId, L"../../Assets/Models/Guard_01/Animations/NotFamilyFriendly.animset");
    mGuard_02Animator.Initialize(mGuard_02.modelId);

    mKnight.Initialize("Knight/Model/Knight.model"); // Knight
    mKnight.transform.position = { 0.5f, 0.0f, 0.6f };
    mKnight.animator = &mKnightAnimator;
    mm->AddAnimation(mKnight.modelId, L"../../Assets/Models/Knight/Animations/GoofyRun_Knight.animset");
    //mm->AddAnimation(mGuard_01.modelId, L"../../Assets/Models/Guard_01/Animations/NotFamilyFriendly.animset");
    mKnightAnimator.Initialize(mKnight.modelId);

    MeshPX screenQuadMesh = MeshBuilder::CreateScreenQuadPX();
    mScreenQuad.meshBuffer.Initialize(screenQuadMesh);

    std::filesystem::path shaderFile = L"../../Assets/Shaders/Standard.fx";
    mStandardEffect.Initialize(shaderFile);
    mStandardEffect.SetCamera(mCamera);
    mStandardEffect.SetDirectionalLight(mDirectionalLight);
    mStandardEffect.SetLightCamera(mShadowEffect.GetLightCamera());
    mStandardEffect.SetShadowMap(mShadowEffect.GetDepthMap());

    shaderFile = L"../../Assets/Shaders/Shadow.fx";
    mShadowEffect.Initialize();
    mShadowEffect.SetDirectionalLight(mDirectionalLight);

    mTerrainEffect.Initialize();
    mTerrainEffect.SetCamera(mCamera);
    mTerrainEffect.SetLightCamera(mShadowEffect.GetLightCamera());
    mTerrainEffect.SetDirectionalLight(mDirectionalLight);
    mTerrainEffect.SetShadowMap(mShadowEffect.GetDepthMap());

    GraphicsSystem* gs = GraphicsSystem::Get();
    const uint32_t screenWidth = gs->GetBackBufferWidth();
    const uint32_t screenHeight = gs->GetBackBufferHeight();
}

void GameState::Terminate()
{
    mTerrainEffect.Terminate();
    mShadowEffect.Terminate();
    mScreenQuad.Terminate();
	mGuard_01.Terminate();
    mGuard_02.Terminate();
    mKnight.Terminate();
    mGround.Terminate();
    mStandardEffect.Terminate();
}

void GameState::Update(float deltaTime)
{
	UpdateCamera(deltaTime);

    // Update animator
    mGuard_01Animator.Update(deltaTime * mAnimationSpeed);
    mGuard_02Animator.Update(deltaTime * mAnimationSpeed);
    mKnightAnimator.Update(deltaTime * mAnimationSpeed);
}

void GameState::Render()
{
    SimpleDraw::AddGroundPlane(20.0f, mDrawSkeleton ? Colors::DarkRed : Colors::White);
    SimpleDraw::Render(mCamera);

    // Animation Util
    AnimationUtil::BoneTransforms boneTransforms;

    AnimationUtil::ComputeBoneTransforms(mGuard_01.modelId, boneTransforms, &mGuard_01Animator);
    AnimationUtil::DrawSkeleton(mGuard_01.modelId, boneTransforms);

    AnimationUtil::ComputeBoneTransforms(mGuard_02.modelId, boneTransforms, &mGuard_02Animator);
    AnimationUtil::DrawSkeleton(mGuard_02.modelId, boneTransforms);

    AnimationUtil::ComputeBoneTransforms(mKnight.modelId, boneTransforms, &mKnightAnimator);
    AnimationUtil::DrawSkeleton(mKnight.modelId, boneTransforms);

    //----------------------------------------------------------
  // First Pass: Render to Shadow Map [Have to do Shadow Pass first]
  //----------------------------------------------------------
    mShadowEffect.Begin();
        mShadowEffect.Render(mGuard_01);
        mShadowEffect.Render(mGuard_02);
        mShadowEffect.Render(mKnight);
    mShadowEffect.End();

    mTerrainEffect.Begin();
        mTerrainEffect.Render(mGround);
    mTerrainEffect.End();
    //----------------------------------------------------------
    // Second Pass: Render Scene
//----------------------------------------------------------
	//SimpleDraw::AddGroundPlane(20.0f, Colors::White);
	//SimpleDraw::Render(mCamera);

	mStandardEffect.Begin();
     // mStandardEffect.Render(mGround); As we already render it in the TerrainEffect
		mStandardEffect.Render(mGuard_01);
        mStandardEffect.Render(mGuard_02);
		mStandardEffect.Render(mKnight);
	mStandardEffect.End();

//----------------------------------------------------------
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

    if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (uint32_t i = 0; i < mGuard_01.renderObjects.size(); ++i)
        {
            Material& material = mGuard_01.renderObjects[i].material;
            std::string renderObjectId = "RenderObject " + std::to_string(i);
            ImGui::PushID(renderObjectId.c_str());
			if (ImGui::CollapsingHeader(renderObjectId.c_str()))
			{
				ImGui::LabelText("label", "Material:");
				ImGui::ColorEdit4("Emissive#Material", &material.emissive.r);
				ImGui::ColorEdit4("Ambient#Material", &material.ambient.r);
				ImGui::ColorEdit4("Diffuse#Material", &material.diffuse.r);
				ImGui::ColorEdit4("Specular#Material", &material.specular.r);
				ImGui::DragFloat("Shininess#Material", &material.shininess, 0.1f, 0.1f, 10000.0f);
			}
            ImGui::PopID();
        }
    }
	ImGui::Separator();

	mStandardEffect.DebugUI();

    mShadowEffect.DebugUI();

    mTerrainEffect.DebugUI();

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