#include "Precompiled.h"
#include "ModelComponent.h"
#include "SaveUtil.h"

using namespace IExeEngine;

void IExeEngine::ModelComponent::Initialize()
{

}

void IExeEngine::ModelComponent::Terminate()
{
    RenderObjectComponent::Terminate();
}

void IExeEngine::ModelComponent::Deserialize(const rapidjson::Value& value)
{

}

Graphics::ModelId IExeEngine::ModelComponent::GetModelId() const
{
    return Graphics::ModelId();
}

Graphics::Model& IExeEngine::ModelComponent::GetModel() const
{
    // TODO: insert return statement here
}
