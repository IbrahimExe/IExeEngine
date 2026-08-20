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
	};
}