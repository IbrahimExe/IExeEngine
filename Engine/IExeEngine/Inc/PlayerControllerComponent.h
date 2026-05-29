#pragma once

#include "Component.h"

namespace IExeEngine
{
    class TransformComponent;
    class RidgidBodyComponent;

    class PlayerControllerComponent : public Component
    {
    public:
        void Initialize() override;

        void Terminate() override;

        void Update(float deltaTime) override;

        void DebugUI() override;

        void Deserialize(const rapidjson::Value& value) override;

    private:
        TransformComponent* mTransformComponent = nullptr;
        RidgidBodyComponent* mRidgidBodyComponent = nullptr;

        float mShiftSpeed = 10.0f;
        float mMoveSpeed = 2.0f;
        float mTurnSpeed = 0.1f;
    };
}