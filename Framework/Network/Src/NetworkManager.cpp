#include "Precompiled.h"
#include "NetworkManager.h"

#include "Client.h"
#include "Server.h"
#include "NetworkEvents.h"
#include "NetworkController.h"

using namespace IExeEngine;
using namespace IExeEngine::Network;
using namespace IExeEngine::Graphics;

namespace
{
	std::unique_ptr<NetworkManager> sNetworkManager;
	Core::WindowMessageHandler sWindowsMessageHandler;
}

LRESULT CALLBACK NetworkManager::NetworkManagerMessageHandler(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (sNetworkManager != nullptr)
	{
		switch (msg)
		{
		case WM_SOCKET:
		{
			// Check for error
			if (WSAGETSELECTERROR(lParam))
			{
				ASSERT(false, "Socket failed with error!");
				sNetworkManager->Terminate();
			}
			else
			{
				switch (WSAGETSELECTEVENT(lParam))
				{
				case FD_READ:
				{
					sNetworkManager->ReceiveMsg();
					break;
				}
				}
			}
		}
		}
	}
	return sWindowsMessageHandler.ForwardMessage(window, msg, wParam, lParam);
}

void NetworkManager::StaticInitialize(HWND handle)
{
	ASSERT(sNetworkManager == nullptr, "NetworkManager: Is already initialized!");
	sNetworkManager = std::make_unique<NetworkManager>();
	sNetworkManager->Initialize(handle);
}

void NetworkManager::StaticTerminate()
{
	if (sNetworkManager != nullptr)
	{
		sNetworkManager->Terminate();
		sNetworkManager.reset();
	}
}

NetworkManager* NetworkManager::Get()
{
	return sNetworkManager.get();
}

void NetworkManager::Initialize(HWND handle)
{
	mWindow = handle;
	mStarted = false;
	mServerAddress = "127.0.0.1"; // Default local address for network on same device
}

void NetworkManager::StartNetwork(bool server, const std::string& serverAddress)
{
	mServer = server;
	if (server)
	{
		mNetwork = new Server(mPort);
		mNetwork->Initialize(mWindow, "");

		char hostname[256];
		gethostname(hostname, sizeof(hostname));

		struct hostent* host = gethostbyname(hostname);
		ASSERT(host != nullptr, "NetworkManager: failed to get hostname~");

		struct in_addr addr;
		memcpy(&addr, host->h_addr_list[0], sizeof(struct in_addr));

		// Give local computer IPV4 address
		mServerAddress = std::string(inet_ntoa(addr));

		mPlayerIds.push_back("Server");
	}
	else
	{
		mNetwork = new Client(mPort);
		mNetwork->Initialize(mWindow, serverAddress);

		std::string msg = "Client:connected";
		SendMsg(msg.c_str(), msg.size());

        mPlayerIds.push_back("Client"); // Client by default but can be overwritten by server if server sends a SetId event

		char msgData[100];
		// Message Format: eventType sender data
		sprintf_s(msgData, "%d %s", (int)EventType::Connected, GetLocalId().c_str());
		SendMsg(msgData, 100);
	}

	mConnected = false;
	mStarted = true;

	sWindowsMessageHandler.Hook(mWindow, NetworkManagerMessageHandler);
}

