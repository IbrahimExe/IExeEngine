#include "Precompiled.h"
#include "ZombieControllerComponent.h"
#include "TransformComponent.h"
#include "RigidBodyComponent.h"
#include "AnimatorComponent.h"
#include "SoundBankComponent.h"
#include "GameObject.h"
#include "SaveUtil.h"

using namespace IExeEngine;
using namespace IExeEngine::Input;

void ZombieControllerComponent::Initialize()
{
    mTransformComponent = GetOwner().GetComponent<TransformComponent>();
    mRigidBodyComponent = GetOwner().GetComponent<RigidBodyComponent>();
    mAnimatorComponent = GetOwner().GetComponent<AnimatorComponent>();
    mSoundBankComponent = GetOwner().GetComponent<SoundBankComponent>();

    ASSERT(mTransformComponent != nullptr,
        "ZombieControllerComponent: Requires a TransformComponent on the same GameObject!");
    ASSERT(mRigidBodyComponent != nullptr,
        "ZombieControllerComponent: Requires a RigidBodyComponent on the same GameObject!");

    // Seed random for attack sound selection
    srand(static_cast<unsigned int>(time(nullptr)));

    // Boot into idle
    TransitionToState(ZombieState::Idle);
}

void ZombieControllerComponent::Terminate()
{
    if (mSoundBankComponent != nullptr)
    {
        mSoundBankComponent->StopAll();
    }

    mTransformComponent = nullptr;
    mRigidBodyComponent = nullptr;
    mAnimatorComponent = nullptr;
    mSoundBankComponent = nullptr;
}

void ZombieControllerComponent::Update(float deltaTime)
{
    // Always tick the attack cooldown down, regardless of state
    if (mCooldownTimer > 0.0f)
    {
        mCooldownTimer -= deltaTime;
    }

    // Attacking state: run timer logic and lock out movement 
    if (mCurrentState == ZombieState::Attacking)
    {
        UpdateAttack(deltaTime);
        return;
    }

    // Gather movement input
    Math::Vector2 moveInput = Math::Vector2::Zero;
    float         turnInput = 0.0f;
    UpdateMovement(deltaTime, moveInput, turnInput);

    const bool isMoving = (moveInput.x != 0.0f || moveInput.y != 0.0f);

    // Transition Idle <-> Walking
    if (isMoving && mCurrentState != ZombieState::Walking)
    {
        TransitionToState(ZombieState::Walking);
    }
    else if (!isMoving && mCurrentState != ZombieState::Idle)
    {
        TransitionToState(ZombieState::Idle);
    }

    auto input = InputSystem::Get();
    if (input->IsKeyPressed(KeyCode::SPACE) && mCooldownTimer <= 0.0f)
    {
        TransitionToState(ZombieState::Attacking);
        return;
    }

    // Apply physics velocity
    Math::Vector3 forward = Math::GetLook(mTransformComponent->GetMatrix4());
    Math::Vector3 right = Math::GetRight(mTransformComponent->GetMatrix4());
    forward = Math::Normalize({ forward.x, 0.0f, forward.z });
    right = Math::Normalize({ right.x,   0.0f, right.z });

    Math::Vector3 vel = (forward * moveInput.y) + (right * moveInput.x);
    vel.y = mRigidBodyComponent->GetVelocity().y; // Preserve gravity / falling velocity

    mRigidBodyComponent->SetVelocity(vel);

    Math::Vector3 angularVel = mRigidBodyComponent->GetAngularVelocity();
    angularVel.y = turnInput;
    mRigidBodyComponent->SetAngularVelocity(angularVel);
}

void ZombieControllerComponent::UpdateMovement(float deltaTime,
    Math::Vector2& outMoveInput,
    float& outTurnInput)
{
    auto input = InputSystem::Get();

    if (input->IsKeyDown(KeyCode::W)) outMoveInput.y = mMoveSpeed;
    if (input->IsKeyDown(KeyCode::S)) outMoveInput.y = -mMoveSpeed;
    if (input->IsKeyDown(KeyCode::D)) outMoveInput.x = mMoveSpeed;
    if (input->IsKeyDown(KeyCode::A)) outMoveInput.x = -mMoveSpeed;

    outTurnInput = 0.0f;
    if (input->IsMouseDown(MouseButton::RBUTTON))
    {
        outTurnInput = input->GetMouseMoveX() * mTurnSpeed;
    }
}

void ZombieControllerComponent::UpdateAttack(float deltaTime)
{
    mAttackTimer += deltaTime;

    const float hitFrame = mAttackDuration * 0.5f;
    if (!mAttackImpulseApplied && mAttackTimer >= hitFrame)
    {
        ApplyAttackImpulse();
        mAttackImpulseApplied = true;
    }

    // Attack animation finished — start cooldown and return to idle
    if (mAttackTimer >= mAttackDuration)
    {
        mCooldownTimer = mAttackCooldown;
        TransitionToState(ZombieState::Idle);
    }
}

