#include "Precompiled.h"
#include "GameObjectFactory.h"
#include "Component.h"
#include "GameObject.h"
// components includes we've made 
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "FPSCameraComponent.h"
#include "MeshComponent.h"
#include "ModelComponent.h"
#include "AnimatorComponent.h"
#include "RigidBodyComponent.h"
#include "SoundEventComponent.h"
#include "SoundBankComponent.h"

using namespace IExeEngine;

namespace
{
    // Need to add above the other funcitons so they have the knowledge of it before...
    CustomComponent TryMakeComponent;
    CustomComponent TryGetComponent;

    // Helper funcitons in here only stay in this specific .cpp file
    Component* AddComponent(const std::string& componentName, GameObject& gameObject)
    {
        Component* newComponent = nullptr;
        if (componentName == "TransformComponent")
        {
            newComponent = gameObject.AddComponent<TransformComponent>();
        }
        else if (componentName == "CameraComponent")
        {
            newComponent = gameObject.AddComponent<CameraComponent>();
        }
        else if (componentName == "FPSCameraComponent")
        {
            newComponent = gameObject.AddComponent<FPSCameraComponent>();
        }
        else if (componentName == "MeshComponent")
        {
            newComponent = gameObject.AddComponent<MeshComponent>();
        }
        else if (componentName == "ModelComponent")
        {
            newComponent = gameObject.AddComponent<ModelComponent>();
        }
        else if (componentName == "AnimatorComponent")
        {
            newComponent = gameObject.AddComponent<AnimatorComponent>();
        }
        else if (componentName == "RigidBodyComponent")
        {
            newComponent = gameObject.AddComponent<RigidBodyComponent>();
        }
        else if (componentName == "SoundEventComponent")
        {
            newComponent = gameObject.AddComponent<SoundEventComponent>();
        }
        else if (componentName == "SoundBankComponent")
        {
            newComponent = gameObject.AddComponent<SoundBankComponent>();
        }
        else
        {
            newComponent = TryMakeComponent(componentName, gameObject);
        }

        ASSERT(newComponent != nullptr, "GameObjectFactory: Component type [%s] not found!", componentName.c_str());

        return newComponent;
    }

    Component* GetComponent(const std::string& componentName, GameObject& gameObject)
    {
        Component* component = nullptr;
        if (componentName == "TransformComponent")
        {
            component = gameObject.GetComponent<TransformComponent>();
        }
        else if (componentName == "CameraComponent")
        {
            component = gameObject.GetComponent<CameraComponent>();
        }
        else if (componentName == "FPSCameraComponent")
        {
            component = gameObject.GetComponent<FPSCameraComponent>();
        }
        else if (componentName == "MeshComponent")
        {
            component = gameObject.GetComponent<MeshComponent>();
        }
        else if (componentName == "ModelComponent")
        {
            component = gameObject.GetComponent<ModelComponent>();
        }
        else if (componentName == "AnimatorComponent")
        {
            component = gameObject.GetComponent<AnimatorComponent>();
        }
        else if (componentName == "RigidBodyComponent")
        {
            component = gameObject.GetComponent<RigidBodyComponent>();
        }
        else
        {
            component = TryGetComponent(componentName, gameObject);
        }

        ASSERT(component != nullptr, "GameObjectFactory: Component type [%s] not found!", componentName.c_str());
        return component;
    }
}

void GameObjectFactory::SetCustomMake(CustomComponent callback)
{
    TryMakeComponent = callback;
}

void GameObjectFactory::SetCustomGet(CustomComponent callback)
{
    TryGetComponent = callback;
}

void GameObjectFactory::Make(const std::filesystem::path& templatePath, GameObject& gameObject, GameWorld& gameWorld)
{
    FILE* file = nullptr;
    auto err = fopen_s(&file, templatePath.u8string().c_str(), "r");
    ASSERT(err == 0, "GameObjectFactory: Failed to open file %s", templatePath.u8string().c_str());

    char readBuffer[65536];
    rapidjson::FileReadStream readStream(file, readBuffer, sizeof(readBuffer));
    fclose(file);
    // Pass in a template, read it into the file buffer, close the file

    rapidjson::Document doc;
    doc.ParseStream(readStream); // Maps out all of the names and object types -> Allows us to do stuff w them.
    auto components = doc["Components"].GetObj();
    for (auto& component : components)
    {
        // TODO read data...
        Component* newComponent = AddComponent(component.name.GetString(), gameObject);
        if (newComponent != nullptr)
        {
            // Apply the jason value data
            newComponent->Deserialize(component.value);
        }
    }
}

void GameObjectFactory::OverrideDeserialize(const rapidjson::Value& value, GameObject& gameObject)
{
    if (value.HasMember("Components"))
    {
        auto components = value["Components"].GetObj();
        for (auto& component : components)
        {
            Component* ownedComponent = GetComponent(component.name.GetString(), gameObject);
            if (ownedComponent != nullptr)
            {
                ownedComponent->Deserialize(component.value);
            }
        }
    }
}