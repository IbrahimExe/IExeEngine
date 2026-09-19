#include "Precompiled.h"
#include "NetworkElement.h"

using namespace IExeEngine;
using namespace IExeEngine::Network;

NetworkElement::NetworkElement(unsigned short port)
	: mPort(port)
{

}

NetworkElement::~NetworkElement()
{
	StopNetwork();
}

bool NetworkElement::IsInitialized() const
{
	return mInitialized;
}

SOCKET NetworkElement::GetSocket() const
{
	return mMsgConnection;
}

unsigned short NetworkElement::GetPort() const
{
	return mPort;
}

const char* NetworkElement::GetData() const
{
	return mDataBuffer.data();
}

int NetworkElement::GetDataLength() const
{
	return mDataLength;
}

int NetworkElement::GetLastError() const
{
	return mWSAErr;
}

void NetworkElement::ResetMsg()
{
	mDataBuffer.fill('\0');
	mDataLength = 0;
}

bool NetworkElement::StartNetwork()
{
	if (mNetworkStarted)
	{
		return true;
	}

	WSADATA wsaData{};
	mWSAErr = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (mWSAErr != 0)
	{
		return false;
	}

	mNetworkStarted = true;
	return true;
}

void NetworkElement::StopNetwork()
{
	if (mNetworkStarted)
	{
		WSACleanup();
		mNetworkStarted = false;
	}
}

bool NetworkElement::ConfigureSocketForMessages(HWND handle)
{
	if (mMsgConnection == INVALID_SOCKET)
	{
		return false;
	}
	if (handle != nullptr)
	{

		if (WSAAsyncSelect(mMsgConnection, handle, WM_SOCKET, FD_READ | FD_CLOSE) == SOCKET_ERROR)
		{
			mWSAErr = WSAGetLastError();
			return false;
		}

		return true;
	}
	return false;
}