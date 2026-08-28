#pragma once

#include "Service.h"

namespace IExeEngine
{
	class NetworkService final : public Service
	{
	public:
		SET_TYPE_ID(ServiceId::Network);

		void Update(float deltatime) override;
		void DebugUI() override;
		void Deserialize(const rapidjson::Value& value) override;

	private:
		struct PlayerEntry
		{
			std::string id;
		};

		std::vector<PlayerEntry> mPlayers;
		std::filesystem::path mPlayerTemplate;
	};
}