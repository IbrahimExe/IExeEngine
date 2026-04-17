#include "Precompiled.h"
#include "GameObject.h"

using namespace IExeEngine;

static uint32_t gUniqueId = 0;

void GameObject::Initialize()
{
    ASSERT(!mInitialized, "GameObject: Already initialized!");

    mId = ++gUniqueId;
    mInitialized = true;
}

void GameObject::Terminate()
{
    for (auto& )
}

void GameObject::Update(float deltaTime)
{

}

void GameObject::DebugUI()
{
    ImGui::PushID(mId);
    if (ImGui::CollapsingHeader(mName.c_str()))
    {
        // WIll add more here later
    }
    ImGui::PopID();
}

void GameObject::SetName(std::string& name)
{
    mName = std::move(name);
}

const std::string& GameObject::GetName() const
{
    // TODO: insert return statement here
    return mName;
}

uint32_t GameObject::GetId() const
{
    // TODO: insert return statement here
    return mId;
}

const GameObjectHandle& GameObject::GetHandle() const
{
    // TODO: insert return statement here
    return mHandle;
}
