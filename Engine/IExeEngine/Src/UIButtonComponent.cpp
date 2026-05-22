#include "Precompiled.h"
#include "UIButtonComponent.h"
#include "GameWorld.h"
#include "UIRenderService.h"
#include "UIRenderService.h"
#include "UISpriteComponent.h"

using namespace IExeEngine;
using namespace IExeEngine::Graphics;
using namespace IExeEngine::Input;

void UIButtonComponent::Initialize()
{
    for (ButtonStateEntry& buttonState : mButtonStates)
    {
        if (nullptr)
        {
            
        }
    }
}

void UIButtonComponent::Terminate()
{

}

void UIButtonComponent::Update(float deltaTime)
{

}

void UIButtonComponent::Render()
{

}

void UIButtonComponent::Deserialize(const rapidjson::Value& value)
{

}

Math::Vector2 UIButtonComponent::GetPosition(bool includeOrigin)
{
    return Math::Vector2();
}

void UIButtonComponent::SetCallback(ButtonCallback cb)
{

}

void UIButtonComponent::OnClick()
{

}
