#include "GameState.h"

using namespace IExeEngine;
using namespace IExeEngine::Graphics;
using namespace IExeEngine::Input;
using namespace IExeEngine::Physics;


void GameState::Initialize()
{
	mCamera.SetPosition({ 2.0f, 2.0f, -2.0f });
	mCamera.SetLookAt({ 0.0f, 1.2f, 0.0f });

	mDirectionalLight.direction = Math::Normalize({ 1.0f, -1.0f, 1.0f });
    mDirectionalLight.ambient = { 0.4f, 0.4f, 0.4f, 1.0f };
    mDirectionalLight.diffuse = { 0.8f, 0.8f, 0.8f, 1.0f };
    mDirectionalLight.specular = { 0.9f, 0.9f, 0.9f, 1.0f };

    // DragonBorn - Free Spline Animation Software

    // Football
    Mesh football = MeshBuilder::CreateSphere(50, 50, 0.5f);
    mFootball.meshBuffer.Initialize(football);
    mFootball.transform.position.y = 5.0f;
    mBallShape.InitializeSphere(0.5f);
    mBallRigidBody.Initialize(mFootball.transform, mBallShape, 5.0f);

    TextureManager* tm_basket = TextureManager::Get();
	mFootball.diffuseMapId = tm_basket->LoadTexture(L"../../Assets/Textures/misc/Brazuca.jpg");

	// Ground
    Mesh plane = MeshBuilder::CreatePlane(20, 20, 1.0f, true);
    mGroundObject.meshBuffer.Initialize(plane);
    mGroundShape.InitializeHull({ 5.0f, 0.5f, 5.0f }, {0.0f, -0.5f, 0.0f});
    mGroundRigidBody.Initialize(mGroundObject.transform, mGroundShape, 0.0f);

    mGroundObject.diffuseMapId = tm_basket->LoadTexture(L"../../Assets/Textures/misc/concrete.jpg");

    std::filesystem::path shaderFile = L"../../Assets/Shaders/Standard.fx";
    mStandardEffect.Initialize(shaderFile);
    mStandardEffect.SetCamera(mCamera);
    mStandardEffect.SetDirectionalLight(mDirectionalLight);

    // Boxes
    Mesh boxShape = MeshBuilder::CreateCube(1.0f);
    TextureId boxTextureId = tm_basket->LoadTexture(L"../../Assets/Textures/misc/cardboard.jpg");

    float yOffset = 4.5f;
    float xOffset = 0.0f;
	int rowCount = 1;
	int boxIndex = 0;
	mBoxes.resize(10);
	while (boxIndex < mBoxes.size())
	{
        xOffset = -((static_cast<float>(rowCount) - 1.0f) * 0.5f);
		for (int r = 0; r < rowCount; ++r)
		{
            BoxData& box = mBoxes[boxIndex];
            box.box.meshBuffer.Initialize(boxShape);
            box.box.diffuseMapId = boxTextureId;
            box.box.transform.position.x = xOffset;
            box.box.transform.position.y = yOffset;
            box.box.transform.position.z = 4.0f;
            box.shape.InitializeBox({ 0.5f, 0.5f, 0.5f });
            xOffset += 1.0f;
            ++boxIndex;
		}
        yOffset -= 1.0f;
        rowCount += 1;
	}
    for (int i = mBoxes.size() - 1; i >= 0; --i)
    {
        mBoxes[i].rigidBody.Initialize(mBoxes[i].box.transform, mBoxes[i].shape, 1.0f);
    }

    // Cloth
    int rows = 20;
    int cols = 20;
    mClothMesh = MeshBuilder::CreatePlane(rows, cols, 0.5f);
    for (Graphics::Vertex& v : mClothMesh.vertices)
    {
        v.position.y += 10.0f;
        v.position.z += 10.0f;
    }

    uint32_t lastVertex = mClothMesh.vertices.size() - 1;
    uint32_t lastVertexOS = lastVertex - cols; // Other Side
    mClothSoftBody.Initialize(mClothMesh, 1.0f, { lastVertex, lastVertexOS });
    mCloth.meshBuffer.Initialize(nullptr, sizeof(Vertex), mClothMesh.vertices.size(),
        mClothMesh.indices.data(), mClothMesh.indices.size());
    mCloth.diffuseMapId = tm_basket->LoadTexture(L"../../Assets/Textures/misc/cloth.jpg");

    // Cloth Ball
    mClothBallMesh = MeshBuilder::CreateSphere(10, 10, 2.0f);
    for (Graphics::Vertex& v : mClothBallMesh.vertices)
    {
        v.position.y += 10.0f;
        //v.position.z += 10.0f;
    }
    mClothBallSoftBody.Initialize(mClothBallMesh, 1.0f, {});
    mClothBall.meshBuffer.Initialize(nullptr, sizeof(Vertex), mClothBallMesh.vertices.size(),
        mClothBallMesh.indices.data(), mClothBallMesh.indices.size());
    mClothBall.diffuseMapId = tm_basket->LoadTexture(L"../../Assets/Textures/misc/cardboard.jpg");
}

