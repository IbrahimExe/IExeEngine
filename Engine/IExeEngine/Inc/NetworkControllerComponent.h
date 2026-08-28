#pragma once

#include "Component.h"

namespace IExeEngine
{
	class TransformComponent;
	class RigidBodyComponent;

	class NetworkControllerComponent : public Component
	{
	public:
		SET_TYPE_ID(ComponentId::NetworkController);

		void Initialize() override;
		void Terminate() override;
		void Update(float deltaTime) override;
		void DebugUI() override;
		void Deserialize(const rapidjson::Value& value) override;

		Network::NetworkController* GetNetworkController();
		void SetRemote(bool remote);
	
	private:
		TransformComponent* mTransformComponent = nullptr;
		RigidBodyComponent* mRigidBodyComponent = nullptr;
		std::unique_ptr<Network::NetworkController> mNetworkController;

		float mShiftSpeed = 10.0f;
		float mMoveSpeed = 1.0f;
		float mTurnSpeed = 0.1f;
		float mJumpSpeed = 10.0f;
		bool mRemote = false;

	};
}