#pragma once

#include "ConstantBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "DirectionalLight.h"
#include "Camera.h"
#include "Sampler.h"
#include "Material.h"

namespace IExeEngine::Graphics
{
    class RenderObject;
    class RenderGroup;
    class Texture;

    class HatchingEffect
    {
    public:
        void Initialize(const std::filesystem::path& path = L"../../Assets/Shaders/Hatching.fx");
        void Terminate();

        void Begin();
        void End();

        void Render(const RenderObject& renderObject);
        void Render(const RenderGroup& renderGroup);

        void DebugUI();

        void SetDirectionalLight(const DirectionalLight& dl) { mDirectionalLight = &dl; }
        void SetCamera(const Camera& cam) { mCamera = &cam; }
        void SetLightCamera(const Camera& cam) { mLightCamera = &cam; }
        void SetShadowMap(const Texture& tex) { mShadowMap = &tex; }

    private:
        struct TransformData
        {
            Math::Matrix4 wvp;   // register(b0)
            Math::Matrix4 world;
            Math::Matrix4 lwvp;  // light world-view-proj for shadows
            Math::Vector3 viewPosition;
            float padding = 0.0f;
        };

        // same SettingsData layout as StandardEffect (register b3)
        struct SettingsData
        {
            int useDiffuseMap = 1;
            int useSpecMap = 1;
            int useNormalMap = 1;
            int useBumpMap = 1;
            int useShadowMap = 1;
            float bumpIntensity = -0.02f;
            float depthBias = 0.000003f;
            float padding = 0.0f;
        };

        // Hatching parameters (register b4) - keep 16 byte alignment
        struct HatchingData
        {
            float cellCountX;
            float cellCountY;
            float opacity;
            float darknessCutoff;

            float lineThickness;
            float angleA;
            float angleB;
            float padding;
        };

        using TransformBuffer = TypedConstantBuffer<TransformData>;
        TransformBuffer mTransformBuffer;

        using LightBuffer = TypedConstantBuffer<DirectionalLight>;
        LightBuffer mLightBuffer;

        using MaterialBuffer = TypedConstantBuffer<Material>;
        MaterialBuffer mMaterialBuffer;

        using SettingsBuffer = TypedConstantBuffer<SettingsData>;
        SettingsBuffer mSettingsBuffer;

        using HatchingBuffer = TypedConstantBuffer<HatchingData>;
        HatchingBuffer mHatchingBuffer;

        VertexShader mVertexShader;
        PixelShader mPixelShader;
        Sampler mSampler;

        const DirectionalLight* mDirectionalLight = nullptr;
        const Camera* mCamera = nullptr;
        const Camera* mLightCamera = nullptr;
        const Texture* mShadowMap = nullptr;

        // tweakables
        SettingsData mSettingsData;
        int mCellCountX = 30;
        int mCellCountY = 300;
        float mOpacity = 0.8f;
        float mDarknessCutoff = 0.12f;
        float mLineThickness = 0.08f;
        float mAngleA = 0.8f; // 45 degrees
        float mAngleB = -2.2f; // -45 degrees
    };
}
