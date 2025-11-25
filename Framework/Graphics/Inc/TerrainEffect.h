#pragma once

#include "ConstantBuffer.h"
#include "DirectionalLight.h"
#include "Material.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "Sampler.h"

namespace IExeEngine::Graphics
{
    class Camera;
    class Texture;
    class RenderObject;

    class TerrainEffect final
    {
    public:
        void Initialize();
        void Terminate();

        void Begin();
        void End();

        void Render(const RenderObject& renderObject);
        void DebugUI();

        void SetCamera(const Camera& camera);
        void SetLightCamera(const Camera& lightCamera);
        void SetDirectionalLight(const DirectionalLight& directionalLight);
        void SetShadowMap(const Texture& shadowMap);

    private:
        struct TransformData
        {
            Math::Matrix4 world;
            Math::Matrix4 wvp; // World-View-Projection matrix
            Math::Matrix4 lwvp; // Light World-View-Projection matrix
            Math::Vector3 viewPosition;
            float padding = 0.0f;
        };

        struct SettingsData
        {
            int useShadowMap = 1;
            float depthBias = 0.000003f;
            float lowHeight = 5.0f;
            float blendHeight = 1.0f;
        };

        using TransformBuffer = TypedConstantBuffer<TransformData>;
        using LightBuffer = TypedConstantBuffer<DirectionalLight>;
        using MaterialBuffer = TypedConstantBuffer<Material>;
        using SettingsBuffer = TypedConstantBuffer<SettingsData>;

        TransformBuffer mTransformBuffer;
        LightBuffer mLightBuffer;
        MaterialBuffer mMaterialBuffer;
        SettingsBuffer mSettingsBuffer;

        VertexShader mVertexShader;
        PixelShader mPixelShader;
        Sampler mSampler;

        SettingsData mSettingsData;
        const Camera* mCamera = nullptr;
        const Camera* mLightCamera = nullptr;
        const DirectionalLight* mDirectionalLight = nullptr;
        const Texture* mShadowMap = nullptr;
    };
}