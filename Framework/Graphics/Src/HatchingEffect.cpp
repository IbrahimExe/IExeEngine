#include "Precompiled.h"
#include "HatchingEffect.h"

#include "VertexTypes.h"
#include "Camera.h"
#include "RenderObject.h"
#include "TextureManager.h"
#include "GraphicsSystem.h"

#include "BlendState.h"

using namespace IExeEngine;
using namespace IExeEngine::Graphics;

void HatchingEffect::Initialize(const std::filesystem::path& path)
{
    // Initialize constant buffers
    mTransformBuffer.Initialize();
    mLightBuffer.Initialize();
    mMaterialBuffer.Initialize();
    mSettingsBuffer.Initialize();
    mHatchingBuffer.Initialize();

    // Shaders
    mVertexShader.Initialize<Vertex>(path);
    mPixelShader.Initialize(path);

    // Sampler (same as other effects)
    mSampler.Initialize(Sampler::Filter::Linear, Sampler::AddressMode::Wrap);
}

void HatchingEffect::Terminate()
{
    mSampler.Terminate();
    mPixelShader.Terminate();
    mVertexShader.Terminate();

    mHatchingBuffer.Terminate();
    mSettingsBuffer.Terminate();
    mMaterialBuffer.Terminate();
    mLightBuffer.Terminate();
    mTransformBuffer.Terminate();
}

void HatchingEffect::Begin()
{
    mVertexShader.Bind();
    mPixelShader.Bind();

    mSampler.BindPS(0);
    mSampler.BindVS(0);

    // register b0..b4 bind points (match HLSL)
    mTransformBuffer.BindVS(0);

    mLightBuffer.BindVS(1);
    mLightBuffer.BindPS(1);

    mMaterialBuffer.BindPS(2);

    mSettingsBuffer.BindVS(3);
    mSettingsBuffer.BindPS(3);

    mHatchingBuffer.BindPS(4);
}

void HatchingEffect::End()
{

}

void HatchingEffect::Render(const RenderObject& renderObject)
{
    ASSERT(mCamera != nullptr, "HatchingEffect::Render - camera not set!");
    ASSERT(mDirectionalLight != nullptr, "HatchingEffect::Render - directional light not set!");

    // 1) Transform
    const Math::Matrix4 matWorld = renderObject.transform.GetMatrix4();
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

    // 2) Light
    if (mDirectionalLight)
        mLightBuffer.Update(*mDirectionalLight);

    // 3) Material
    mMaterialBuffer.Update(renderObject.material);

    // 4) Settings (mirror StandardEffect's logic)
    SettingsData settings;
    settings.useDiffuseMap = (renderObject.diffuseMapId > 0 && mSettingsData.useDiffuseMap > 0) ? 1 : 0;
    settings.useSpecMap = (renderObject.specMapId > 0 && mSettingsData.useSpecMap > 0) ? 1 : 0;
    settings.useNormalMap = (renderObject.normalMapId > 0 && mSettingsData.useNormalMap > 0) ? 1 : 0;
    settings.useBumpMap = (renderObject.bumpMapId > 0 && mSettingsData.useBumpMap > 0) ? 1 : 0;
    settings.bumpIntensity = mSettingsData.bumpIntensity;
    settings.useShadowMap = (mShadowMap != nullptr && mSettingsData.useShadowMap > 0) ? 1 : 0;
    settings.depthBias = mSettingsData.depthBias;
    mSettingsBuffer.Update(settings);

    // 5) Hatching params
    HatchingData hData;
    hData.cellCountX = static_cast<float>(mCellCountX);
    hData.cellCountY = static_cast<float>(mCellCountY);
    hData.opacity = mOpacity;
    hData.darknessCutoff = mDarknessCutoff;
    hData.lineThickness = mLineThickness;
    hData.angleA = mAngleA;
    hData.angleB = mAngleB;
    hData.padding = 0.0f;
    mHatchingBuffer.Update(hData);

    // 6) Bind textures
    TextureManager* tm = TextureManager::Get();
    tm->BindPS(renderObject.diffuseMapId, 0);
    tm->BindPS(renderObject.specMapId, 1);
    tm->BindPS(renderObject.normalMapId, 2);
    tm->BindVS(renderObject.bumpMapId, 3);

    // 7) Render
    renderObject.meshBuffer.Render();
}

void HatchingEffect::Render(const RenderGroup& renderGroup)
{
    ASSERT(mCamera != nullptr, "HatchingEffect::Render - camera not set!");
    ASSERT(mDirectionalLight != nullptr, "HatchingEffect::Render - directional light not set!");

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

    // Hatching params (update once)
    HatchingData hData;
    hData.cellCountX = static_cast<float>(mCellCountX);
    hData.cellCountY = static_cast<float>(mCellCountY);
    hData.opacity = mOpacity;
    hData.darknessCutoff = mDarknessCutoff;
    hData.lineThickness = mLineThickness;
    hData.angleA = mAngleA;
    hData.angleB = mAngleB;
    hData.padding = 0.0f;
    mHatchingBuffer.Update(hData);

    TextureManager* tm = TextureManager::Get();

    for (const RenderObject& renderObject : renderGroup.renderObjects)
    {
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

void HatchingEffect::DebugUI()
{
    if (ImGui::CollapsingHeader("HatchingEffect"))
    {
        ImGui::DragInt("Cells X", &mCellCountX, 1, 4, 2048);
        ImGui::DragInt("Cells Y", &mCellCountY, 1, 4, 2048);
        ImGui::DragFloat("Opacity", &mOpacity, 0.01f, 0.0f, 2.0f);
        ImGui::DragFloat("Darkness Cutoff", &mDarknessCutoff, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Line Thickness", &mLineThickness, 0.001f, 0.001f, 0.2f);
        ImGui::DragFloat("Angle A (deg)", &mAngleA, 0.01f, -3.14159f, 3.14159f);
        ImGui::DragFloat("Angle B (deg)", &mAngleB, 0.01f, -3.14159f, 3.14159f);

        //bool useDiffuse = mSettingsData.useDiffuseMap > 0;
        //if (ImGui::Checkbox("UseDiffuseMap##Hatch", &useDiffuse)) mSettingsData.useDiffuseMap = useDiffuse ? 1 : 0;

        //bool useNormal = mSettingsData.useNormalMap > 0;
        //if (ImGui::Checkbox("UseNormalMap##Hatch", &useNormal)) mSettingsData.useNormalMap = useNormal ? 1 : 0;

        //bool useBump = mSettingsData.useBumpMap > 0;
        //if (ImGui::Checkbox("UseBumpMap##Hatch", &useBump)) mSettingsData.useBumpMap = useBump ? 1 : 0;

        //ImGui::DragFloat("BumpIntensity##Hatch", &mSettingsData.bumpIntensity, 0.01f, -5.0f, 5.0f);
        //bool useShadow = mSettingsData.useShadowMap > 0;
        //if (ImGui::Checkbox("UseShadowMap##Hatch", &useShadow)) mSettingsData.useShadowMap = useShadow ? 1 : 0;
        //ImGui::DragFloat("DepthBias##Hatch", &mSettingsData.depthBias, 0.000001f, 0.0f, 1.0f, "%.6f");
    }
}
