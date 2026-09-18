#include "Precompiled.h"
#include "Server.h"

using namespace IExeEngine;
using namespace IExeEngine::Network;

Server::Server(unsigned short port)
	: NetworkElement(port)
{

}

Server::~Server()
{
	Terminate();
}

void Server::Initialize(HWND handle, const std::string& serverAddress)
{
	if (mInitialized)
	{
		return;
	}
	if (!StartNetwork())
	{
		return;
	}

	mMsgConnection = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	// UDP (User Datagram Protocol) is fast, wild, continuouly throws data out.
	// It does not wait or check if it arrived, good for game engines and networking.
	if (mMsgConnection == INVALID_SOCKET)
	{
		mWSAErr = WSAGetLastError();
		Terminate();
		return;
	}

	BOOL reuseAddress = TRUE;
	if (setsockopt(mMsgConnection,
		SOL_SOCKET,
		SO_REUSEADDR,
		reinterpret_cast<const char*>(&reuseAddress),
		sizeof(reuseAddress)) == SOCKET_ERROR)
	{
		mWSAErr = WSAGetLastError();
		Terminate();
		return;
	}

	mServerAddr = {};
	mServerAddr.sin_family = AF_INET;
	mServerAddr.sin_port = htons(mPort);
	mServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(mMsgConnection, reinterpret_cast<const sockaddr*>(&mServerAddr), sizeof(mServerAddr)) == SOCKET_ERROR)
	{
		mWSAErr = WSAGetLastError();
		Terminate();
		return;
	}

	if (!ConfigureSocketForMessages(handle))
	{
		ASSERT(false, "Server: Failed to configure socket!");
		Terminate();
		return;
	}

	ResetMsg();
	mConnected = false;
	mInitialized = true;
}

void Server::Terminate()
{
	if (mMsgConnection != INVALID_SOCKET)
	{
		WSAAsyncSelect(mMsgConnection, nullptr, 0, 0);
		closesocket(mMsgConnection);
		mMsgConnection = INVALID_SOCKET;
	}

	mConnected = false;
	mInitialized = false;
	StopNetwork();
}

void Server::ReceiveMsg()
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
		// WSAEWOULDBLOCK is normal for a non_blocking socket
		//if (mWSAErr == WSAEWOULDBLOCK)
		//{
		//	return;
		//}
		//return;
	}

	mClientAddr = senderAddress;
	mDataLength = dataReceived;
	mConnected = true;

	if (static_cast<std::size_t>(dataReceived) < mDataBuffer.size())
	{
		mDataBuffer[dataReceived] = '\0';
	}
}

void Server::SendMsg(const char* msg, int length)
{
	if (!mInitialized || mMsgConnection == INVALID_SOCKET || !mConnected || msg == nullptr || length <= 0)
	{
		return;
	}

	if (static_cast<std::size_t>(length) > RECEIVE_BUFFER_SIZE)
	{
		return;
	}

	int dataSent = sendto(mMsgConnection,
		msg,
		length,
		0,
		reinterpret_cast<const sockaddr*>(&mClientAddr),
		sizeof(mClientAddr));

	if (dataSent == SOCKET_ERROR)
	{
		mWSAErr = WSAGetLastError();
	}
}

bool Server::HasClient() const
{
	return mConnected;
}