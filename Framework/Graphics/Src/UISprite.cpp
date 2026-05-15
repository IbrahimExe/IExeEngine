#include "Precompiled.h"
#include "UISprite.h"

using namespace IExeEngine;
using namespace IExeEngine::Graphics;

UISprite::~UISprite()
{
	ASSERT(mTextureId == 0, "UISprite: Terminate must be called!");
}

void UISprite::Initialize(const std::filesystem::path& filePath)
{
	TextureManager* tm = TextureManager::Get();
	mTextureId = tm->LoadTexture(filePath);
	const Texture* texture = tm->GetTexture(mTextureId);
	ASSERT(texture != nullptr, "UISprite: Failed to load texture %s!", filePath.u8string().c_str());
	SetRect(0, 0, texture->GetWidth(), texture->GetHeight());
}

void UISprite::Terminate()
{
	TextureManager::Get()->ReleaseTexture(mTextureId);
	mTextureId = 0;
}

void UISprite::SetPosition(const Math::Vector2& position)
{
	mPosition.x = position.x;
	mPosition.y = position.y;
}

void UISprite::SetScale(const Math::Vector2& scale)
{
	mScale.x = scale.x;
	mScale.y = scale.y;
}

void UISprite::SetRect(uint32_t top, uint32_t left, uint32_t right, uint32_t bottom)
{
	mRect.top = top;
	mRect.left = left;
	mRect.right = right;
	mRect.bottom = bottom;
	UpdateOrigin();
}

void UISprite::SetPivot(Pivot pivot)
{
	mPivot = pivot;
	UpdateOrigin();
}

void UISprite::SetFlip(Flip flip)
{
	switch (flip)
	{
	case Flip::None:			mFlip = DirectX::SpriteEffects_None; break;
	case Flip::Horizontal:		mFlip = DirectX::SpriteEffects_FlipHorizontally; break;
	case Flip::Vertical:		mFlip = DirectX::SpriteEffects_FlipVertically; break;
	case Flip::Both:			mFlip = DirectX::SpriteEffects_FlipBoth; break;
	default:
		break;
	}
}

void UISprite::SetColor(const Color& color)
{
	mColor.m128_f32[0] = color.r;
	mColor.m128_f32[1] = color.g;
	mColor.m128_f32[2] = color.b;
	mColor.m128_f32[3] = color.a;
}

void UISprite::SetRotation(float rotation)
{
	mRotation = rotation;
}

bool UISprite::IsInSprite(float x, float y) const
{
	const float width = mRect.right - mRect.left;
	const float height = mRect.bottom - mRect.top;
	return x >= mPosition.x - mOrigin.x && x <= mPosition.x + width - mOrigin.x &&
		y >= mPosition.y - mOrigin.y && y <= mPosition.y + height - mOrigin.y;
}

void UISprite::GetOrigin(float& x, float& y)
{
	x = mOrigin.x;
	y = mOrigin.y;
}

constexpr DirectX::XMFLOAT2 gOffsets[] =
{
	{ 0.0f, 0.0f }, // TopLeft
	{ 0.5f, 0.0f }, // Top
	{ 1.0f, 0.0f }, // TopRight
	{ 0.0f, 0.5f }, // CenterLeft
	{ 0.5f, 0.5f }, // Center
	{ 1.0f, 0.5f }, // CenterRight
	{ 0.0f, 1.0f }, // BottomLeft
	{ 0.5f, 1.0f }, // Bottom
	{ 1.0f, 1.0f }  // BottomRight
};

void UISprite::UpdateOrigin()
{
	const float width = mRect.right - mRect.left;
	const float height = mRect.bottom - mRect.top;
	auto index = static_cast<std::underlying_type_t<Pivot>>(mPivot);

	mOrigin = { width * gOffsets[index].x, height * gOffsets[index].y };
}