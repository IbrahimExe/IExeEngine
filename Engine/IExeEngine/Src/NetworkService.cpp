#include "Precompiled.h"
#include "NetworkService.h"
#include "SaveUtil.h"
#include "GameWorld.h"
#include "NetworkControllerComponent.h"
#include "TransformComponent.h"

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
				NetworkManager::Get()->RemoveNetworkController(mPlayers[0].id);
				mPlayers[0].id = id;
				NetworkManager::Get()->SetNetworkController(id, mPlayers[0].networkComponent->GetNetworkController());
			}
			else
			{
				GameObject* go = GetWorld().CreateGameObject(id, mPlayerTemplate);
				// Will register automatically and add player to the list
				go->Initialize();

				// Let the new player know where I am at (then they will do the same)
				TransformComponent* transform = mPlayers[0].networkComponent->GetOwner().GetComponent<TransformComponent>();
				Math::Vector3 position = transform->position;

				// Send initial position update
				char msgData[100];
				sprintf_s(msgData, "%d %s %.4f %.4f %.4f", (int)Network::EventType::SetPosition, mPlayers[0].id.c_str(), position.x, position.y, position.z);
				NetworkManager::Get()->SendMsg(msgData, 100);
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
	ASSERT(!mPlayerTemplate.empty(), "NetworkService: player template is required!");
}

void NetworkService::RegisterPlayer(NetworkControllerComponent* networkController)
{
	auto itr = std::find_if(mPlayers.begin(), mPlayers.end(),
		[&](const PlayerEntry& entry)
		{
			return entry.networkComponent == networkController;
		});
	if (itr == mPlayers.end())
	{
		PlayerEntry newEntry;
		newEntry.id = networkController->GetOwner().GetName();
		newEntry.networkComponent = networkController;
		networkController->SetRemote(mPlayers.size() > 0);
		mPlayers.push_back(newEntry);

		NetworkManager::Get()->SetNetworkController(newEntry.id, newEntry.networkComponent->GetNetworkController());
	}
}

void NetworkService::UnregisterPlayer(NetworkControllerComponent* networkController)
{
	auto itr = std::find_if(mPlayers.begin(), mPlayers.end(),
		[&](const PlayerEntry& entry)
		{
			return entry.networkComponent == networkController;
		});
	if (itr != mPlayers.end())
	{
		NetworkManager::Get()->RemoveNetworkController(itr->id);
		mPlayers.erase(itr);
	}
}