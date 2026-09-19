#include "Precompiled.h"
#include "Client.h"

using namespace IExeEngine;
using namespace IExeEngine::Network;

Client::Client(unsigned short port)
	: NetworkElement(port)
{

}

Client::~Client()
{
	Terminate();
}

void Client::Initialize(HWND handle, const std::string& serverAddress)
{
	if (mInitialized)
	{
		return;
	}
	if (serverAddress.empty())
	{
		mWSAErr = WSAEINVAL;
		return;
	}
	if (!StartNetwork())
	{
		return;
	}

	mMsgConnection = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (mMsgConnection == INVALID_SOCKET)
	{
		mWSAErr = WSAGetLastError();
		Terminate();
		return;
	}

	sockaddr_in localAddress{};
	localAddress.sin_family = AF_INET;
	localAddress.sin_port = htons(0); // Make this 0 for any available port
	localAddress.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(mMsgConnection, reinterpret_cast<const sockaddr*>(&localAddress), sizeof(localAddress)) == SOCKET_ERROR)
	{
		mWSAErr = WSAGetLastError();
		Terminate();
		return;
	}

	if (!ResolveServerAddress(serverAddress))
	{
		ASSERT(FALSE, "Client: Failed to resolve server address!");
		Terminate();
		return;
	}
	if (!ConfigureSocketForMessages(handle))
	{
		ASSERT(FALSE, "Client: Failed to configure socket for message!");
		Terminate();
		return;
	}

	ResetMsg();
	mInitialized = true;

}

void Client::Terminate()
{
	if (mMsgConnection != INVALID_SOCKET)
	{
		WSAAsyncSelect(mMsgConnection, nullptr, 0, 0);
		closesocket(mMsgConnection);
		mMsgConnection = INVALID_SOCKET;
	}

	mInitialized = false;
	StopNetwork();
}

void Client::ReceiveMsg()
{
	if (!mInitialized || mMsgConnection == INVALID_SOCKET)
	{
		return;
	}

	sockaddr_in senderAddress{};
	int senderAddressLength = sizeof(senderAddress);

	int dataReceived = recvfrom(mMsgConnection,
		mDataBuffer.data(),
		static_cast<int>(mDataBuffer.size()),
		0,
		reinterpret_cast<sockaddr*>(&senderAddress),
		&senderAddressLength);
	if (dataReceived == SOCKET_ERROR)
	{
		mWSAErr = WSAGetLastError();
		return;
	}

	mDataLength = dataReceived;
	if (static_cast<size_t>(dataReceived) < mDataBuffer.size())
	{
		mDataBuffer[dataReceived] = '\0';
	}
}

void Client::SendMsg(const char* msg, int length)
{
	if (!mInitialized || mMsgConnection == INVALID_SOCKET)
	{
		return;
	}
	if (msg == nullptr || length <= 0 || length > RECEIVE_BUFFER_SIZE)
	{
		mWSAErr = WSAEMSGSIZE;
		return;
	}

	int dataSent = sendto(mMsgConnection,
		msg,
		length,
		0,
		reinterpret_cast<const sockaddr*>(&mRemoteServerAddr),
		sizeof(mRemoteServerAddr));
	if (dataSent == SOCKET_ERROR)
	{
		mWSAErr = WSAGetLastError();
	}
}

bool Client::ResolveServerAddress(const std::string& serverAddress)
{
	addrinfo hints{};
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;

	addrinfo* addressList = nullptr;
	const std::string portStr = std::to_string(mPort);
	mWSAErr = getaddrinfo(serverAddress.c_str(), portStr.c_str(), &hints, &addressList);
	if (mWSAErr != 0)
	{
		return false;
	}
	bool foundAddress = false;
	if (addressList != nullptr && addressList->ai_addrlen >= sizeof(sockaddr_in))
	{
		mRemoteServerAddr = *reinterpret_cast<const sockaddr_in*>(addressList->ai_addr);
		foundAddress = true;
	}

	freeaddrinfo(addressList);
	if (!foundAddress)
	{
		mWSAErr = WSAHOST_NOT_FOUND;
		return false;
	}
	return true;
}