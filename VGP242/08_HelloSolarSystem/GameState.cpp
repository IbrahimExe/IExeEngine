#include "GameState.h"

using namespace IExeEngine;
using namespace IExeEngine::Graphics;
using namespace IExeEngine::Input;

enum class PlanetRenderTarget
{
	Sun,
	Mercury,
	Venus,
	Earth,
	Moon,
	Mars,
	Jupiter,
	Saturn,
	Uranus,
	Neptune,
	Pluto
};
PlanetRenderTarget gCurrentPlanet = PlanetRenderTarget::Sun;

const char* gPlanetNames[] = 
{
	"Sun", "Mercury", "Venus", "Earth", "Moon", "Mars",
	"Jupiter", "Saturn", "Uranus", "Neptune", "Pluto"
};

void GameState::Initialize()
{
	mCamera.SetPosition({ 0.0f, 5.0f, -10.0f });
	mCamera.SetLookAt({ 0.0f, 0.0f, 0.0f });

	mRenderTargetCamera.SetPosition({ 0.0f, 1.0f, -10.0f });
	mRenderTargetCamera.SetLookAt({ 0.0f, 0.0f, 0.0f });
	mRenderTargetCamera.SetAspectRatio(1.0f);

	// Initialize GPU Communication
	mSimpleTextureEffect.Initialize();

	// Initialize Render Objects
	// Space
	MeshPX spaceSphere = MeshBuilder::CreateSkySpherePX(60, 60, 500.0f);
	mSpace.mesh.Initialize(spaceSphere);
	mSpace.textureId = TextureManager::Get()->LoadTexture(L"darkerSpace.jpg");

	// Sun
	MeshPX sunSphere = MeshBuilder::CreateSpherePX(60, 60, 5.0f);
	mSun.mesh.Initialize(sunSphere);
	mSun.textureId = TextureManager::Get()->LoadTexture(L"sun.jpg");

	// Mercury
	mMercury.radius = 0.3f;
	mMercury.centerObject = mSun.matWorld;
	mMercury.distanceFromCenter = 8.0f;
	mMercury.orbitTranslation = 1.0f;
	mMercury.rotationOnAxis = 1.0f;
	MeshPX mercurySphere = MeshBuilder::CreateSpherePX(60, 60, mMercury.radius);
	mMercury.renderData.mesh.Initialize(mercurySphere);
	mMercury.renderData.textureId = TextureManager::Get()->LoadTexture(L"planets/mercury.jpg");
	mMercury.renderData.matWorld = Math::Matrix4::Translation(mMercury.distanceFromCenter, 0, 0);

	// Venus
	mVenus.radius = 0.5f;
	mVenus.centerObject = mSun.matWorld;
	mVenus.distanceFromCenter = 10.0f;
	mVenus.orbitTranslation = 0.9f;
	mVenus.rotationOnAxis = 1.0f;
	MeshPX venusSphere = MeshBuilder::CreateSpherePX(60, 60, mVenus.radius);
	mVenus.renderData.mesh.Initialize(venusSphere);
	mVenus.renderData.textureId = TextureManager::Get()->LoadTexture(L"planets/venus.jpg");
	mVenus.renderData.matWorld = Math::Matrix4::Translation(mVenus.distanceFromCenter, 0, 0);

	// Earth
	mEarth.radius = 0.5f;
	mEarth.centerObject = mSun.matWorld;
	mEarth.distanceFromCenter = 12.0f;
	mEarth.orbitTranslation = 0.8f;
	mEarth.rotationOnAxis = 1.0f;
    MeshPX earthSphere = MeshBuilder::CreateSpherePX(60, 60, mEarth.radius);
    // MeshPC earthSphere = MeshBuilder::CreateCylinderPC(2, 5); Uncomment if you're a flat earther
	mEarth.renderData.mesh.Initialize(earthSphere);
	mEarth.renderData.textureId = TextureManager::Get()->LoadTexture(L"planets/earth.jpg");
	mEarth.renderData.matWorld = Math::Matrix4::Translation(mEarth.distanceFromCenter, 0, 0);

	// Moon
	mMoon.radius = 0.1f;
	mMoon.centerObject = mEarth.renderData.matWorld;
	mMoon.distanceFromCenter = 0.75f;
	mMoon.orbitTranslation = 0.2f;
	mMoon.rotationOnAxis = 1.0f;
	MeshPX moonSphere = MeshBuilder::CreateSpherePX(60, 60, mMoon.radius);
	mMoon.renderData.mesh.Initialize(moonSphere);
	mMoon.renderData.textureId = TextureManager::Get()->LoadTexture(L"planets/'moon'.jpg");
	mMoon.renderData.matWorld = Math::Matrix4::Translation(mEarth.distanceFromCenter + mMoon.distanceFromCenter, 0, 0);

	// Mars
	mMars.radius = 0.5f;
	mMars.centerObject = mSun.matWorld;
	mMars.distanceFromCenter = 14.0f;
	mMars.orbitTranslation = 0.7f;
	mMars.rotationOnAxis = 1.0f;
	MeshPX marsSphere = MeshBuilder::CreateSpherePX(60, 60, mMars.radius);
	mMars.renderData.mesh.Initialize(marsSphere);
	mMars.renderData.textureId = TextureManager::Get()->LoadTexture(L"planets/mars.jpg");
	mMars.renderData.matWorld = Math::Matrix4::Translation(mMars.distanceFromCenter, 0, 0);

	// Jupiter
	mJupiter.radius = 2.0f;
	mJupiter.centerObject = mSun.matWorld;
	mJupiter.distanceFromCenter = 17.0f;
	mJupiter.orbitTranslation = 0.6f;
	mJupiter.rotationOnAxis = 1.0f;
	MeshPX jupiterSphere = MeshBuilder::CreateSpherePX(60, 60, mJupiter.radius);
	mJupiter.renderData.mesh.Initialize(jupiterSphere);
	mJupiter.renderData.textureId = TextureManager::Get()->LoadTexture(L"planets/jupiter.jpg");
	mJupiter.renderData.matWorld = Math::Matrix4::Translation(mJupiter.distanceFromCenter, 0, 0);

	// Saturn
	mSaturn.radius = 1.75f;
	mSaturn.centerObject = mSun.matWorld;
	mSaturn.distanceFromCenter = 24.0f;
	mSaturn.orbitTranslation = 0.5f;
	mSaturn.rotationOnAxis = 1.0f;
	MeshPX saturnObj = MeshBuilder::CreateOBJPX(L"../../Assets/Models/Planets/Saturn/Saturn.obj", 1.0f);
	mSaturn.renderData.mesh.Initialize(saturnObj);
	mSaturn.renderData.textureId = TextureManager::Get()->LoadTexture(L"planets/saturn.jpg");
	mSaturn.renderData.matWorld = Math::Matrix4::Translation(mSaturn.distanceFromCenter, 0, 0);

	// Uranus
	mUranus.radius = 0.7f;
	mUranus.centerObject = mSun.matWorld;
	mUranus.distanceFromCenter = 29.0f;
	mUranus.orbitTranslation = 0.4f;
	mUranus.rotationOnAxis = 1.0f;
	MeshPX uranusSphere = MeshBuilder::CreateSpherePX(60, 60, mUranus.radius);
	mUranus.renderData.mesh.Initialize(uranusSphere);
	mUranus.renderData.textureId = TextureManager::Get()->LoadTexture(L"planets/uranus.jpg");
	mUranus.renderData.matWorld = Math::Matrix4::Translation(mUranus.distanceFromCenter, 0, 0);

	// Neptune
	mNeptune.radius = 0.7f;
	mNeptune.centerObject = mSun.matWorld;
	mNeptune.distanceFromCenter = 32.0f;
	mNeptune.orbitTranslation = 0.3f;
	mNeptune.rotationOnAxis = 1.0f;
	MeshPX neptuneSphere = MeshBuilder::CreateSpherePX(60, 60, mNeptune.radius);
	mNeptune.renderData.mesh.Initialize(neptuneSphere);
	mNeptune.renderData.textureId = TextureManager::Get()->LoadTexture(L"planets/neptune.jpg");
	mNeptune.renderData.matWorld = Math::Matrix4::Translation(mNeptune.distanceFromCenter, 0, 0);

	// Pluto
	mPluto.radius = 0.2f;
	mPluto.centerObject = mSun.matWorld;
	mPluto.distanceFromCenter = 36.0f;
	mPluto.orbitTranslation = 0.2f;
	mPluto.rotationOnAxis = 1.0f;
	MeshPX plutoSphere = MeshBuilder::CreateSpherePX(60, 60, mPluto.radius);
	mPluto.renderData.mesh.Initialize(plutoSphere);
	mPluto.renderData.textureId = TextureManager::Get()->LoadTexture(L"planets/pluto.jpg");
	mPluto.renderData.matWorld = Math::Matrix4::Translation(mPluto.distanceFromCenter, 0, 0);

	// Asteroids
    mAsteroid_01.centerObject = mSun.matWorld;
    mAsteroid_01.distanceFromCenter = 36.0f; 
	MeshPX asteroidObj_01 = MeshBuilder::CreateOBJPX(L"../../Assets/Models/Asteroid/Asteroid.obj", 0.0078f);
    mAsteroid_01.renderData.mesh.Initialize(asteroidObj_01);
    mAsteroid_01.renderData.textureId = TextureManager::Get()->LoadTexture(L"Asteroid/Asteroid.jpg");
    mAsteroid_01.renderData.matWorld = Math::Matrix4::Translation(mAsteroid_01.distanceFromCenter, -30.54, -6.68);

	mAsteroid_02.centerObject = mSun.matWorld;
	mAsteroid_02.distanceFromCenter = -22.0f;
	MeshPX asteroidObj_02 = MeshBuilder::CreateOBJPX(L"../../Assets/Models/Asteroid/Asteroid.obj", 0.0098f);
	mAsteroid_02.renderData.mesh.Initialize(asteroidObj_02);
	mAsteroid_02.renderData.textureId = TextureManager::Get()->LoadTexture(L"Asteroid/Asteroid.jpg");
	mAsteroid_02.renderData.matWorld = Math::Matrix4::Translation(mAsteroid_02.distanceFromCenter, 90.42, 64.56);

	mAsteroid_03.centerObject = mSun.matWorld;
	mAsteroid_03.distanceFromCenter = 26.0f;
	MeshPX asteroidObj_03 = MeshBuilder::CreateOBJPX(L"../../Assets/Models/Asteroid/Asteroid.obj", 0.012f);
	mAsteroid_03.renderData.mesh.Initialize(asteroidObj_03);
	mAsteroid_03.renderData.textureId = TextureManager::Get()->LoadTexture(L"Asteroid/Asteroid.jpg");
	mAsteroid_03.renderData.matWorld = Math::Matrix4::Translation(mAsteroid_03.distanceFromCenter, 110.08, -142.66);

	mAsteroid_04.centerObject = mSun.matWorld;
	mAsteroid_04.distanceFromCenter = -38.0f;
	MeshPX asteroidObj_04 = MeshBuilder::CreateOBJPX(L"../../Assets/Models/Asteroid/Asteroid.obj", 0.0048f);
	mAsteroid_04.renderData.mesh.Initialize(asteroidObj_04);
	mAsteroid_04.renderData.textureId = TextureManager::Get()->LoadTexture(L"Asteroid/Asteroid.jpg");
	mAsteroid_04.renderData.matWorld = Math::Matrix4::Translation(mAsteroid_04.distanceFromCenter, -66.98, 122.51);

	mAsteroid_05.centerObject = mSun.matWorld;
	mAsteroid_05.distanceFromCenter = -38.0f;
	MeshPX asteroidObj_05 = MeshBuilder::CreateOBJPX(L"../../Assets/Models/Asteroid/Asteroid.obj", 0.0078f);
	mAsteroid_05.renderData.mesh.Initialize(asteroidObj_05);
	mAsteroid_05.renderData.textureId = TextureManager::Get()->LoadTexture(L"Asteroid/Asteroid.jpg");
	mAsteroid_05.renderData.matWorld = Math::Matrix4::Translation(mAsteroid_05.distanceFromCenter, 30.54, 6.68);

	mAsteroid_06.centerObject = mSun.matWorld;
	mAsteroid_06.distanceFromCenter = 22.0f;
	MeshPX asteroidObj_06 = MeshBuilder::CreateOBJPX(L"../../Assets/Models/Asteroid/Asteroid.obj", 0.0098f);
	mAsteroid_06.renderData.mesh.Initialize(asteroidObj_06);
	mAsteroid_06.renderData.textureId = TextureManager::Get()->LoadTexture(L"Asteroid/Asteroid.jpg");
	mAsteroid_06.renderData.matWorld = Math::Matrix4::Translation(mAsteroid_06.distanceFromCenter, 90.42, 64.56);

	mAsteroid_07.centerObject = mSun.matWorld;
	mAsteroid_07.distanceFromCenter = -46.0f;
	MeshPX asteroidObj_07 = MeshBuilder::CreateOBJPX(L"../../Assets/Models/Asteroid/Asteroid.obj", 0.012f);
	mAsteroid_07.renderData.mesh.Initialize(asteroidObj_07);
	mAsteroid_07.renderData.textureId = TextureManager::Get()->LoadTexture(L"Asteroid/Asteroid.jpg");
	mAsteroid_07.renderData.matWorld = Math::Matrix4::Translation(mAsteroid_07.distanceFromCenter, 110.08, -142.66);

	mAsteroid_08.centerObject = mSun.matWorld;
	mAsteroid_08.distanceFromCenter = 38.0f;
	MeshPX asteroidObj_08 = MeshBuilder::CreateOBJPX(L"../../Assets/Models/Asteroid/Asteroid.obj", 0.042f);
	mAsteroid_08.renderData.mesh.Initialize(asteroidObj_08);
	mAsteroid_08.renderData.textureId = TextureManager::Get()->LoadTexture(L"Asteroid/Asteroid.jpg");
	mAsteroid_08.renderData.matWorld = Math::Matrix4::Translation(mAsteroid_08.distanceFromCenter, -90.08, 22.80);

	mAsteroid_09.centerObject = mSun.matWorld;
	mAsteroid_09.distanceFromCenter = -80.0f;
	MeshPX asteroidObj_09 = MeshBuilder::CreateOBJPX(L"../../Assets/Models/Asteroid/Asteroid.obj", 0.0078f);
	mAsteroid_09.renderData.mesh.Initialize(asteroidObj_09);
	mAsteroid_09.renderData.textureId = TextureManager::Get()->LoadTexture(L"Asteroid/Asteroid.jpg");
	mAsteroid_09.renderData.matWorld = Math::Matrix4::Translation(mAsteroid_09.distanceFromCenter, 26.98, 22.0);

	mAsteroid_10.centerObject = mSun.matWorld;
	mAsteroid_10.distanceFromCenter = 100.0f;
	MeshPX asteroidObj_10 = MeshBuilder::CreateOBJPX(L"../../Assets/Models/Asteroid/Asteroid.obj", 0.01f);
	mAsteroid_10.renderData.mesh.Initialize(asteroidObj_10);
	mAsteroid_10.renderData.textureId = TextureManager::Get()->LoadTexture(L"Asteroid/Asteroid.jpg");
	mAsteroid_10.renderData.matWorld = Math::Matrix4::Translation(mAsteroid_10.distanceFromCenter, -82.0, -72.22);

	mAsteroid_11.centerObject = mSun.matWorld;
	mAsteroid_11.distanceFromCenter = -168.0f;
	MeshPX asteroidObj_11 = MeshBuilder::CreateOBJPX(L"../../Assets/Models/Asteroid/Asteroid.obj", 0.0048f);
	mAsteroid_11.renderData.mesh.Initialize(asteroidObj_11);
	mAsteroid_11.renderData.textureId = TextureManager::Get()->LoadTexture(L"Asteroid/Asteroid.jpg");
	mAsteroid_11.renderData.matWorld = Math::Matrix4::Translation(mAsteroid_11.distanceFromCenter, -66.98, 122.51);

	constexpr uint32_t size = 512;
	mRenderTarget.Initialize(size, size, RenderTarget::Format::RGBA_U32);
}

