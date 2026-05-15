#include "Precompiled.h"
#include "UIFont.h"

#include "GraphicsSystem.h"

using namespace IExeEngine;
using namespace IExeEngine::Graphics;

void UIFont::StaticInitialize(FontType font)
{

}

void UIFont::StaticTerminate()
{

}

UIFont* UIFont::Get()
{
    return nullptr;
}

UIFont::~UIFont()
{

}

void UIFont::Initialize(FontType font)
{

}

void UIFont::Terminate()
{

}

void UIFont::DrawString(const wchar_t* str, const Math::Vector2& position, const Color& color, float size)
{

}

float UIFont::GetStringWidth(const wchar_t* str, float size) const
{
    return 0.0f;
}
