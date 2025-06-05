#pragma once

#include <IExeEngine/Inc/IExeEngine.h>

class GameState : public IExeEngine::AppState
{
public:
	void Initialize() override;

	void Terminate() override;
	
	void Update(float deltaTime) override;

	void Render() override;

	void DebugUI() override;

private:
	struct Object
	{
		IExeEngine::Math::Matrix4 worldMat = IExeEngine::Math::Matrix4::Identity;
		IExeEngine::Graphics::MeshBuffer meshBuffer;
		IExeEngine::Graphics::TextureId textureId = 0;
	};

	void UpdateCamera(float deltaTime);

    void RenderObject(const Object& object, const IExeEngine::Graphics::Camera& camera);

	IExeEngine::Graphics::Camera mCamera;
	IExeEngine::Graphics::Camera mRenderTargetCamera;

	// GPU Communication
	IExeEngine::Graphics::ConstantBuffer mTransformBuffer;
	IExeEngine::Graphics::VertexShader mVertexShader;
	IExeEngine::Graphics::PixelShader mPixelShader;
	IExeEngine::Graphics::Sampler mSampler;

	// Render Object
	Object mSpace;
	Object mEarth;
	Object mSun;

	// Render Target
    IExeEngine::Graphics::RenderTarget mRenderTarget;
};