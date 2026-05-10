#include "Precompiled.h"
#include "GameWorld.h"
#include "GameObjectFactory.h"

#include "CameraService.h"
#include "RenderService.h"
#include "PhysicsService.h"

using namespace IExeEngine;

namespace
{
    CustomService TryAddService;
}

void GameWorld::SetCustomService(CustomService customService)
{
    TryAddService = customService;
}

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

GameObject* GameWorld::CreateGameObject(std::string name, const std::filesystem::path& templatePath)
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
    slot.gameObject->mWorld = this;

    if (!templatePath.empty())
    {
        GameObjectFactory::Make(templatePath, *slot.gameObject, *this);
    }
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

void GameWorld::LoadLevel(const std::filesystem::path& levelFile)
{
    FILE* file = nullptr;
    auto err = fopen_s(&file, levelFile.u8string().c_str(), "r");
    ASSERT(err == 0 && file != nullptr, "GameWorld: Failed to open %s!", levelFile.u8string().c_str());

    char readBuffer[65536];
    rapidjson::FileReadStream readStream(file, readBuffer, sizeof(readBuffer));
    fclose(file);

    rapidjson::Document doc;
    doc.ParseStream(readStream);

    auto services = doc["Services"].GetObj();
    for (auto& service : services)
    {
        std::string serviceName = service.name.GetString();
        Service* newService = nullptr;
        if (serviceName == "CameraService")
        {
            newService = AddService<CameraService>();
        }
        else if (serviceName == "RenderService")
        {
            newService = AddService<RenderService>();
        }
        else if (serviceName == "PhysicsService")
        {
            newService = AddService<PhysicsService>();
        }
        else
        {
            // Check if its a custom service
            newService = TryAddService(serviceName, *this);
        }

        ASSERT(newService != nullptr, "GameWorld: Failed to add service %s!", serviceName.c_str());
        newService->Deserialize(service.value);
    }

    uint32_t capacity = static_cast<uint32_t>(doc["Capacity"].GetInt());
    Initialize(capacity);

    auto gameObjects = doc["GameObjects"].GetObj();
    for (auto& gameObject : gameObjects)
    {
        std::string name = gameObject.name.GetString();
        std::string templateFile = gameObject.value["Template"].GetString();
        GameObject* go = CreateGameObject(name, templateFile);
        GameObjectFactory::OverrideDeserialize(gameObject.value, *go);
        go->Initialize();
    }
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