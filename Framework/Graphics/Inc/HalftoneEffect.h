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

    class HalftoneEffect
    {
    public:
        void Initialize(const std::filesystem::path& path = L"../../Assets/Shaders/Halftone.fx");
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
            Math::Matrix4 lwvp;  // Light World View Proj for shadows
            Math::Vector3 viewPosition;
            float padding = 0.0f;
        };

        // SettingsData (register b3)
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

        struct HalftoneData
        {
            float cellCountX;    // register(b4)
            float cellCountY;
            float opacity;       // 0..1
            float threshold;     // soft edge amount
        };

        using TransformBuffer = TypedConstantBuffer<TransformData>;
        TransformBuffer mTransformBuffer;

        using LightBuffer = TypedConstantBuffer<DirectionalLight>;
        LightBuffer mLightBuffer;

        using MaterialBuffer = TypedConstantBuffer<Material>;
        MaterialBuffer mMaterialBuffer;

        using SettingsBuffer = TypedConstantBuffer<SettingsData>;
        SettingsBuffer mSettingsBuffer;

        using HalftoneBuffer = TypedConstantBuffer<HalftoneData>;
        HalftoneBuffer mHalftoneBuffer;

        VertexShader mVertexShader;
        PixelShader mPixelShader;
        Sampler mSampler;

        // Pointers to external resources
        const DirectionalLight* mDirectionalLight = nullptr;
        const Camera* mCamera = nullptr;
        const Camera* mLightCamera = nullptr;
        const Texture* mShadowMap = nullptr;


        SettingsData mSettingsData;
        int mCellCountX = 40;
        int mCellCountY = 40;
        float mOpacity = 1.0f;
        float mThreshold = 0.02f; // small softness
    };
}