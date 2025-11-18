#pragma once

#include "ConstantBuffer.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "DirectionalLight.h"
#include "Material.h"
#include "Sampler.h"

namespace IExeEngine::Graphics
{
    class Camera;
    class RenderObject;
    class RenderGroup;
    class Texture;

    class StandardEffect final
    {
    public:
        void Initialize(const std::filesystem::path& path);
        void Terminate();

        void Begin();
        void End();

        void Render(const RenderObject& renderObject);
        void Render(const RenderGroup& renderGroup);

        void SetCamera(const Camera& camera);

        void SetDirectionalLight(const DirectionalLight& directionalLight);
        // Shadows
        void SetLightCamera(const Camera& camera);
        void SetShadowMap(const Texture& shadowMap);

        void DebugUI();

    private:

        struct TransformData
        {
            Math::Matrix4 wvp; // World-View-Projection matrix
            Math::Matrix4 world; // World matrix
            Math::Matrix4 lwvp; // Light World-View-Projection matrix (World Proj of light objects for shadows)
            Math::Vector3 viewPosition; // Camera position in world space
            float padding = 0.0f; // Padding to maintain the 16 byte alignment
        };

        struct SettingsData
        {
            int useDiffuseMap = 1;
            int useSpecMap = 1;
            int useNormalMap = 1;
            int useBumpMap = 1;
            int useShadowMap = 1;
            float bumpIntensity = -0.02f;
            float depthBias = 0.000003f;
            float padding = 0.0f; // Padding to make the structure 16-byte aligned (After ShadowMap only need one)
        };

        using TransformBuffer = TypedConstantBuffer<TransformData>;
        TransformBuffer mTransformBuffer;

        using LightBuffer = TypedConstantBuffer<DirectionalLight>;
        LightBuffer mLightBuffer;

        using MaterialBuffer = TypedConstantBuffer<Material>;
        MaterialBuffer mMaterialBuffer;

        using SettingsBuffer = TypedConstantBuffer<SettingsData>;
        SettingsBuffer mSettingsBuffer;

        VertexShader mVertexShader;
        PixelShader mPixelShader;
        Sampler mSampler;

        SettingsData mSettingsData;

        const Camera* mCamera = nullptr;
        const DirectionalLight* mDirectionalLight = nullptr;

        const Camera* mLightCamera = nullptr;
        const Texture* mShadowMap = nullptr;
    };
}
