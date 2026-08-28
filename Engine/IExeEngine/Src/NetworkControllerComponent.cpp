#include "Precompiled.h"
#include "NetworkControllerComponent.h"
#include "TransformComponent.h"
#include "RigidBodyComponent.h"
#include "GameWorld.h"
#include "SaveUtil.h"
#include "NetworkService.h"

using namespace IExeEngine;
using namespace IExeEngine::Input;

void NetworkControllerComponent::Initialize()
{
	mRigidBodyComponent = GetOwner().GetComponent<RigidBodyComponent>();
	mTransformComponent = GetOwner().GetComponent<TransformComponent>();
	ASSERT(mRigidBodyComponent != nullptr || mTransformComponent != nullptr, "NetworkControllerComponent: Needs either a RigidBodyComponent or a TransformComponent to work!");

	mNetworkController = std::make_unique<Network::NetworkController>();

	NetworkService* networkService = GetOwner().GetWorld().GetService<NetworkService>();
	networkService->RegisterPlayer(this);
}

void NetworkControllerComponent::Terminate()
{
	NetworkService* networkService = GetOwner().GetWorld().GetService<NetworkService>();
	networkService->UnregisterPlayer(this);
	mNetworkController.reset();
	mRigidBodyComponent = nullptr;
	mTransformComponent = nullptr;
}

void NetworkControllerComponent::Update(float deltaTime)
{

	Math::Vector2 moveInput = Math::Vector2::Zero;
	Network::EventInput inputEvent;

	if (mRemote)
	{
		Math::Vector3 newPosition = Math::Vector3::Zero;
		if (mNetworkController->TryGetNewPosition(newPosition))
		{
			if (mRigidBodyComponent != nullptr)
			{
				mRigidBodyComponent->SetPosition(newPosition);
			}
			else
			{
				mTransformComponent->position = newPosition;
			}
		}
		inputEvent = mNetworkController->GetInput();
		const float moveSpeed = (inputEvent.shiftSpeed > 0) ? mShiftSpeed : mMoveSpeed;
		moveInput.x = (float)inputEvent.moveX * moveSpeed;
		moveInput.y = (float)inputEvent.moveY * moveSpeed;
	}
	else
	{
		auto input = InputSystem::Get();
		const float moveSpeed = ((input->IsKeyDown(KeyCode::LSHIFT)) ? mShiftSpeed : mMoveSpeed);
		const float turnSpeed = mTurnSpeed;
		inputEvent.jump = input->IsKeyPressed(KeyCode::SPACE) ? 1 : 0;
		inputEvent.shiftSpeed = input->IsKeyDown(KeyCode::LSHIFT) ? 1 : 0;
		if (input->IsKeyDown(KeyCode::W))
		{
			moveInput.y = moveSpeed;
			inputEvent.moveY += 1;
		}
		if (input->IsKeyDown(KeyCode::S))
		{
			moveInput.y = -moveSpeed;
			inputEvent.moveY -= 1;
		}
		if (input->IsKeyDown(KeyCode::D))
		{
			moveInput.x = moveSpeed;
			inputEvent.moveX += 1;
		}
		if (input->IsKeyDown(KeyCode::A))
		{
			moveInput.x = -moveSpeed;
			inputEvent.moveX -= 1;
		}

		mNetworkController->SetInput(inputEvent);
	}

	if (inputEvent.jump > 0)
	{
		// Jump
		if (mRigidBodyComponent != nullptr)
		{
			Math::Vector3 vel = mRigidBodyComponent->GetVelocity();
			vel.y = mJumpSpeed;
			mRigidBodyComponent->SetVelocity(vel);
		}
	}
	if (mRigidBodyComponent != nullptr)
	{
		Math::Vector3 vel = mRigidBodyComponent->GetVelocity();
		vel.x = moveInput.x;
		vel.z = moveInput.y;
		mRigidBodyComponent->SetVelocity(vel);
	}
	else
	{
		Math::Vector3 vel = { moveInput.x, 0.0f, moveInput.y };
		mTransformComponent->position += vel * deltaTime;
	}

}

void NetworkControllerComponent::DebugUI()
{
	ImGui::DragFloat("MoveSpeed", &mMoveSpeed, 0.1f, 0.1f, 100.0f);
	ImGui::DragFloat("ShiftSpeed", &mShiftSpeed, 0.1f, 0.1f, 1000.0f);
	ImGui::DragFloat("TurnSpeed", &mTurnSpeed, 0.001f, 0.01f, 1.0f);
	ImGui::DragFloat("JumpSpeed", &mJumpSpeed, 0.001f, 0.1f, 1.0f);
}

void NetworkControllerComponent::Deserialize(const rapidjson::Value& value)
{
	SaveUtil::ReadFloat("MoveSpeed", mMoveSpeed, value);
	SaveUtil::ReadFloat("ShiftSpeed", mShiftSpeed, value);
	SaveUtil::ReadFloat("TurnSpeed", mTurnSpeed, value);
	SaveUtil::ReadFloat("JumpSpeed", mJumpSpeed, value);
}

Network::NetworkController* NetworkControllerComponent::GetNetworkController()
{
	return mNetworkController.get();
}
void NetworkControllerComponent::SetRemote(bool remote)
{
	mRemote = remote;
}