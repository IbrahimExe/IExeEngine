#pragma once

#include "NetworkElement.h"

namespace IExeEngine::Network
{
	class NetworkManager;

	class NetworkManager final
	{
	public:
		static void StaticInitialize(HWND handle);
		static void StaticTerminate();
		static NetworkManager* Get();

		NetworkManager() = default;
		~NetworkManager() = default;

		void Initialize(HWND handle);
		void StartNetwork(bool server, const std::string& serverAddress);
		void Update(float deltaTime);
		void Terminate();
		void DebugUI();

		void ReceiveMsg();
		void SendMsg(const char* msg, int length);

		const std::string& GetLocalId() const;
		const std::vector<std::string>& GetPlayerIds() const;

	private:
		static LRESULT CALLBACK NetworkManagerMessageHandler(HWND window, UINT msg, WPARAM wParam, LPARAM lParam);

		bool mStarted = false;
		bool mConnected = false;
		bool mServer = false;
		unsigned short mPort = DEFAULT_PORT;
		std::string mServerAddress;
		NetworkElement* mNetwork = nullptr;
		HWND mWindow = nullptr;
		std::string mWriteMessage;
		std::vector<std::string> mPlayerIds;
		std::unordered_map<std::string, float> mNextSetIdAttempt;
	};
}