using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace FileCreator.Services
{
    // generating .h and .cpp files on disk
    public static class FileGeneratorService
    {

        // Creates a .h file in the project's Inc folder
        public static string CreateHeaderFile(
            string incPath,
            string fileName,
            string engineName,
            string projectName)
        {
            string filePath = Path.Combine(incPath, $"{fileName}.h");

            string content =
                $"#pragma once\n" +
                $"\n" +
                $"namespace {engineName}::{projectName}\n" +
                $"{{\n" +
                $"\n" +
                $"}}";

            File.WriteAllText(filePath, content);
            return filePath;
        }

        // Creates a .cpp file in the project's Src folder.
        public static string CreateCppFile(
            string srcPath,
            string fileName,
            string engineName,
            string projectName)
        {
            string filePath = Path.Combine(srcPath, $"{fileName}.cpp");

            string content =
                $"#include \"Precompiled.h\"\n" +
                $"#include \"{fileName}.h\"\n" +
                $"\n" +
                $"using namespace {engineName};\n" +
                $"using namespace {engineName}::{projectName};";

            File.WriteAllText(filePath, content);
            return filePath;
        }
    }
}
