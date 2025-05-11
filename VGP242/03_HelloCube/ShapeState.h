#pragma once

#include <IExeEngine/Inc/IExeEngine.h>

class ShapeState : public IExeEngine::AppState
{
public:
	void Initialize() override;
	void Terminate() override;
	void Update(float deltaTime) override;
	void Render() override;

protected:
	virtual void CreateShape();

	using Vertices = std::vector<IExeEngine::Graphics::VertexPC>;
	Vertices mVertices;

	IExeEngine::Graphics::Camera mCamera;
	IExeEngine::Graphics::ConstantBuffer mTransformBuffer;
	IExeEngine::Graphics::MeshBuffer mMeshbuffer;
	IExeEngine::Graphics::VertexShader mVertexShader;
	IExeEngine::Graphics::PixelShader mPixelShader;
};