void ZombieControllerComponent::TransitionToState(ZombieState newState)
{
    if (mCurrentState == newState) return;
    mCurrentState = newState;

    switch (newState)
    {
        // Idle 
    case ZombieState::Idle:
    {
        if (mAnimatorComponent != nullptr)
        {
            mAnimatorComponent->Play(mIdleAnimIndex, /*looping=*/true);
        }
        if (mSoundBankComponent != nullptr)
        {
            mSoundBankComponent->Stop("ZIdle.wav"); // Reset before replaying
            mSoundBankComponent->Play("ZIdle.wav");
        }
        break;
    }

    // Walking
    case ZombieState::Walking:
    {
        if (mAnimatorComponent != nullptr)
        {
            mAnimatorComponent->Play(mWalkAnimIndex, /*looping=*/true);
        }
        if (mSoundBankComponent != nullptr)
        {
            mSoundBankComponent->Stop("ZIdle.wav"); // Footstep loop stops while walking sound plays
        }
        break;
    }

    // Attacking
    case ZombieState::Attacking:
    {
        // Reset attack tracking
        mAttackTimer = 0.0f;
        mAttackImpulseApplied = false;

        if (mAnimatorComponent != nullptr)
        {
            mAnimatorComponent->Play(mAttackAnimIndex, /*looping=*/false);
        }

        if (mSoundBankComponent != nullptr)
        {
            mSoundBankComponent->Stop("ZIdle.wav");
            mSoundBankComponent->Play("ZScream.wav");                // Scream on attack trigger
            mSoundBankComponent->Play(GetRandomAttackSound());        // Random swing sound
        }

        // stop all movement so the zombie stands still during its swing
        Math::Vector3 vel = mRigidBodyComponent->GetVelocity();
        vel.x = 0.0f;
        vel.z = 0.0f;
        mRigidBodyComponent->SetVelocity(vel);
        mRigidBodyComponent->SetAngularVelocity({ 0.0f, 0.0f, 0.0f });
        break;
    }
    } // end switch
}

void ZombieControllerComponent::ApplyAttackImpulse()
{
    // Build the attack impulse direction: forward + slight upward arc
    Math::Vector3 forward = Math::GetLook(mTransformComponent->GetMatrix4());
    forward = Math::Normalize({ forward.x, 0.0f, forward.z });
    const Math::Vector3 impulseDir = Math::Normalize({ forward.x, 0.3f, forward.z });


}

std::string ZombieControllerComponent::GetRandomAttackSound() const
{
    if (mAttackSoundKeys.empty()) return "";
    const int idx = rand() % static_cast<int>(mAttackSoundKeys.size());
    return mAttackSoundKeys[idx];
}

//  DebugUI
void ZombieControllerComponent::DebugUI()
{
    if (ImGui::CollapsingHeader("ZombieController"))
    {
        // Live state readout
        const char* stateLabels[] = { "Idle", "Walking", "Attacking" };
        ImGui::Text("State:    %s", stateLabels[static_cast<int>(mCurrentState)]);
        ImGui::Text("Cooldown: %.2fs", mCooldownTimer);
        ImGui::Text("AtkTimer: %.2fs", mAttackTimer);
        ImGui::Separator();

        // Movement
        ImGui::Text("-- Movement --");
        ImGui::DragFloat("MoveSpeed", &mMoveSpeed, 0.1f, 0.1f, 20.0f);
        ImGui::DragFloat("TurnSpeed", &mTurnSpeed, 0.001f, 0.001f, 1.0f);
        ImGui::Separator();

        // Attack
        ImGui::Text("-- Attack --");
        ImGui::DragFloat("AttackRange", &mAttackRange, 0.1f, 0.5f, 20.0f);
        ImGui::DragFloat("AttackForce", &mAttackForce, 0.5f, 1.0f, 100.0f);
        ImGui::DragFloat("AttackDuration", &mAttackDuration, 0.05f, 0.1f, 5.0f);
        ImGui::DragFloat("AttackCooldown", &mAttackCooldown, 0.05f, 0.0f, 10.0f);
        ImGui::Separator();

        // Animation indices (adjust until clips look right)
        ImGui::Text("-- Animation Indices --");
        ImGui::DragInt("Idle Anim", &mIdleAnimIndex, 1, 0, 20);
        ImGui::DragInt("Walk Anim", &mWalkAnimIndex, 1, 0, 20);
        ImGui::DragInt("Attack Anim", &mAttackAnimIndex, 1, 0, 20);
        ImGui::Separator();

        // Manual trigger for testing attack without spacebar
        if (ImGui::Button("Force Attack") && mCurrentState != ZombieState::Attacking)
        {
            TransitionToState(ZombieState::Attacking);
        }
    }
}

//  Deserialize
void ZombieControllerComponent::Deserialize(const rapidjson::Value& value)
{
    SaveUtil::ReadFloat("MoveSpeed", mMoveSpeed, value);
    SaveUtil::ReadFloat("TurnSpeed", mTurnSpeed, value);
    SaveUtil::ReadFloat("AttackRange", mAttackRange, value);
    SaveUtil::ReadFloat("AttackForce", mAttackForce, value);
    SaveUtil::ReadFloat("AttackDuration", mAttackDuration, value);
    SaveUtil::ReadFloat("AttackCooldown", mAttackCooldown, value);

    // Correctly use ReadInt now that we know it exists
    SaveUtil::ReadInt("IdleAnimIndex", mIdleAnimIndex, value);
    SaveUtil::ReadInt("WalkAnimIndex", mWalkAnimIndex, value);
    SaveUtil::ReadInt("AttackAnimIndex", mAttackAnimIndex, value);
}