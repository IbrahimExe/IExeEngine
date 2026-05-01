#include "Precompiled.h"
#include "GameObjectFactory.h"
#include "Component.h"
#include "GameObject.h"
// components includes we've made 
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "FPSCameraComponent.h"
#include "MeshComponent.h"

using namespace IExeEngine;

namespace
{
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

        ASSERT(newComponent != nullptr, "GameObjectFactory: Component type [%s] not found!", componentName.c_str());

        return newComponent;
    }
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
