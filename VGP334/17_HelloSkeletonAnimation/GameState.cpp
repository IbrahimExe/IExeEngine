#include "GameState.h"

using namespace IExeEngine;
using namespace IExeEngine::Graphics;
using namespace IExeEngine::Input;

enum class CurrentModel
{
	Timmy,
	Parasite,
	Zombie
};

const char* gObjectNames[] = { "Timmy", "Parasite", "Zombie" };

CurrentModel gCurrentModel = CurrentModel::Timmy;

void GameState::Initialize()
{
	mCamera.SetPosition({ 0.0f, 1.0f, -3.0f });
	mCamera.SetLookAt({ 0.0f, 0.0f, 0.0f });

	mDirectionalLight.direction = Math::Normalize({ 1.0f, -1.0f, 1.0f });
	mDirectionalLight.ambient = { 0.4f, 0.4f, 0.4f, 1.0f };
	mDirectionalLight.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
	mDirectionalLight.specular = { 0.9f, 0.9f, 0.9f, 1.0f };

	ModelManager* timmy = ModelManager::Get();
	mCharacter.Initialize("Character_01/Character_01.model"); // Lil Timmy
	mCharacter.transform.position = { 0.0f, 0.0f, 0.0f };
    mCharacter.animator = &mCharacterAnimator;
    timmy->AddAnimation(mCharacter.modelId, L"../../Assets/Models/Character_01/SixSeven.animset");
	timmy->AddAnimation(mCharacter.modelId, L"../../Assets/Models/Character_01/NotFamilyFriendly.animset");

    mCharacterAnimator.Initialize(mCharacter.modelId);

	parasite.Initialize("parasite/parasite.model"); // Parasite
	parasite.transform.position = { 0.0f, 0.0f, 0.0f };
    parasite.animator = &mParasiteAnimator;
    ModelManager::Get()->AddAnimation(parasite.modelId, L"../../Assets/Models/parasite/samba.animset");

    mParasiteAnimator.Initialize(parasite.modelId);

	zombie.Initialize("zombie/zombie.model"); // Zombie
	zombie.transform.position = { 0.0f, 0.0f, 0.0f };
    zombie.animator = &mZombieAnimator;
    ModelManager::Get()->AddAnimation(zombie.modelId, L"../../Assets/Models/zombie/Thriller.animset");

    mZombieAnimator.Initialize(zombie.modelId);

	std::filesystem::path shaderFile = L"../../Assets/Shaders/Standard.fx";
	mStandardEffect.Initialize(shaderFile);
	mStandardEffect.SetCamera(mCamera);
	mStandardEffect.SetDirectionalLight(mDirectionalLight);
}

void GameState::Terminate()
{
	mCharacter.Terminate();
	parasite.Terminate();
	zombie.Terminate();
	mStandardEffect.Terminate();
}

void GameState::Update(float deltaTime)
{
	UpdateCamera(deltaTime);

    // Update animator
    mCharacterAnimator.Update(deltaTime * mAnimationSpeed);
    mParasiteAnimator.Update(deltaTime * mAnimationSpeed);
    mZombieAnimator.Update(deltaTime * mAnimationSpeed);
}

void GameState::Render()
{
	SimpleDraw::AddGroundPlane(20.0f, mDrawSkeleton ? Colors::DarkRed : Colors::White);
	SimpleDraw::Render(mCamera);

	if (mDrawSkeleton)
	{
		// Draw skeleton for selected model only
		AnimationUtil::BoneTransforms boneTransforms;

		switch (gCurrentModel)
		{
		case CurrentModel::Timmy:
			AnimationUtil::ComputeBoneTransforms(mCharacter.modelId, boneTransforms, &mCharacterAnimator);
			AnimationUtil::DrawSkeleton(mCharacter.modelId, boneTransforms);
			break;
		case CurrentModel::Parasite:
			AnimationUtil::ComputeBoneTransforms(parasite.modelId, boneTransforms, &mParasiteAnimator);
			AnimationUtil::DrawSkeleton(parasite.modelId, boneTransforms);
			break;
		case CurrentModel::Zombie:
			AnimationUtil::ComputeBoneTransforms(zombie.modelId, boneTransforms, &mZombieAnimator);
			AnimationUtil::DrawSkeleton(zombie.modelId, boneTransforms);
			break;
		}
	}
	else
	{
		mStandardEffect.Begin();

			switch (gCurrentModel)
			{
			case CurrentModel::Timmy:
				mStandardEffect.Render(mCharacter);
				break;
			case CurrentModel::Parasite:
				mStandardEffect.Render(parasite);
				break;
			case CurrentModel::Zombie:
				mStandardEffect.Render(zombie);
				break;
			}

		mStandardEffect.End();
	}
}

void GameState::DebugUI()
{
	ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	// Modl Selection
	if (ImGui::CollapsingHeader("Model Selection", ImGuiTreeNodeFlags_DefaultOpen))
	{
		int currentModelIndex = static_cast<int>(gCurrentModel);
		if (ImGui::Combo("Current Model", &currentModelIndex, gObjectNames, std::size(gObjectNames)))
		{
			gCurrentModel = static_cast<CurrentModel>(currentModelIndex);
		}

		ImGui::Checkbox("Draw Skeleton", &mDrawSkeleton);
	}

	ImGui::Separator();

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
		// Get the currently selected model's render objects
		std::vector<RenderObject>* currentRenderObjects = nullptr;

		switch (gCurrentModel)
		{
		case CurrentModel::Timmy:
			currentRenderObjects = &mCharacter.renderObjects;
			break;
		case CurrentModel::Parasite:
			currentRenderObjects = &parasite.renderObjects;
			break;
		case CurrentModel::Zombie:
			currentRenderObjects = &zombie.renderObjects;
			break;
		}

		if (currentRenderObjects)
		{
			for (uint32_t i = 0; i < currentRenderObjects->size(); ++i)
			{
				Material& material = (*currentRenderObjects)[i].material;
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
	}

	ImGui::Separator();

    ImGui::DragFloat("AnimationSpeed", &mAnimationSpeed, 0.1f, 0.0f, 10.0f);
	int maxAnimations = mCharacterAnimator.GetAnimationCount();
    int maxParasiteAnimations = mParasiteAnimator.GetAnimationCount();
    int maxZombieAnimations = mZombieAnimator.GetAnimationCount();
    if (ImGui::DragInt("AnimIndex", &mClipIndex, 1, -1, maxAnimations - 1 )) // -1 for no animation
	{
		mCharacterAnimator.PlayAnimation(mClipIndex, true); // Always looping
        mParasiteAnimator.PlayAnimation(mClipIndex, true); 
        mZombieAnimator.PlayAnimation(mClipIndex, true);
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