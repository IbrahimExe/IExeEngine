#include "Precompiled.h"
#include "HalftoneEffect.h"

#include "VertexTypes.h"
#include "Camera.h"
#include "RenderObject.h"
#include "TextureManager.h"
#include "GraphicsSystem.h"

using namespace IExeEngine;
using namespace IExeEngine::Graphics;

void HalftoneEffect::Initialize(const std::filesystem::path& path)
{
    mTransformBuffer.Initialize();
    mLightBuffer.Initialize();
    mMaterialBuffer.Initialize();
    mSettingsBuffer.Initialize();
    mHalftoneBuffer.Initialize();

    // Shaders
    mVertexShader.Initialize<Vertex>(path);
    mPixelShader.Initialize(path);

    // Sampler
    mSampler.Initialize(Sampler::Filter::Linear, Sampler::AddressMode::Wrap);
}

void HalftoneEffect::Terminate()
{
    mSampler.Terminate();
    mPixelShader.Terminate();
    mVertexShader.Terminate();

    mHalftoneBuffer.Terminate();
    mSettingsBuffer.Terminate();
    mMaterialBuffer.Terminate();
    mLightBuffer.Terminate();
    mTransformBuffer.Terminate();
}

void HalftoneEffect::Begin()
{
    mVertexShader.Bind();
    mPixelShader.Bind();

    mSampler.BindPS(0);
    mSampler.BindVS(0);

    // register(b0)
    mTransformBuffer.BindVS(0);

    // register(b1)
    mLightBuffer.BindVS(1);
    mLightBuffer.BindPS(1);

    // register(b2)
    mMaterialBuffer.BindPS(2);

    // register(b3) 
    mSettingsBuffer.BindVS(3);
    mSettingsBuffer.BindPS(3);

    // register(b4): Halftone-specific params
    mHalftoneBuffer.BindPS(4);
}

void HalftoneEffect::End()
{

}