void GameState::Terminate()
{
	mRenderTarget.Terminate();
	TextureManager::Get()->ReleaseTexture(mSpace.textureId);
	TextureManager::Get()->ReleaseTexture(mSun.textureId);
	TextureManager::Get()->ReleaseTexture(mMercury.renderData.textureId);
	TextureManager::Get()->ReleaseTexture(mVenus.renderData.textureId);
	TextureManager::Get()->ReleaseTexture(mEarth.renderData.textureId);
	TextureManager::Get()->ReleaseTexture(mMoon.renderData.textureId);
	TextureManager::Get()->ReleaseTexture(mMars.renderData.textureId);
	TextureManager::Get()->ReleaseTexture(mJupiter.renderData.textureId);
	TextureManager::Get()->ReleaseTexture(mSaturn.renderData.textureId);
	TextureManager::Get()->ReleaseTexture(mUranus.renderData.textureId);
	TextureManager::Get()->ReleaseTexture(mNeptune.renderData.textureId);
	TextureManager::Get()->ReleaseTexture(mPluto.renderData.textureId);
    TextureManager::Get()->ReleaseTexture(mAsteroid_01.renderData.textureId);
    TextureManager::Get()->ReleaseTexture(mAsteroid_02.renderData.textureId);
	TextureManager::Get()->ReleaseTexture(mAsteroid_03.renderData.textureId);
	TextureManager::Get()->ReleaseTexture(mAsteroid_04.renderData.textureId);
	TextureManager::Get()->ReleaseTexture(mAsteroid_05.renderData.textureId);
	TextureManager::Get()->ReleaseTexture(mAsteroid_06.renderData.textureId);
	TextureManager::Get()->ReleaseTexture(mAsteroid_07.renderData.textureId);
	TextureManager::Get()->ReleaseTexture(mAsteroid_08.renderData.textureId);
	TextureManager::Get()->ReleaseTexture(mAsteroid_09.renderData.textureId);
	TextureManager::Get()->ReleaseTexture(mAsteroid_10.renderData.textureId);
	TextureManager::Get()->ReleaseTexture(mAsteroid_11.renderData.textureId);


	mSpace.mesh.Terminate();
	mSun.mesh.Terminate();
	mMercury.renderData.mesh.Terminate();
	mVenus.renderData.mesh.Terminate();
	mEarth.renderData.mesh.Terminate();
	mMoon.renderData.mesh.Terminate();
	mMars.renderData.mesh.Terminate();
	mJupiter.renderData.mesh.Terminate();
	mSaturn.renderData.mesh.Terminate();
	mUranus.renderData.mesh.Terminate();
	mNeptune.renderData.mesh.Terminate();
	mPluto.renderData.mesh.Terminate();

    mAsteroid_01.renderData.mesh.Terminate();
    mAsteroid_02.renderData.mesh.Terminate();
	mAsteroid_03.renderData.mesh.Terminate();
	mAsteroid_04.renderData.mesh.Terminate();
	mAsteroid_05.renderData.mesh.Terminate();
	mAsteroid_06.renderData.mesh.Terminate();
	mAsteroid_07.renderData.mesh.Terminate();
	mAsteroid_08.renderData.mesh.Terminate();
	mAsteroid_09.renderData.mesh.Terminate();
	mAsteroid_10.renderData.mesh.Terminate();
	mAsteroid_11.renderData.mesh.Terminate();

	mSimpleTextureEffect.Terminate();
}

