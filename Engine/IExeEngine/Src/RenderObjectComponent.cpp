#include "Precompiled.h"
#include "RenderObjectComponent.h"
#include "SaveUtil.h"

using namespace IExeEngine;

void RenderObjectComponent::Initialize()
{

}

void RenderObjectComponent::Terminate()
{

}

void RenderObjectComponent::Deserialize(const rapidjson::Value& value)
{
    SaveUtil::ReadBool("CastShadow", mCastShadow, value);
}

bool RenderObjectComponent::CanCastShadow() const
{
    return mCastShadow;
}
