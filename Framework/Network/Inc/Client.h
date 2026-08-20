#pragma once

#include "NetworkElement.h"

namespace IExeEngine::Network
{
	class Client final : public NetworkElement
	{
	public:
		explicit Client(unsigned short port = DEFAULT_PORT);
		~Client() override;

		void Initialize(HWND handle, const std::string& serverAddress) override;
		void Terminate() override;
		void ReceiveMsg() override;
		void SendMsg(const char* msg, int length) override;

	private:
		bool ResolveServerAddress(const std::string& serverAddress);
		sockaddr_in mRemoteServerAddr = {};
	};
}