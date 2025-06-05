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
	void UpdateCamera(float deltaTime);

	IExeEngine::Graphics::Camera mCamera;

	// GPU Communication
	IExeEngine::Graphics::ConstantBuffer mTransformBuffer;
	IExeEngine::Graphics::VertexShader mVertexShader;
	IExeEngine::Graphics::PixelShader mPixelShader;
	IExeEngine::Graphics::Sampler mSampler;

	// Render Object
	IExeEngine::Math::Matrix4 mWorldMat = IExeEngine::Math::Matrix4::Identity;
	IExeEngine::Graphics::MeshBuffer mObject;
	IExeEngine::Graphics::TextureId mTextureId;
};