#include "Precompiled.h"
#include "CameraComponent.h"

using namespace IExeEngine;

void CameraComponent::Initialize()
{

}

void CameraComponent::Terminate()
{

}

void CameraComponent::DebugUI()
{
	Math::Vector3 pos = mCamera.GetPosition();
	Math::Vector3 dir = mCamera.GetDirection();
	if (ImGui::DragFloat3("Position##Camera", &pos.x, 0.1f))
	{
		mCamera.SetPosition(pos);
	}
	if (ImGui::DragFloat3("Direction##Camera", &dir.x, 0.1f))
	{
		mCamera.SetDirection(dir);
	}
}

Graphics::Camera& CameraComponent::GetCamera()
{
	return mCamera;
}

const Graphics::Camera& CameraComponent::GetCamera() const
{
	return mCamera;
}