void NetworkManager::Update(float deltaTime)
{
	// If not started -> return
	if (!mStarted)
	{
		return;
	}

    // Even if we dont get messages, 
	// we still need to update the network element to handle timeouts and other network events
	for (auto& nextSetData : mNextSetIdAttempt)
	{
		nextSetData.second -= deltaTime;
		if (nextSetData.second <= 0.0f)
		{
			nextSetData.second = 0.5f;
			// Send SetId again since we dont know if player got already
			char msgData[100];
			// Message format: eventType sender data
			sprintf_s(msgData, "%d %s %s", (int)EventType::SetId, GetLocalId().c_str(), nextSetData.first.c_str());
			SendMsg(msgData, 100);
		}
	}

	// If no data -> return
	if (mNetwork->GetDataLength() <= 0)
	{
		return;
	}

	const char* data = mNetwork->GetData();
	LOG("[Network] %s", data);
	mConnected = true; // If data is received, we may assume we are connected
    EventType event = (EventType)(data[0] - '0');
	switch (event)
	{
		// Only done by SERVER (Client was connected)
	case EventType::Connected:
	{
		char msgData[100];
		std::string clientId = "Client" + std::to_string(mPlayerIds.size());
		// Message Format: eventType sender data
		sprintf_s(msgData, "%d %s %s", (int)EventType::SetId, GetLocalId().c_str(), clientId.c_str());
		SendMsg(msgData, 100);
		mNextSetIdAttempt[clientId] = 0.5f;
	}
	break;
	case EventType::SetId:
	{
		// Consume data
		int eventType = 0;
		char senderId[100];
		char localId[100];
		sscanf_s(data, "%d %s %s", &eventType, senderId, (unsigned int)sizeof(senderId), localId, (unsigned int)sizeof(localId));
		mPlayerIds[0] = localId;
		if (std::find(mPlayerIds.begin(), mPlayerIds.end(), senderId) == mPlayerIds.end())
		{
			mPlayerIds.push_back(senderId);
		}

		char msgData[100];
		sprintf_s(msgData, "%d %s", (int)EventType::IdRecieved, GetLocalId().c_str());
		SendMsg(msgData, 100);
	}
	break;
	// Only done by Server
	case EventType::IdRecieved:
	{
		int eventType = 0;
		char senderId[100];
		sscanf_s(data, "%d %s", &eventType, senderId, sizeof(senderId));
		auto itr = mNextSetIdAttempt.find(senderId);
		if (itr != mNextSetIdAttempt.end())
		{
			mNextSetIdAttempt.erase(itr);
		}
		if (std::find(mPlayerIds.begin(), mPlayerIds.end(), senderId) == mPlayerIds.end())
		{
			mPlayerIds.push_back(senderId);
		}
	}
	break;
	case EventType::SetPosition:
	{
		int eventType = 0;
		char senderId[100];
		Math::Vector3 position;
		sscanf_s(data, "%d %s %f %f %f", &eventType, senderId, (unsigned int)sizeof(senderId), &position.x, &position.y, &position.z);
		auto itr = mNetworkControllers.find(senderId);
		if (itr != mNetworkControllers.end())
		{
			itr->second->SetPosition(position);
		}
	}
	break;
	case EventType::Input:
	{
		int eventType = 0;
		EventInput input;
		char senderId[100];
		sscanf_s(data, "%d %s %d %d %d %d", &eventType, senderId, (unsigned int)sizeof(senderId), &input.moveX, &input.moveY, &input.jump, &input.shiftSpeed);
		auto itr = mNetworkControllers.find(senderId);
		if (itr != mNetworkControllers.end())
		{
			itr->second->SetInput(input);
		}
	}
	break;
	default:
		ASSERT(false, "NetworkManager: Invalid network event!");
		break;
	}

	// TODO: Handle all the messages that come and apply effects

	mNetwork->ResetMsg();
}

void NetworkManager::Terminate()
{
	sWindowsMessageHandler.Unhook();
	if (mNetwork != nullptr)
	{
		mNetwork->Terminate();
		delete mNetwork;
		mNetwork = nullptr;
	}
	mStarted = false;
	mConnected = false;
}

void NetworkManager::DebugUI()
{
	if (ImGui::CollapsingHeader("Network", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (mConnected || (mServer && mNetwork != nullptr))
		{
			ImGui::LabelText("LocalId", GetLocalId().c_str());

			if (mServer)
			{
				ImGui::LabelText("Element", "Server");
			}
			else
			{
				ImGui::LabelText("Element", "Client");
			}
			ImGui::LabelText("ServerAddress", mServerAddress.c_str());
			ImGui::LabelText("Port", "%d", mPort);
		}
		else if (mNetwork == nullptr)
		{
			ImGui::Checkbox("Server", &mServer);
			int port = mPort;
			if (ImGui::DragInt("Port", &port, 1, 0, 60000))
			{
				mPort = static_cast<unsigned short>(port);
			}
			ImGui::InputText("Server Address", mServerAddress.data(), mServerAddress.capacity());
			if (ImGui::Button("Connect"))
			{
				StartNetwork(mServer, mServerAddress);
			}
		}
	}
}

void NetworkManager::ReceiveMsg()
{
	mNetwork->ReceiveMsg();
}

void NetworkManager::SendMsg(const char* msg, int length)
{
	mNetwork->SendMsg(msg, length);
}

const std::string& NetworkManager::GetLocalId() const
{
	ASSERT(!mPlayerIds.empty(), "NetworkManager: No Players are added!");
	return mPlayerIds[0];
}

const std::vector<std::string>& NetworkManager::GetPlayerIds() const
{
	return mPlayerIds;
}

void NetworkManager::SetNetworkController(const std::string& id, NetworkController* networkController)
{
	mNetworkControllers[id] = networkController;
}

void NetworkManager::RemoveNetworkController(const std::string& id)
{
	auto itr = mNetworkControllers.find(id);
	if (itr != mNetworkControllers.end())
	{
		mNetworkControllers.erase(itr);
	}
}