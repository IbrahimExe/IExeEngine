#pragma once

namespace IExeEngine::Graphics
{
    struct Model;


    namespace ModelIO
    {
        void SaveModel(std::filesystem::path filePath, const Model& model);
        void LoadModel(std::filesystem::path filePath, Model& model);

        void SaveMaterial(std::filesystem::path filePath, const Model& material);
        void LoadMaterial(std::filesystem::path filePath, Model& material);
    }
}
