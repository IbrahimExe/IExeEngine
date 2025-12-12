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
            Math::Matrix4 wvp;   // register(b0) - matches StandardEffect
            Math::Matrix4 world;
            Math::Matrix4 lwvp;  // light world-view-proj for shadows
            Math::Vector3 viewPosition;
            float padding = 0.0f;
        };

        struct HalftoneData
        {
            float cellCountX;    // we'll bind this to a separate CB (register chosen below)
            float cellCountY;
            float opacity;       // 0..1
            float threshold;     // brightness threshold / edge softness
        };

        using TransformBuffer = TypedConstantBuffer<TransformData>;
        TransformBuffer mTransformBuffer;

        // Use same light buffer type as StandardEffect
        using LightBuffer = TypedConstantBuffer<DirectionalLight>;
        LightBuffer mLightBuffer;

        // Use material buffer if we want access to material fields (not strictly necessary but kept for parity)
        using MaterialBuffer = TypedConstantBuffer<Material>;
        MaterialBuffer mMaterialBuffer;

        // Halftone-specific params
        using HalftoneBuffer = TypedConstantBuffer<HalftoneData>;
        HalftoneBuffer mHalftoneBuffer;

        VertexShader mVertexShader;
        PixelShader mPixelShader;
        Sampler mSampler;

        const DirectionalLight* mDirectionalLight = nullptr;
        const Camera* mCamera = nullptr;
        const Camera* mLightCamera = nullptr;
        const Texture* mShadowMap = nullptr;

        int mCellCountX = 40;
        int mCellCountY = 40;
        float mOpacity = 1.0f;
        float mThreshold = 0.02f; // small soft-edge value

    };
}