void GameState::Terminate()
{
    mClothBall.Terminate();
    mClothBallSoftBody.Terminate();

    mCloth.Terminate();
    mClothSoftBody.Terminate();

    for (BoxData& box : mBoxes)
    {
        box.rigidBody.Terminate();
        box.shape.Terminate();
        box.box.Terminate();
    }

	mStandardEffect.Terminate();

    mGroundRigidBody.Terminate();
    mGroundShape.Terminate();
    mGroundObject.Terminate();

    mBallRigidBody.Terminate();
    mBallShape.Terminate();
	mFootball.Terminate();
}

void GameState::Update(float deltaTime)
{
	UpdateCamera(deltaTime);

    if (InputSystem::Get()->IsKeyPressed(KeyCode::SPACE))
    {
        Math::Vector3 spawnPos = mCamera.GetPosition() + (mCamera.GetDirection() * 0.5f);
        mBallRigidBody.SetPosition(spawnPos);
        mBallRigidBody.SetVelocity(mCamera.GetDirection() * 50.0f);
    }
}

void GameState::Render()
{
    mCloth.meshBuffer.Update(mClothMesh.vertices.data(), mClothMesh.vertices.size());
    mClothBall.meshBuffer.Update(mClothBallMesh.vertices.data(), mClothBallMesh.vertices.size());
	SimpleDraw::AddGroundPlane(20.0f, Colors::Wheat);
	SimpleDraw::Render(mCamera);

    mStandardEffect.Begin();

	    mStandardEffect.Render(mFootball);
        mStandardEffect.Render(mGroundObject);
        mStandardEffect.Render(mCloth);
        mStandardEffect.Render(mClothBall);
        for (BoxData& box : mBoxes)
        {
            mStandardEffect.Render(box.box);
        }

    mStandardEffect.End();

}

void GameState::DebugUI()
{
	ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::DragFloat3("Direction#Light", &mDirectionalLight.direction.x, 0.01f))
		{
			mDirectionalLight.direction = Math::Normalize(mDirectionalLight.direction);
		}

		ImGui::ColorEdit4("Ambient#Light", &mDirectionalLight.ambient.r);
		ImGui::ColorEdit4("Diffuse#Light", &mDirectionalLight.diffuse.r);
		ImGui::ColorEdit4("Specular#Light", &mDirectionalLight.specular.r);
	}

	ImGui::Separator();

	Math::Vector3 pos = mFootball.transform.position;
    if (ImGui::DragFloat3("BallPosition", &pos.x))
    {
        mFootball.transform.position = pos;
        mBallRigidBody.SetPosition(mFootball.transform.position);
    }

	mStandardEffect.DebugUI();
    PhysicsWorld::Get()->DebugUI();
	ImGui::End();
    SimpleDraw::Render(mCamera);
}

void GameState::UpdateCamera(float deltaTime)
{
	// Camera Controls:
	InputSystem* input = InputSystem::Get();
	const float moveSpeed = input->IsKeyDown(KeyCode::LSHIFT) ? 10.0f : 4.0f;
	const float turnSpeed = 0.5f;

	if (input->IsKeyDown(KeyCode::W)) { mCamera.Walk(moveSpeed * deltaTime); }

	else if (input->IsKeyDown(KeyCode::S)) { mCamera.Walk(-moveSpeed * deltaTime); }

	else if (input->IsKeyDown(KeyCode::D)) { mCamera.Strafe(moveSpeed * deltaTime); }

	else if (input->IsKeyDown(KeyCode::A)) { mCamera.Strafe(-moveSpeed * deltaTime); }

	else if (input->IsKeyDown(KeyCode::E)) { mCamera.Rise(moveSpeed * deltaTime); }

	else if (input->IsKeyDown(KeyCode::Q)) { mCamera.Rise(-moveSpeed * deltaTime); }

	if (input->IsMouseDown(MouseButton::RBUTTON))
	{
		mCamera.Yaw(input->GetMouseMoveX() * turnSpeed * deltaTime);
		mCamera.Pitch(input->GetMouseMoveY() * turnSpeed * deltaTime);
	}
}