#include "Precompiled.h"
#include "CameraComponent.h"
#include "SaveUtil.h"

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

void CameraComponent::Deserialize(const rapidjson::Value& value)
{
	Math::Vector3 readValue = Math::Vector3::Zero;
    if (SaveUtil::ReadVector3("Position", readValue, value))
    {
        mCamera.SetPosition(readValue); // Set the values to the one found in the json
    }
	if (SaveUtil::ReadVector3("LookAt", readValue, value))
	{
		mCamera.SetLookAt(readValue);
	}
    if (SaveUtil::ReadVector3("Direction", readValue, value))
    {
        mCamera.SetDirection(readValue);
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