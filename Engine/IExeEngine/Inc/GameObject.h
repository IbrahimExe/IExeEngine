#pragma once

#include "GameObjectHandle.h"

namespace IExeEngine
{
    //class GameWorld;

    class GameObject final
    {
    public:
        GameObject() = default;

        void Initialize();
        void Terminate();
        void Update(float deltaTime);
        void DebugUI();

        void SetName(std::string& name);
        const std::string& GetName() const;
        uint32_t GetId() const;

        const GameObjectHandle& GetHandle() const;

    private:
        friend class GameWorld;

        std::string mName = "EMPTY";
        bool mInitialized = false;
        uint32_t mId = 0;

        GameObjectHandle mHandle;
    };
}
