#include "Precompiled.h"
#include "GameObjectFactory.h"

using namespace IExeEngine;

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
    }
}
