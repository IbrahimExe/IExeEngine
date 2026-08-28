#include "Precompiled.h"
#include "NetworkService.h"
#include "SaveUtil.h"
#include "GameWorld.h"

using namespace IExeEngine;
using namespace IExeEngine::Network;

void NetworkService::Update(float deltaTime)
{
	NetworkManager::Get()->Update(deltaTime);

	const std::vector<std::string>& playerIds = NetworkManager::Get()->GetPlayerIds();
	for (const std::string& id : playerIds)
	{
		auto iter = std::find_if(mPlayers.begin(), mPlayers.end(),
			[id](const PlayerEntry& entry)
			{
				return id == entry.id;
			});

		if (iter == mPlayers.end())
		{
			if (NetworkManager::Get()->GetLocalId() == id)
			{
				mPlayers[0].id = id;
			}
			else
			{
				GameObject* go = GetWorld().CreateGameObject(id, mPlayerTemplate);
				go->Initialize();

				// Remove Later
				PlayerEntry entry;
				entry.id = id;
				mPlayers.push_back(entry);

			}
		}
	}
}

void NetworkService::DebugUI()
{
	NetworkManager::Get()->DebugUI();
}

void NetworkService::Deserialize(const rapidjson::Value& value)
{
	std::string playerTemplate;
	SaveUtil::ReadString("PlayerTemplate", playerTemplate, value);
	mPlayerTemplate = playerTemplate;
	ASSERT(!mPlayerTemplate.empty(), "NetworkService: player template is required");
}