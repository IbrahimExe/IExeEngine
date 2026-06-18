#pragma once

#include "Component.h"
#include <string>
#include <vector>

namespace IExeEngine
{
    class TransformComponent;
    class RigidBodyComponent;
    class AnimatorComponent;
    class SoundBankComponent;

    class ZombieControllerComponent : public Component
    {
    public:
        SET_TYPE_ID(ComponentId::ZombieController);

        void Initialize() override;

        void Terminate() override;

        void Update(float deltaTime) override;

        void DebugUI() override;

        void Deserialize(const rapidjson::Value& value) override;

    private:
        // animation state machine
        enum class ZombieState
        {
            Idle,
            Walking,
            Attacking
        };

        void UpdateMovement(float deltaTime, Math::Vector2& outMoveInput, float& outTurnInput);
        void UpdateAttack(float deltaTime);
        void TransitionToState(ZombieState newState);
        void ApplyAttackImpulse();
        std::string GetRandomAttackSound() const;

        // COmponent refs
        TransformComponent* mTransformComponent = nullptr;
        RigidBodyComponent* mRigidBodyComponent = nullptr;
        AnimatorComponent* mAnimatorComponent = nullptr;
        SoundBankComponent* mSoundBankComponent = nullptr;

        // state
        ZombieState mCurrentState = ZombieState::Idle;

        // movement
        float mMoveSpeed = 2.0f;
        float mTurnSpeed = 0.1f;

        // attack
        float mAttackRange = 2.5f;   // Radius in world units to check for targets
        float mAttackForce = 20.0f;  // Impulse magnitude applied to hit targets
        float mAttackDuration = 1.2f;   // How long the attacking state lasts (seconds)
        float mAttackCooldown = 2.0f;   // Minimum time between attacks (seconds)
        float mAttackTimer = 0.0f;   // Tracks time spent in Attacking state
        float mCooldownTimer = 0.0f;   // Counts down to 0 before next attack is allowed
        bool  mAttackImpulseApplied = false; // Ensures impulse fires exactly once per attack

        // Animation Indices
        int mIdleAnimIndex = 0;
        int mWalkAnimIndex = 1;
        int mAttackAnimIndex = 2;

        // Sound
        std::vector<std::string> mAttackSoundKeys; // Randomly chosen sound file on each attack 
    };
}