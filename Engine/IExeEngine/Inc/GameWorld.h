#pragma once

#include "GameObject.h"

namespace IExeEngine
{
    class GameWorld final
    {
    public:
        void Initialize(uint32_t capacity = 10);
        void Terminate();
        void Update(float deltaTime);
        void Render();
        void DebugUI();
    
        GameObject* CreateGameObject(std::string name);
        void DestroyGameObject(const GameObjectHandle& handle);

    private:
        bool IsValid(const GameObjectHandle& handle);
        void ProcessDestoyList(); // As we never want to destoy game objects during an update loop...

        struct Slot
        {
            std::unique_ptr<GameObject> gameObject;
            uint32_t generation = 0;
        };

        using GameObjectSlots = std::vector<Slot>;
        GameObjectSlots mGameObjectSlots;
        std::vector<uint32_t> mFreeSlots;
        std::vector<uint32_t> mToBeDestroyed;
        bool mInitialized = false;
    };
}