Math::Vector3 GetObjectPosition(Math::Matrix4& centerObject)
{
	return Math::Vector3(
		centerObject._41, // ( X, 
		centerObject._42, //   Y, 
		centerObject._43  //   Z ) 
	);
}

void GameState::Update(float deltaTime)
{
	UpdateCamera(deltaTime);

	UpdatePlanet(mMercury, deltaTime);
	UpdatePlanet(mVenus, deltaTime);
	UpdatePlanet(mEarth, deltaTime);
	UpdateMoon(mMoon, mEarth, deltaTime);
	UpdatePlanet(mMars, deltaTime);
	UpdatePlanet(mJupiter, deltaTime);
	UpdatePlanet(mSaturn, deltaTime);
	UpdatePlanet(mUranus, deltaTime);
	UpdatePlanet(mNeptune, deltaTime);
	UpdatePlanet(mPluto, deltaTime);

    // Update Render Target Camera to follow the selected planet using ImGui combo box selection
	if (gCurrentPlanet == PlanetRenderTarget::Sun)
	{
		UpdateRenderTargetCamera(deltaTime, Math::Vector3::Zero, 11);
	}
	else if (gCurrentPlanet == PlanetRenderTarget::Mercury)
	{
		UpdateRenderTargetCamera(deltaTime, GetObjectPosition
		(mMercury.renderData.matWorld), mMercury.radius + 1.0f);
	}
	else if (gCurrentPlanet == PlanetRenderTarget::Venus)
	{
		UpdateRenderTargetCamera(deltaTime, GetObjectPosition
		(mVenus.renderData.matWorld), mVenus.radius + 1.0f);
	}
	else if (gCurrentPlanet == PlanetRenderTarget::Earth)
	{
		UpdateRenderTargetCamera(deltaTime, GetObjectPosition
		(mEarth.renderData.matWorld), mEarth.radius + 1.0f);
	}
	else if (gCurrentPlanet == PlanetRenderTarget::Moon)
	{
		UpdateRenderTargetCamera(deltaTime, GetObjectPosition
		(mMoon.renderData.matWorld), mMoon.radius + 1.0f);
	}
	else if (gCurrentPlanet == PlanetRenderTarget::Mars)
	{
		UpdateRenderTargetCamera(deltaTime, GetObjectPosition
		(mMars.renderData.matWorld), mMars.radius + 1.0f);
	}
	else if (gCurrentPlanet == PlanetRenderTarget::Jupiter)
	{
		UpdateRenderTargetCamera(deltaTime, GetObjectPosition
		(mJupiter.renderData.matWorld), mJupiter.radius + 2.0f);
	}
	else if (gCurrentPlanet == PlanetRenderTarget::Saturn)
	{
		UpdateRenderTargetCamera(deltaTime, GetObjectPosition
		(mSaturn.renderData.matWorld), mSaturn.radius + 2.0f);
	}
	else if (gCurrentPlanet == PlanetRenderTarget::Uranus)
	{
		UpdateRenderTargetCamera(deltaTime, GetObjectPosition
		(mUranus.renderData.matWorld), mUranus.radius + 1.0f);
	}
	else if (gCurrentPlanet == PlanetRenderTarget::Neptune)
	{
		UpdateRenderTargetCamera(deltaTime, GetObjectPosition
		(mNeptune.renderData.matWorld), mNeptune.radius + 1.0f);
	}
	else if (gCurrentPlanet == PlanetRenderTarget::Pluto)
	{
		UpdateRenderTargetCamera(deltaTime, GetObjectPosition
		(mPluto.renderData.matWorld), mPluto.radius + 1.0f);
	}
}

