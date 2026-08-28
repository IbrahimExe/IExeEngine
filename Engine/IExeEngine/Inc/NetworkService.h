#pragma once

#include "Service.h"

namespace IExeEngine
{
	class NetworkControllerComponent;

	class NetworkService final : public Service
	{
	public:
		SET_TYPE_ID(ServiceId::Network);

		void Update(float deltatime) override;
		void DebugUI() override;
		void Deserialize(const rapidjson::Value& value) override;

		void RegisterPlayer(NetworkControllerComponent* networkController);
		void UnregisterPlayer(NetworkControllerComponent* networkController);

	private:
		struct PlayerEntry
		{
			std::string id;
			NetworkControllerComponent* networkComponent = nullptr;
		};

		std::vector<PlayerEntry> mPlayers;
		std::filesystem::path mPlayerTemplate;
	};
}