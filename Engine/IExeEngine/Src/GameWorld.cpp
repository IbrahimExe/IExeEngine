#include "Precompiled.h"
#include "GameWorld.h"

using namespace IExeEngine;

void GameWorld::Initialize(uint32_t capacity)
{
    ASSERT(!mInitialized, "GameWorld: Already initialized!");
    // Services have to be added first, then Game Object can be added 
    for (auto& service : mServices)
    {
        service->Initialize();
    }

    mGameObjectSlots.resize(capacity);
    mFreeSlots.resize(capacity);
    std::iota(mFreeSlots.begin(), mFreeSlots.end(), 0); // Set the values of the free slots to 0, 1, 2, 3, 4, 5, 6, 7, 8, 9... up to capacity

    mInitialized = true;
}

void GameWorld::Terminate()
{
    for (Slot& slot : mGameObjectSlots)
    {
        if (slot.gameObject != nullptr)
        {
            slot.gameObject->Terminate();
            slot.gameObject.reset();
        }
    }
    mGameObjectSlots.clear();
    mFreeSlots.clear();
    mToBeDestroyed.clear();

    // Get rid of all game objects to make sure they dont mess with the services
    for (auto& service : mServices)
    {
        service->Terminate();
        service.reset();
    }
    mServices.clear();

    mInitialized = false;
}

void GameWorld::Update(float deltaTime)
{
    for (Slot& slot : mGameObjectSlots)
    {
        if (slot.gameObject != nullptr)
        {
            slot.gameObject->Update(deltaTime);
        }
    }
    for (auto& service : mServices)
    {
        service->Update(deltaTime);
    }
    
    ProcessDestoyList();
}

void GameWorld::Render()
{
    for (auto& service : mServices)
    {
        service->Render();
    }
}

void GameWorld::DebugUI()
{
    for (Slot& slot : mGameObjectSlots)
    {
        if (slot.gameObject != nullptr)
        {
            slot.gameObject->DebugUI();
        }
    }
    for (auto& service : mServices)
    {
        service->DebugUI();
    }
}

GameObject* GameWorld::CreateGameObject(std::string name)
{
    ASSERT(mInitialized, "GameWorld: Not initialized!");
    if (mFreeSlots.empty())
    {
        ASSERT(false, "GameWorld: No more free slots to create a new GameObject!");
        return nullptr;
    }

    const uint32_t freeSlot = mFreeSlots.back();
    mFreeSlots.pop_back();

    // = Is free for the game object to be created
    Slot& slot = mGameObjectSlots[freeSlot];
    slot.gameObject = std::make_unique<GameObject>();
    slot.gameObject->SetName(name);
    slot.gameObject->mHandle.mIndex = freeSlot;
    slot.gameObject->mHandle.mGeneration = slot.generation;

    return slot.gameObject.get();
}

void GameWorld::DestroyGameObject(const GameObjectHandle& handle)
{
    if (!IsValid(handle))
    {
        return;
    }

    Slot& slot = mGameObjectSlots[handle.mIndex]; // we already know where its located, despite having made and deleted ~100000s of game objects
    ++slot.generation;
    mToBeDestroyed.push_back(handle.mIndex);
}

bool GameWorld::IsValid(const GameObjectHandle& handle)
{
    if (handle.mIndex < 0 || handle.mIndex >= mGameObjectSlots.size())
    {
        return false;
    }
    if (mGameObjectSlots[handle.mIndex].generation != handle.mGeneration)
    {
        return false;
    }

    return true;
}

void GameWorld::ProcessDestoyList()
{
    for (uint32_t index : mToBeDestroyed)
    {
        Slot& slot = mGameObjectSlots[index];
        GameObject* gameObject = slot.gameObject.get();
        ASSERT(!IsValid(gameObject->GetHandle()), "GameWorld: gameObjects is still AALLIIVVEEEE!");

        gameObject->Terminate();
        slot.gameObject.reset();
        mFreeSlots.push_back(index);
    }

    mToBeDestroyed.clear();
}