#include "Precompiled.h"
#include "Texture.h"

#include "GraphicsSystem.h"
#include <DirectXTK/Inc/WICTextureLoader.h>

using namespace IExeEngine;
using namespace IExeEngine::Graphics;

void Texture::UnbindPS(uint32_t slot)
{
    static ID3D11ShaderResourceView* dummy = nullptr;
    GraphicsSystem::Get()->GetContext()->HSSetShaderResources(slot, 1, &dummy);
}

Texture::~Texture()
{
    ASSERT(mShaderResourceView == nullptr, "Texture: Terminate must be called!");
}

Texture::Texture(Texture&& rhs) noexcept
    : mShaderResourceView(rhs.mShaderResourceView)
{
    rhs.mShaderResourceView = nullptr;
}

Texture& Texture::operator=(Texture&& rhs) noexcept
{
    mShaderResourceView = rhs.mShaderResourceView;
    rhs.mShaderResourceView = nullptr;
    return *this;
}

void Texture::Initialize(const std::filesystem::path& fileName)
{
    auto device = GraphicsSystem::Get()->GetDevice();
    auto context = GraphicsSystem::Get()->GetContext();

    HRESULT hr = DirectX::CreateWICTextureFromFile(device, context, fileName.c_str(), nullptr, &mShaderResourceView);
    ASSERT(SUCCEEDED(hr), "Texture: Failed to create texture %s!", fileName.u8string().c_str());

    // To obtain width/ height
    // We need to get the resource info, convert to texture2D, then get texture description
    ID3D11Resource* resource = nullptr;
    mShaderResourceView->GetResource(&resource);

    ID3D11Texture2D* texture2D = nullptr;
    hr = resource->QueryInterface(&texture2D);
    ASSERT(SUCCEEDED(hr), "Texture: Failed to find texture data!");

    D3D11_TEXTURE2D_DESC desc;
    texture2D->GetDesc(&desc);
    mWidth = static_cast<uint32_t>(desc.Width);
    mHeight = static_cast<uint32_t>(desc.Height);

    SafeRelease(texture2D);
    SafeRelease(resource);
}

void Texture::Terminate()
{
    SafeRelease(mShaderResourceView);
}

void Texture::BindVS(uint32_t slot) const
{
    auto context = GraphicsSystem::Get()->GetContext();
    context->VSSetShaderResources(slot, 1, &mShaderResourceView);
}

void Texture::BindPS(uint32_t slot) const
{
    auto context = GraphicsSystem::Get()->GetContext();
    context->PSSetShaderResources(slot, 1, &mShaderResourceView);
}

void* Texture::GetRawData() const
{
    return mShaderResourceView;
}

uint32_t Texture::GetWidth() const
{
    return mWidth;
}

uint32_t Texture::GetHeight() const
{
    return mHeight;
}