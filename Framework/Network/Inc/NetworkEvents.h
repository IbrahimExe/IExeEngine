#pragma once

namespace IExeEngine::Network
{
	enum class EventType
	{
		Invalid = 0,
		Connected,
		SetId,
		IdRecieved,
		SetPosition,
		Input
	};

	struct EventSetPosition
	{
		EventSetPosition() = default;
		Math::Vector3 position;
	};

	struct EventInput
	{
		EventInput() = default;
		int moveX = 0;
		int moveY = 0;
		int jump = 0;
		int shiftSpeed = 0;
	};
}