#pragma once

#include "NetworkEvents.h"

namespace IExeEngine::Network
{
	class NetworkController
	{
	public:
		NetworkController() = default;
		~NetworkController() = default;

		void SetPosition(const Math::Vector3& position);
		bool TryGetNewPosition(Math::Vector3& newPosition);

		void SetInput(const EventInput& input);
		bool TryGetDirtyInput(EventInput& input);
		const EventInput& GetInput() const;
	
	private:
		bool mNewPosition = false;
		Math::Vector3 mPosition;

		bool mInputIsDirty = false;
		EventInput mInput;
	};
}