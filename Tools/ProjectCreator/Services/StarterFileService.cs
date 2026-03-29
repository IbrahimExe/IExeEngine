using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Text.RegularExpressions;
using ProjectCreator.Models;

// Copies GameState.h, GameState.cpp and WinMain.cpp from a source project
// into the new project folder, then patches the appName in WinMain.cpp.
namespace ProjectCreator.Services
{
    public static class StarterFileService
    {
        private static readonly string[] FilesToCopy =
        {
            "GameState.h",
            "GameState.cpp",
            "WinMain.cpp"
        };

        // Copies the three starter files and patches WinMain.cpp.
        // Returns a list of log messages describing what was done.
        public static List<string> CopyAndPatch(
            ProjectCreationRequest req,
            string sourceProjectFolderPath)
        {
            var log = new List<string>();

            foreach (string fileName in FilesToCopy)
            {
                string sourcePath = Path.Combine(sourceProjectFolderPath, fileName);
                string destPath = Path.Combine(req.ProjectFolderPath, fileName);

                if (!File.Exists(sourcePath))
                {
                    log.Add($"[!!] {fileName} not found in source project so it has been skipped.");
                    continue;
                }

                File.Copy(sourcePath, destPath, overwrite: true);
                log.Add($"Copied {fileName}");
            }

            // Patch the appName in WinMain.cpp
            string winMainPath = Path.Combine(req.ProjectFolderPath, "WinMain.cpp");
            if (File.Exists(winMainPath))
            {
                bool patched = PatchAppName(winMainPath, req.AppDisplayName);
                log.Add(patched
                    ? $"Patched appName to \"{req.AppDisplayName}\""
                    : "[!!] appName line not found in WinMain.cpp — patch skipped.");
            }

            return log;
        }

        // Replaces the appName line in WinMain.cpp with the new display name.
        // Handles both L"..." wide string format and "..." narrow string format.
        private static bool PatchAppName(string winMainPath, string displayName)
        {
            string content = File.ReadAllText(winMainPath);

            //config.appName = L"anything"; or config.appName = "anything";
            var regex = new Regex(
                @"config\.appName\s*=\s*L?""[^""]*""\s*;",
                RegexOptions.Compiled);

            if (!regex.IsMatch(content))
                return false;

            string patched = regex.Replace(
                content,
                $@"config.appName = L""{displayName}"";");

            File.WriteAllText(winMainPath, patched);
            return true;
        }

        // Returns the on-disk folder path of a source project given its
        // .sln relative path
        public static string GetSourceProjectFolder(
            string engineRoot,
            SlnProjectEntry sourceProject)
        {
            // The relative path in the .sln points to the .vcxproj file
            // need the containing folder
            string vcxprojRelative = sourceProject.RelativePath
                .Replace('/', Path.DirectorySeparatorChar)
                .Replace('\\', Path.DirectorySeparatorChar);

            string vcxprojFull = Path.Combine(engineRoot, vcxprojRelative);
            return Path.GetDirectoryName(vcxprojFull) ?? engineRoot;
        }

        // Derives a human-readable display name from the project name.
        // Strips the leading number and underscore, then splits on capitals.
        public static string DeriveDisplayName(string projectName)
        {
            // Strip leading digits and underscore e.g. "22_"
            string stripped = Regex.Replace(projectName, @"^\d+_", "");

            // Remove any non-alphanumeric characters except spaces
            stripped = Regex.Replace(stripped, @"[^a-zA-Z0-9]", "");
            // i.e: "22_HelloNewProject" -> "Hello New Project"
            //      "20_HelloEvents&Audio" -> "Hello Events Audio"

            // Split on capital letters to insert spaces
            string spaced = Regex.Replace(stripped, @"(?<!^)(?=[A-Z])", " ");

            return spaced.Trim();
        }


        // Checks which of the three starter files actually exist in the source project.
        // Returns a list of any files that are missing so the UI can warn the user.
        public static List<string> CheckMissingFiles(string sourceProjectFolderPath)
        {
            var missing = new List<string>();

            foreach (string fileName in FilesToCopy)
            {
                string path = Path.Combine(sourceProjectFolderPath, fileName);
                if (!File.Exists(path))
                    missing.Add(fileName);
            }

            return missing;
        }
    }
}