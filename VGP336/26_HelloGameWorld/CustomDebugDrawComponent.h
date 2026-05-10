#pragma once

#include "CustomTypeIds.h"
#include <IExeEngine/Inc/IExeEngine.h>

class CustomDebugDrawComponent : public IExeEngine::Component
{
public:
    SET_TYPE_ID(CustomComponentId::CustomDebugDraw);

    void Initialize() override;

    void Terminate() override;

    void DebugUI() override;

    void Deserialize(const rapidjson::Value& value) override;

    void AddDebugDraw() const;

private:
    const IExeEngine::TransformComponent* mTransformComponent = nullptr;
    IExeEngine::Math::Vector3 mPosition = IExeEngine::Math::Vector3::Zero;
    IExeEngine::Graphics::Color mColor = IExeEngine::Graphics::Colors::White;

    uint32_t mSlices = 0;
    uint32_t mRings = 0;
    float mRadius = 0;
};