void GameState::Render()
{
	SimpleDraw::Render(mCamera);

    // Render to Render Target ImGui Image
	mSimpleTextureEffect.SetCamera(mRenderTargetCamera);
	mRenderTarget.BeginRender();
	mSimpleTextureEffect.Begin();
	if (gCurrentPlanet == PlanetRenderTarget::Sun)
	{
		mSimpleTextureEffect.Render(mSun);
	}
	else if (gCurrentPlanet == PlanetRenderTarget::Mercury)
	{
		mSimpleTextureEffect.Render(mMercury.renderData);
	}
	else if (gCurrentPlanet == PlanetRenderTarget::Venus)
	{
		mSimpleTextureEffect.Render(mVenus.renderData);
	}
	else if (gCurrentPlanet == PlanetRenderTarget::Earth)
	{
		mSimpleTextureEffect.Render(mEarth.renderData);
	}
	else if (gCurrentPlanet == PlanetRenderTarget::Moon)
	{
		mSimpleTextureEffect.Render(mMoon.renderData);
	}
	else if (gCurrentPlanet == PlanetRenderTarget::Mars)
	{
		mSimpleTextureEffect.Render(mMars.renderData);
	}
	else if (gCurrentPlanet == PlanetRenderTarget::Jupiter)
	{
		mSimpleTextureEffect.Render(mJupiter.renderData);
	}
	else if (gCurrentPlanet == PlanetRenderTarget::Saturn)
	{
		mSimpleTextureEffect.Render(mSaturn.renderData);
	}
	else if (gCurrentPlanet == PlanetRenderTarget::Uranus)
	{
		mSimpleTextureEffect.Render(mUranus.renderData);
	}
	else if (gCurrentPlanet == PlanetRenderTarget::Neptune)
	{
		mSimpleTextureEffect.Render(mNeptune.renderData);
	}
	else if (gCurrentPlanet == PlanetRenderTarget::Pluto)
	{
		mSimpleTextureEffect.Render(mPluto.renderData);
	}
	mSimpleTextureEffect.End();
	mRenderTarget.EndRender();

	// Render to Scene
	mSimpleTextureEffect.SetCamera(mCamera);
	mSimpleTextureEffect.Begin();

	mSimpleTextureEffect.Render(mSpace);
	mSimpleTextureEffect.Render(mSun);
	mSimpleTextureEffect.Render(mMercury.renderData);
	mSimpleTextureEffect.Render(mVenus.renderData);
	mSimpleTextureEffect.Render(mEarth.renderData);
	mSimpleTextureEffect.Render(mMoon.renderData);
	mSimpleTextureEffect.Render(mMars.renderData);
	mSimpleTextureEffect.Render(mJupiter.renderData);
	mSimpleTextureEffect.Render(mSaturn.renderData);
	mSimpleTextureEffect.Render(mUranus.renderData);
	mSimpleTextureEffect.Render(mNeptune.renderData);
	mSimpleTextureEffect.Render(mPluto.renderData);

    mSimpleTextureEffect.Render(mAsteroid_01.renderData);
    mSimpleTextureEffect.Render(mAsteroid_02.renderData);
	mSimpleTextureEffect.Render(mAsteroid_03.renderData);
	mSimpleTextureEffect.Render(mAsteroid_04.renderData);
	mSimpleTextureEffect.Render(mAsteroid_05.renderData);
	mSimpleTextureEffect.Render(mAsteroid_06.renderData);
	mSimpleTextureEffect.Render(mAsteroid_07.renderData);
	mSimpleTextureEffect.Render(mAsteroid_08.renderData);
	mSimpleTextureEffect.Render(mAsteroid_09.renderData);
	mSimpleTextureEffect.Render(mAsteroid_10.renderData);
	mSimpleTextureEffect.Render(mAsteroid_11.renderData);

	mSimpleTextureEffect.End();
}

