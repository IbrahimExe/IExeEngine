#include "HalftoneEffect.h"
#include "Precompiled.h"

#include "VertexTypes.h"
#include "Camera.h"
#include "RenderObject.h"
#include "TextureManager.h"
#include "GraphicsSystem.h"

using namespace IExeEngine;
using namespace IExeEngine::Graphics;

void HalftoneEffect::Initialize(const std::filesystem::path& path)
{
    // Initialize CBs (sizes validated by TypedConstantBuffer)
    mTransformBuffer.Initialize();
    mLightBuffer.Initialize();
    mMaterialBuffer.Initialize();
    mHalftoneBuffer.Initialize();

    // Shaders
    mVertexShader.Initialize<Vertex>(path);
    mPixelShader.Initialize(path);

    // Sampler (reuse same sampler style as StandardEffect)
    mSampler.Initialize(Sampler::Filter::Linear, Sampler::AddressMode::Wrap);
}

void HalftoneEffect::Terminate()
{
    mSampler.Terminate();
    mPixelShader.Terminate();
    mVertexShader.Terminate();

    mHalftoneBuffer.Terminate();
    mMaterialBuffer.Terminate();
    mLightBuffer.Terminate();
    mTransformBuffer.Terminate();
}

void HalftoneEffect::Begin()
{
    // Bind shaders and common buffers (mirror StandardEffect bind layout)
    mVertexShader.Bind();
    mPixelShader.Bind();

    mSampler.BindPS(0);
    mSampler.BindVS(0);

    // Transform buffer layout: same slot as StandardEffect (b0)
    mTransformBuffer.BindVS(0);

    // Light buffer the same as StandardEffect (b1)
    mLightBuffer.BindVS(1);
    mLightBuffer.BindPS(1);

    // Material buffer slot (b2) - not required but keep parity (bind to PS)
    mMaterialBuffer.BindPS(2);

    // Our halftone data -> choose PS slot 4 (register b4 in shader). We avoid colliding with StandardEffect's settings (b3).
    mHalftoneBuffer.BindPS(4);
}

void HalftoneEffect::End()
{

}

void HalftoneEffect::Render(const RenderObject& renderObject)
{
    ASSERT(mCamera != nullptr, "HalftoneEffect::Render - camera not set!");
    ASSERT(mDirectionalLight != nullptr, "HalftoneEffect::Render - directional light not set!");

    // 1) Transform CB (same as StandardEffect)
    const Math::Matrix4 matWorld = renderObject.transform.GetMatrix4();
    const Math::Matrix4 matView = mCamera->GetViewMatrix();
    const Math::Matrix4 matProj = mCamera->GetProjectionMatrix();
    const Math::Matrix4 matFinal = matWorld * matView * matProj;

    TransformData tData;
    tData.wvp = Math::Transpose(matFinal);
    tData.world = Math::Transpose(matWorld);
    tData.viewPosition = mCamera->GetPosition();

    // light WVP for shadows (if available)
    if (mLightCamera != nullptr && mShadowMap != nullptr)
    {
        const Math::Matrix4 matLightView = mLightCamera->GetViewMatrix();
        const Math::Matrix4 matLightProj = mLightCamera->GetProjectionMatrix();
        tData.lwvp = Math::Transpose(matWorld * matLightView * matLightProj);
        mShadowMap->BindPS(4); // sample slot same as StandardEffect's shadow map (t4)
    }
    mTransformBuffer.Update(tData);

    // 2) Light buffer update (same type as StandardEffect)
    if (mDirectionalLight)
        mLightBuffer.Update(*mDirectionalLight);

    // 3) Material buffer - update with object's material so shader can sample diffuse/specular if needed
    mMaterialBuffer.Update(renderObject.material);

    // 4) Halftone params
    HalftoneData hData;
    hData.cellCountX = static_cast<float>(mCellCountX);
    hData.cellCountY = static_cast<float>(mCellCountY);
    hData.opacity = mOpacity;
    hData.threshold = mThreshold;
    mHalftoneBuffer.Update(hData);

    // 5) Bind textures same as StandardEffect (PS slots 0..3)
    TextureManager* tm = TextureManager::Get();
    tm->BindPS(renderObject.diffuseMapId, 0);
    tm->BindPS(renderObject.specMapId, 1);
    tm->BindPS(renderObject.normalMapId, 2);
    tm->BindVS(renderObject.bumpMapId, 3);

    // 6) Render geometry
    renderObject.meshBuffer.Render();
}

void HalftoneEffect::Render(const RenderGroup& renderGroup)
{
    ASSERT(mCamera != nullptr, "HalftoneEffect::Render - camera not set!");
    ASSERT(mDirectionalLight != nullptr, "HalftoneEffect::Render - directional light not set!");

    const Math::Matrix4 matWorld = renderGroup.transform.GetMatrix4();
    const Math::Matrix4 matView = mCamera->GetViewMatrix();
    const Math::Matrix4 matProj = mCamera->GetProjectionMatrix();
    const Math::Matrix4 matFinal = matWorld * matView * matProj;

    TransformData tData;
    tData.wvp = Math::Transpose(matFinal);
    tData.world = Math::Transpose(matWorld);
    tData.viewPosition = mCamera->GetPosition();

    if (mLightCamera != nullptr && mShadowMap != nullptr)
    {
        const Math::Matrix4 matLightView = mLightCamera->GetViewMatrix();
        const Math::Matrix4 matLightProj = mLightCamera->GetProjectionMatrix();
        tData.lwvp = Math::Transpose(matWorld * matLightView * matLightProj);
        mShadowMap->BindPS(4);
    }
    mTransformBuffer.Update(tData);

    if (mDirectionalLight)
        mLightBuffer.Update(*mDirectionalLight);

    // Update halftone params once for the group
    HalftoneData hData;
    hData.cellCountX = static_cast<float>(mCellCountX);
    hData.cellCountY = static_cast<float>(mCellCountY);
    hData.opacity = mOpacity;
    hData.threshold = mThreshold;
    mHalftoneBuffer.Update(hData);

    TextureManager* tm = TextureManager::Get();

    for (const RenderObject& renderObject : renderGroup.renderObjects)
    {
        mMaterialBuffer.Update(renderObject.material);

        tm->BindPS(renderObject.diffuseMapId, 0);
        tm->BindPS(renderObject.specMapId, 1);
        tm->BindPS(renderObject.normalMapId, 2);
        tm->BindVS(renderObject.bumpMapId, 3);

        renderObject.meshBuffer.Render();
    }
}

void HalftoneEffect::DebugUI()
{
    if (ImGui::CollapsingHeader("HalftoneEffect", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::DragInt("Cells X", &mCellCountX, 1, 1, 300);
        ImGui::DragInt("Cells Y", &mCellCountY, 1, 1, 300);
        ImGui::DragFloat("Opacity", &mOpacity, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Threshold", &mThreshold, 0.001f, 0.0f, 0.2f);
    }
}
