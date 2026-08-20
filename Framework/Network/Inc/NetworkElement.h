#pragma once

namespace IExeEngine::Network
{
	class NetworkElement
	{
	public:
        explicit NetworkElement(unsigned short port = DEFAULT_PORT); // explicit constructor to make sure its saved
		virtual ~NetworkElement();

		// Remove Copies 
		NetworkElement(const NetworkElement&) = delete;
		NetworkElement& operator=(const NetworkElement&) = delete;

		virtual void Initialize(HWND handle, const std::string& serverAddress) = 0;
		virtual void Terminate() = 0;

		virtual void ReceiveMsg() = 0;
		virtual void SendMsg(const char* msg, int length) = 0;

		// Shared Functions
		bool IsInitialized() const;
		SOCKET GetSocket() const;
		unsigned short GetPort() const;
		const char* GetData() const;
		int GetDataLength() const;
		int GetLastError() const;
		void ResetMsg();

	protected:
		bool StartNetwork();
		void StopNetwork();
		bool ConfigureSocketForMessages(HWND handle);

		SOCKET mMsgConnection = INVALID_SOCKET;
		sockaddr_in mServerAddr = {};
		sockaddr_in mClientAddr = {};

		std::array<char, RECEIVE_BUFFER_SIZE>mDataBuffer;
		int mDataLength = 0;
		unsigned short mPort = DEFAULT_PORT;
		int mWSAErr = 0;

		bool mNetworkStarted = false;
		bool mInitialized = false;
	};
}