void GameState::DebugUI()
{
	ImGui::Begin("ImGui Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Solar System Settings:");

	ImGui::Checkbox("Show Orbit Rings", &orbitRings);
	if (orbitRings)
	{
		SimpleDraw::AddGroundCircle(60, mMercury.distanceFromCenter, Colors::White, Math::Vector3::Zero);
		SimpleDraw::AddGroundCircle(60, mVenus.distanceFromCenter, Colors::White, Math::Vector3::Zero);
		SimpleDraw::AddGroundCircle(60, mEarth.distanceFromCenter, Colors::White, Math::Vector3::Zero);
		SimpleDraw::AddGroundCircle(60, mMars.distanceFromCenter, Colors::White, Math::Vector3::Zero);
		SimpleDraw::AddGroundCircle(60, mJupiter.distanceFromCenter, Colors::White, Math::Vector3::Zero);
		SimpleDraw::AddGroundCircle(60, mSaturn.distanceFromCenter, Colors::White, Math::Vector3::Zero);
		SimpleDraw::AddGroundCircle(60, mUranus.distanceFromCenter, Colors::White, Math::Vector3::Zero);
		SimpleDraw::AddGroundCircle(60, mNeptune.distanceFromCenter, Colors::White, Math::Vector3::Zero);
		SimpleDraw::AddGroundCircle(60, mPluto.distanceFromCenter, Colors::White, Math::Vector3::Zero);
	}

	ImGui::Text("Choose Planet to Render");
	int currentPlanet = (int)gCurrentPlanet;
	if (ImGui::Combo("Planet", &currentPlanet, gPlanetNames, std::size(gPlanetNames)))
	{
		gCurrentPlanet = static_cast<PlanetRenderTarget>(currentPlanet);
	}

	switch (gCurrentPlanet)
	{
	case PlanetRenderTarget::Sun:
		ImGui::Text("Sun");

		break;
	case PlanetRenderTarget::Mercury:
		ImGui::Text("Mercury");
		ImGui::DragFloat("Orbit Translation Speed", &mMercury.orbitTranslation, 0.01f, 0.0f, 10.0f);
		break;
	case PlanetRenderTarget::Venus:
		ImGui::Text("Venus");
		ImGui::DragFloat("Orbit Translation Speed", &mVenus.orbitTranslation, 0.01f, 0.0f, 10.0f);
		break;
	case PlanetRenderTarget::Earth:
		ImGui::Text("Earth");
		ImGui::DragFloat("Orbit Translation Speed", &mEarth.orbitTranslation, 0.01f, 0.0f, 10.0f);
		break;
	case PlanetRenderTarget::Moon:
		ImGui::Text("Moon");
		ImGui::DragFloat("Orbit Translation Speed", &mMoon.orbitTranslation, 0.01f, 0.0f, 10.0f);
		break;
	case PlanetRenderTarget::Mars:
		ImGui::Text("Mars");
		ImGui::DragFloat("Orbit Translation Speed", &mMars.orbitTranslation, 0.01f, 0.0f, 10.0f);
		break;
	case PlanetRenderTarget::Jupiter:
		ImGui::Text("Jupiter");
		ImGui::DragFloat("Orbit Translation Speed", &mJupiter.orbitTranslation, 0.01f, 0.0f, 10.0f);
		break;
	case PlanetRenderTarget::Saturn:
		ImGui::Text("Saturn");
		ImGui::DragFloat("Orbit Translation Speed", &mSaturn.orbitTranslation, 0.01f, 0.0f, 10.0f);
		break;
	case PlanetRenderTarget::Uranus:
		ImGui::Text("Uranus");
		ImGui::DragFloat("Orbit Translation Speed", &mUranus.orbitTranslation, 0.01f, 0.0f, 10.0f);
		break;
	case PlanetRenderTarget::Neptune:
		ImGui::Text("Neptune");
		ImGui::DragFloat("Orbit Translation Speed", &mNeptune.orbitTranslation, 0.01f, 0.0f, 10.0f);
		break;
	case PlanetRenderTarget::Pluto:
		ImGui::Text("Pluto");
		ImGui::DragFloat("Orbit Translation Speed", &mPluto.orbitTranslation, 0.01f, 0.0f, 10.0f);
		break;
	default:
		break;
	}

	ImGui::Separator();
	ImGui::Text("Render Target Camera");
	ImGui::Image(
		mRenderTarget.GetRawData(),
		{ 512, 512 },
		{ 0, 0 },
		{ 1, 1 },
		{ 1, 1, 1, 1 },
		{ 1, 1, 1, 1 });

	ImGui::End();
}

