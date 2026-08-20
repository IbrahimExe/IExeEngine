#pragma once

#pragma once

#include "NetworkElement.h"

namespace IExeEngine::Network
{
	class Server final : public NetworkElement
	{
	public:
		explicit Server(unsigned short port = DEFAULT_PORT);
		~Server() override;

		void Initialize(HWND handle, const std::string& serverAddress) override;
		void Terminate() override;
		void ReceiveMsg() override;
		void SendMsg(const char* msg, int length) override;

		bool HasClient() const;

	private:
		bool mConnected = false;
	};
}