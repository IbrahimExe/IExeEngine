#include "CustomDebugDrawComponent.h"
#include "CustomDebugDrawService.h"

using namespace IExeEngine;
using namespace IExeEngine::Graphics;
using namespace IExeEngine::Math;

void CustomDebugDrawComponent::Initialize()
{
	mTransformComponent = GetOwner().GetComponent<TransformComponent>();
	CustomDebugDrawService* debugDrawService = GetOwner().GetWorld().GetService<CustomDebugDrawService>();
	if (debugDrawService != nullptr)
	{
		debugDrawService->Register(this);
	}
}

void CustomDebugDrawComponent::Terminate()
{
	CustomDebugDrawService* debugDrawService = GetOwner().GetWorld().GetService<CustomDebugDrawService>();
	if (debugDrawService != nullptr)
	{
		debugDrawService->Unregister(this);
	}
}

void CustomDebugDrawComponent::DebugUI()
{
	ImGui::DragFloat3("Position", &mPosition.x);
	ImGui::ColorEdit4("Color", &mColor.r);
}

void CustomDebugDrawComponent::Deserialize(const rapidjson::Value& value)
{
	int slices = mSlices;
	int rings = mRings;
	SaveUtil::ReadInt("Slices", slices, value);
	SaveUtil::ReadInt("Rings", rings, value);
	SaveUtil::ReadFloat("Radius", mRadius, value);
	SaveUtil::ReadVector3("Position", mPosition, value);
	SaveUtil::ReadColor("Color", mColor, value);
	mSlices = slices;
	mRings = rings;
}

void CustomDebugDrawComponent::AddDebugDraw() const
{
	Vector3 worldSpace = mPosition;
	if (mTransformComponent != nullptr)
	{
		worldSpace = TransformCoord(mPosition, mTransformComponent->GetMatrix4());
	}
	SimpleDraw::AddSphere(mSlices, mRings, mRadius, mColor, worldSpace);
}