void GameState::UpdateCamera(float deltaTime)
{
	Input::InputSystem* input = Input::InputSystem::Get();
	const float moveSpeed = input->IsKeyDown(Input::KeyCode::LSHIFT) ? 20.0f : 2.0f;
	const float turnSpeed = 1.0f;

	if (input->IsKeyDown(Input::KeyCode::W)) mCamera.Walk(moveSpeed * deltaTime);
	else if (input->IsKeyDown(Input::KeyCode::S)) mCamera.Walk(-moveSpeed * deltaTime);

	if (input->IsKeyDown(Input::KeyCode::A)) mCamera.Strafe(-moveSpeed * deltaTime);
	else if (input->IsKeyDown(Input::KeyCode::D)) mCamera.Strafe(moveSpeed * deltaTime);

	if (input->IsKeyDown(Input::KeyCode::Q)) mCamera.Rise(-moveSpeed * deltaTime);
	else if (input->IsKeyDown(Input::KeyCode::E)) mCamera.Rise(moveSpeed * deltaTime);

	if (input->IsMouseDown(MouseButton::RBUTTON))
	{
		mCamera.Yaw(input->GetMouseMoveX() * turnSpeed * deltaTime);
		mCamera.Pitch(input->GetMouseMoveY() * turnSpeed * deltaTime);
	}
}

