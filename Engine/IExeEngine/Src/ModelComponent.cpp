#include "Precompiled.h"
#include "ModelComponent.h"
#include "SaveUtil.h"

using namespace IExeEngine;

void ModelComponent::Initialize()
{
    Graphics::ModelManager* mm = Graphics::ModelManager::Get();
    mModelId = mm->GetModelId(mFileName);

    if (mm->GetModel(mModelId) == nullptr)
    {
        mModelId = mm->LoadModel(mFileName);

        LOG("ModelComponent: Loading %zu animset(s) for %s",
            mAnimations.size(), mFileName.c_str());

        for (const std::string& animation : mAnimations)
        {
            LOG("ModelComponent: Adding -> %s", animation.c_str());
            mm->AddAnimation(mModelId, animation);
        }
    }
    else
    {
        LOG("ModelComponent: CACHE HIT - animations skipped for %s", mFileName.c_str());
    }

    ASSERT(mm->GetModel(mModelId) != nullptr,
        "ModelComponent: Failed to load model %s!", mFileName.c_str());
    RenderObjectComponent::Initialize();
}

void ModelComponent::Terminate()
{
	RenderObjectComponent::Terminate();
}

void ModelComponent::Deserialize(const rapidjson::Value& value)
{
	RenderObjectComponent::Deserialize(value);

	mAnimations.clear();
	SaveUtil::ReadString("FileName", mFileName, value);
	SaveUtil::ReadStringArray("Animations", mAnimations, value);
}

Graphics::ModelId ModelComponent::GetModelId() const
{
	return mModelId;
}

const Graphics::Model& ModelComponent::GetModel() const
{
	return *Graphics::ModelManager::Get()->GetModel(mModelId);
}
