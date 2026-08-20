#include "Precompiled.h"
#include "NetworkService.h"

using namespace IExeEngine;
using namespace IExeEngine::Network;

void NetworkService::Update(float deltaTime)
{
	NetworkManager::Get()->Update(deltaTime);
}

void NetworkService::DebugUI()
{
	NetworkManager::Get()->DebugUI();
}