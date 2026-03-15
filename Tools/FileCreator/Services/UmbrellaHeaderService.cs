using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace FileCreator.Services
{
    // Handles injecting #include entries into a project's umbrella header file.
    public static class UmbrellaHeaderService
    {
        // Adds an #include line for the new header file into the umbrella header.
        // The line is inserted after the last existing #include in the file,
        // keeping includes grouped together cleanly.
        public static bool InjectInclude(string umbrellaHeaderPath, string newFileName)
        {
            string includeStatement = $"#include \"{newFileName}.h\"";

            string[] lines = File.ReadAllLines(umbrellaHeaderPath);

            // check if this include already exists
            foreach (string line in lines)
            {
                if (line.Trim() == includeStatement)
                    return false;
            }

            // index of the last #include line in the file
            int lastIncludeIndex = -1;
            for (int i = 0; i < lines.Length; i++)
            {
                if (lines[i].TrimStart().StartsWith("#include"))
                    lastIncludeIndex = i;
            }

            List<string> newLines = new(lines);

            if (lastIncludeIndex >= 0)
            {
                // Insert after the last #include
                newLines.Insert(lastIncludeIndex + 1, includeStatement);
            }
            else
            {
                // No existing includes found therefore append at end of file
                newLines.Add(string.Empty);
                newLines.Add(includeStatement);
            }

            File.WriteAllLines(umbrellaHeaderPath, newLines);
            return true;
        }

        /// Removes the #include's 
        public static bool RemoveInclude(string umbrellaHeaderPath, string fileName)
        {
            string includeStatement = $"#include \"{fileName}.h\"";

            string[] lines = File.ReadAllLines(umbrellaHeaderPath);

            // Check if the line actually exists before trying to remove it
            int index = Array.FindIndex(lines, l => l.Trim() == includeStatement);

            if (index < 0)
                return false;

            List<string> newLines = new(lines);
            newLines.RemoveAt(index);

            File.WriteAllLines(umbrellaHeaderPath, newLines);
            return true;
        }
    }
}
