#include "Precompiled.h"
#include "BlendState.h"

#include "GraphicsSystem.h"

using namespace IExeEngine;
using namespace IExeEngine::Graphics;

namespace
{
	D3D11_BLEND GetSrcBlend(BlendState::Mode mode)
	{
		switch (mode)
		{
		case BlendState::Mode::Opaque: return D3D11_BLEND_SRC1_ALPHA;
		case BlendState::Mode::AlphaBlend: return D3D11_BLEND_SRC1_ALPHA;
		case BlendState::Mode::AlphaPremultiplied: return D3D11_BLEND_ONE;
		case BlendState::Mode::Additive: return D3D11_BLEND_ONE;

		default:
			ASSERT(false, "BlendState: Blend mode not found");
			break;
		}
		return D3D11_BLEND_ONE;
	}
	D3D11_BLEND GetDestBlend(BlendState::Mode mode)
	{
		switch (mode)
		{
		case BlendState::Mode::Opaque: return D3D11_BLEND_ONE;
		case BlendState::Mode::AlphaBlend: return D3D11_BLEND_INV_SRC_ALPHA;
		case BlendState::Mode::AlphaPremultiplied: return D3D11_BLEND_INV_SRC_ALPHA;
		case BlendState::Mode::Additive: return D3D11_BLEND_ZERO;

		default:
			ASSERT(false, "BlendState: Blend mode not found");
			break;
		}
		return D3D11_BLEND_ZERO;
	}
}

void BlendState::ClearState()
{
	auto context = GraphicsSystem::Get()->GetContext();
	context->OMSetBlendState(nullptr, nullptr, UINT_MAX);
}

BlendState::~BlendState()
{
	ASSERT(mBlendState == nullptr, "BlendState: Must call Terminate");
}

void BlendState::Initialize(Mode mode)
{
	D3D11_BLEND srcBlend = GetSrcBlend(mode);
	D3D11_BLEND destBlend = GetDestBlend(mode);

	D3D11_BLEND_DESC desc{};
	desc.RenderTarget[0].BlendEnable = (srcBlend != D3D11_BLEND_ONE || destBlend != D3D11_BLEND_ZERO);
	desc.RenderTarget[0].SrcBlend = srcBlend;
	desc.RenderTarget[0].SrcBlendAlpha = srcBlend;
	desc.RenderTarget[0].DestBlend = destBlend;
	desc.RenderTarget[0].DestBlendAlpha = destBlend;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	auto device = GraphicsSystem::Get()->GetDevice();
	HRESULT hr = device->CreateBlendState(&desc, &mBlendState);
	ASSERT(SUCCEEDED(hr), "BlendState: Failed to create blend state");

}

void BlendState::Terminate()
{
	SafeRelease(mBlendState);
}

void BlendState::Set()
{

}