//-------------------------------------- Functions to Animate Planets & Moon: ------------------------------------------------------------------

void GameState::UpdatePlanet(Planet& planet, float deltaTime)
{
    // Update orbit and self-rotation angles
	planet.orbitAngle += planet.rotationOnAxis * deltaTime;
	if (planet.orbitAngle >= Math::Constants::TwoPi)
		planet.orbitAngle -= Math::Constants::TwoPi;

    // Update self-rotation angle
	planet.selfRotationAngle += planet.orbitTranslation * deltaTime;
	if (planet.selfRotationAngle >= Math::Constants::TwoPi)
		planet.selfRotationAngle -= Math::Constants::TwoPi;

    // Calculate the transformation matrices
	const auto orbitRotation = Math::Matrix4::RotationY(planet.orbitAngle);
	const auto orbitTranslation = Math::Matrix4::Translation(planet.distanceFromCenter, 0.0f, 0.0f);
	const auto orbitTransform = orbitRotation * orbitTranslation;

	const auto selfRotation = Math::Matrix4::RotationY(planet.selfRotationAngle);

    // Final transformation: rotate on self -> orbit around the Sun -> use Sun's position only
	planet.renderData.matWorld = orbitTransform * planet.centerObject * selfRotation;
}

void GameState::UpdateMoon(Planet& moon, const Planet& earth, float deltaTime)
{
    // Update orbit and self-rotation angles for the moon
	moon.orbitAngle += moon.orbitTranslation * deltaTime;
	if (moon.orbitAngle >= Math::Constants::TwoPi)
		moon.orbitAngle -= Math::Constants::TwoPi;
    // Self-rotation angle for the moon
	moon.selfRotationAngle += moon.rotationOnAxis * deltaTime;
	if (moon.selfRotationAngle >= Math::Constants::TwoPi)
		moon.selfRotationAngle -= Math::Constants::TwoPi;

    // Calculate the transformation matrices for the moon
	auto orbitRotation = Math::Matrix4::RotationY(moon.orbitAngle);
	auto orbitTranslation = Math::Matrix4::Translation(moon.distanceFromCenter, 0.0f, 0.0f);
	auto orbitTransform = orbitTranslation * orbitRotation;

    // Self-rotation matrix for the moon
	auto selfRotation = Math::Matrix4::RotationY(moon.selfRotationAngle);

    // Final transformation: rotate on self -> orbit around the Earth -> use Earth's position
	moon.renderData.matWorld = selfRotation * orbitTransform * earth.renderData.matWorld;
}

void GameState::UpdateRenderTargetCamera(float deltaTime, Math::Vector3 targetPosition, float objectRadius)
{
    // Update the render target camera position based on the selected planet
	if (gCurrentPlanet == PlanetRenderTarget::Sun)
	{
		mRenderTargetCamera.SetPosition({ 0.0f, 1.0f, -10.0f });
		mRenderTargetCamera.SetLookAt({ 0.0f, 0.0f, 0.0f });
	}
	else
	{
		mRenderTargetCamera.SetPosition(targetPosition + Math::Vector3(0.0f, 1.0f, -objectRadius));
		mRenderTargetCamera.SetLookAt(targetPosition);
	}
}