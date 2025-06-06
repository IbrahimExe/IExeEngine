#pragma once

#include"ConstantBuffer.h"
#include "Sampler.h"
#include "VertexShader.h"
#include "PixelShader.h"

namespace IExeEngine::Graphics
{
    class Camera;
    class MeshBuffer;

    class SimpleTextureEffect
    {
    public:
        void Initialize();
        void Terminate();

        void Begin();
        void End();

        void Render(const MeshBuffer& mesh, const Math::Matrix4& matWorld);

        void SetCamera(const Camera& camera);

    private:

}
