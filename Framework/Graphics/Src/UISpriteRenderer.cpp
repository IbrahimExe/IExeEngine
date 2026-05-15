#include "Precompiled.h"
#include "UISpriteRenderer.h"
#include "UISprite.h"

#include "GraphicsSystem.h"

using namespace IExeEngine;
using namespace IExeEngine::Graphics;

namespace
{
	std::unique_ptr<UISpriteRenderer> sUISpriteRenderer;
}

void UISpriteRenderer::StaticInitialize()
{
	ASSERT(sUISpriteRenderer == nullptr, "UISpriteRenderer: Is already initialized!");
	sUISpriteRenderer = std::make_unique<UISpriteRenderer>();
	sUISpriteRenderer->Initialize();
}

void UISpriteRenderer::StaticTerminate()
{
	if (sUISpriteRenderer != nullptr)
	{
		sUISpriteRenderer->Terminate();
		sUISpriteRenderer.reset();
	}
}

UISpriteRenderer* UISpriteRenderer::Get()
{
	ASSERT(sUISpriteRenderer != nullptr, "UISpriteRenderer: Is not initialized!");
	return sUISpriteRenderer.get();
}

UISpriteRenderer::~UISpriteRenderer()
{
	ASSERT(mCommonStates == nullptr && mSpriteBatch == nullptr, "UISpriteRenderer: Terminate must be called!");
}

void UISpriteRenderer::Initialize()
{
	GraphicsSystem* gs = GraphicsSystem::Get();
	mCommonStates = new DirectX::CommonStates(gs->GetDevice());
	mSpriteBatch = new DirectX::SpriteBatch(gs->GetContext());
}

void UISpriteRenderer::Terminate()
{
	if (mCommonStates != nullptr)
	{
		delete mCommonStates;
		mCommonStates = nullptr;
	}
	if (mSpriteBatch != nullptr)
	{
		delete mSpriteBatch;
		mSpriteBatch = nullptr;
	}
}

void UISpriteRenderer::BeginRender()
{
	mSpriteBatch->Begin(DirectX::SpriteSortMode_Deferred, mCommonStates->NonPremultiplied());
}

void UISpriteRenderer::EndRender()
{
	mSpriteBatch->End();

	// Restore the state object
	auto blendState = mCommonStates->Opaque();
	auto depthStencilState = mCommonStates->DepthDefault();
	auto restarizerState = mCommonStates->CullCounterClockwise();

	auto context = GraphicsSystem::Get()->GetContext();
	context->OMSetBlendState(blendState, nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(depthStencilState, 0);
	context->RSSetState(restarizerState);
}

void UISpriteRenderer::Render(const UISprite& uiSprite)
{
	const Texture* texture = TextureManager::Get()->GetTexture(uiSprite.mTextureId);
	if (texture != nullptr)
	{
		mSpriteBatch->Draw(
			(ID3D11ShaderResourceView*)texture->GetRawData(),
			uiSprite.mPosition,
			&uiSprite.mRect,
			uiSprite.mColor,
			uiSprite.mRotation,
			uiSprite.mOrigin,
			uiSprite.mScale,
			uiSprite.mFlip);
	}
}