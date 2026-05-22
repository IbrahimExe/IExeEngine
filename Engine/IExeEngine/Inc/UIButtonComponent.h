#pragma once

#include "UIComponent.h"

namespace IExeEngine
{
    enum class ButtonState
    {
        Default,
        Hover,
        Click,
        Disabled,
        Count
    };

    using ButtonCallBack = std::function<void()>;

    class UIButtonComponent : public UIComponent
    {
    public:
        SET_TYPE_ID(ComponentId::UIButton);

        void Initialize() override;
        void Terminate() override;
        void Update(float deltaTime) override;
        void Render() override;
        void Deserialize(const rapidjson::Value& value) override;

        Math::Vector2 GetPosition(bool includeOrigin = true);
        void SetCallBack(ButtonCallBack cb);

    private:
        void OnClick();

        struct ButtonStateEntry
        {
            std::string texture;
            Graphics::UISprite sprite;
        };
        using ButtonStates = std::array < ButtonStateEntry, static_cast<uint32_t>(ButtonState::Count)>;
        ButtonStates mButtonStates;

        ButtonCallBack mCallBack = nullptr;
        DirectX::XMFLOAT2 mPosition = { 0.0f, 0.0f };
        ButtonState mCurrentState = ButtonState::Default;
    };
}