void HalftoneEffect::Render(const RenderObject& renderObject)
{
    ASSERT(mCamera != nullptr, "HalftoneEffect::Render - camera not set!");
    ASSERT(mDirectionalLight != nullptr, "HalftoneEffect::Render - directional light not set!");

    // 1) Transform
    const Math::Matrix4 matWorld = renderObject.transform.GetMatrix4();
    const Math::Matrix4 matView = mCamera->GetViewMatrix();
    const Math::Matrix4 matProj = mCamera->GetProjectionMatrix();
    const Math::Matrix4 matFinal = matWorld * matView * matProj;

    TransformData tData;
    tData.wvp = Math::Transpose(matFinal);
    tData.world = Math::Transpose(matWorld);
    tData.viewPosition = mCamera->GetPosition();

    // Shadows: compute light WVP if we have a light camera & shadow map
    if (mLightCamera != nullptr && mShadowMap != nullptr)
    {
        const Math::Matrix4 matLightView = mLightCamera->GetViewMatrix();
        const Math::Matrix4 matLightProj = mLightCamera->GetProjectionMatrix();
        tData.lwvp = Math::Transpose(matWorld * matLightView * matLightProj);

        // Bind shadow map (t4) same as StandardEffect
        mShadowMap->BindPS(4);
    }
    mTransformBuffer.Update(tData);

    // 2) Light buffer
    if (mDirectionalLight)
        mLightBuffer.Update(*mDirectionalLight);

    // 3) Material buffer
    mMaterialBuffer.Update(renderObject.material);

    // 4) Settings buffer
    SettingsData settings;
    settings.useDiffuseMap = (renderObject.diffuseMapId > 0 && mSettingsData.useDiffuseMap > 0) ? 1 : 0;
    settings.useSpecMap = (renderObject.specMapId > 0 && mSettingsData.useSpecMap > 0) ? 1 : 0;
    settings.useNormalMap = (renderObject.normalMapId > 0 && mSettingsData.useNormalMap > 0) ? 1 : 0;
    settings.useBumpMap = (renderObject.bumpMapId > 0 && mSettingsData.useBumpMap > 0) ? 1 : 0;
    settings.bumpIntensity = mSettingsData.bumpIntensity;
    settings.useShadowMap = (mShadowMap != nullptr && mSettingsData.useShadowMap > 0) ? 1 : 0;
    settings.depthBias = mSettingsData.depthBias;
    mSettingsBuffer.Update(settings);

    // 5) Halftone data
    HalftoneData hData;
    hData.cellCountX = static_cast<float>(mCellCountX);
    hData.cellCountY = static_cast<float>(mCellCountY);
    hData.opacity = mOpacity;
    hData.threshold = mThreshold;
    mHalftoneBuffer.Update(hData);

    // 6) Bind textures
    TextureManager* tm = TextureManager::Get();
    tm->BindPS(renderObject.diffuseMapId, 0);
    tm->BindPS(renderObject.specMapId, 1);
    tm->BindPS(renderObject.normalMapId, 2);
    tm->BindVS(renderObject.bumpMapId, 3);

    // 7) Render object
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

    // Halftone params
    HalftoneData hData;
    hData.cellCountX = static_cast<float>(mCellCountX);
    hData.cellCountY = static_cast<float>(mCellCountY);
    hData.opacity = mOpacity;
    hData.threshold = mThreshold;
    mHalftoneBuffer.Update(hData);

    TextureManager* tm = TextureManager::Get();

    for (const RenderObject& renderObject : renderGroup.renderObjects)
    {
        // Per-object settings & material
        SettingsData settings;
        settings.useDiffuseMap = (renderObject.diffuseMapId > 0 && mSettingsData.useDiffuseMap > 0) ? 1 : 0;
        settings.useSpecMap = (renderObject.specMapId > 0 && mSettingsData.useSpecMap > 0) ? 1 : 0;
        settings.useNormalMap = (renderObject.normalMapId > 0 && mSettingsData.useNormalMap > 0) ? 1 : 0;
        settings.useBumpMap = (renderObject.bumpMapId > 0 && mSettingsData.useBumpMap > 0) ? 1 : 0;
        settings.bumpIntensity = mSettingsData.bumpIntensity;
        settings.useShadowMap = (mShadowMap != nullptr && mSettingsData.useShadowMap > 0) ? 1 : 0;
        settings.depthBias = mSettingsData.depthBias;
        mSettingsBuffer.Update(settings);

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


        bool useDiffuse = mSettingsData.useDiffuseMap > 0;
        if (ImGui::Checkbox("UseDiffuseMap##Halftone", &useDiffuse)) mSettingsData.useDiffuseMap = useDiffuse ? 1 : 0;

        bool useSpec = mSettingsData.useSpecMap > 0;
        if (ImGui::Checkbox("UseSpecMap##Halftone", &useSpec)) mSettingsData.useSpecMap = useSpec ? 1 : 0;

        bool useNormal = mSettingsData.useNormalMap > 0;
        if (ImGui::Checkbox("UseNormalMap##Halftone", &useNormal)) mSettingsData.useNormalMap = useNormal ? 1 : 0;

        bool useBump = mSettingsData.useBumpMap > 0;
        if (ImGui::Checkbox("UseBumpMap##Halftone", &useBump)) mSettingsData.useBumpMap = useBump ? 1 : 0;

        ImGui::DragFloat("BumpIntensity##Halftone", &mSettingsData.bumpIntensity, 0.01f, -5.0f, 5.0f);
        bool useShadow = mSettingsData.useShadowMap > 0;
        if (ImGui::Checkbox("UseShadowMap##Halftone", &useShadow)) mSettingsData.useShadowMap = useShadow ? 1 : 0;
        ImGui::DragFloat("DepthBias##Halftone", &mSettingsData.depthBias, 0.000001f, 0.0f, 1.0f, "%.6f");
    }
}
