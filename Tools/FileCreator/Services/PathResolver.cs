using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FileCreator.Services
{
    // look for a directory that contains a "Framework" subfolder.
    public static class PathResolver
    {
        private static readonly Dictionary<string, string> UmbrellaHeaderOverrides = new()
        {
            { "Math", "DWMath.h" }
        };


        /// Walks up the directory tree from the executable's location until
        /// it finds a folder containing a "Framework" subdirectory.
        public static string? FindEngineRoot()
        {
            var dir = new DirectoryInfo(AppDomain.CurrentDomain.BaseDirectory);

            while (dir != null)
            {
                if (Directory.Exists(Path.Combine(dir.FullName, "Framework")))
                    return dir.FullName;

                dir = dir.Parent;
            }

            return null;
        }

        // Returns the full path to a Framework project folder.
        public static string GetProjectPath(string engineRoot, string projectName)
            => Path.Combine(engineRoot, "Framework", projectName);

        // Returns the full path to the Inc folder of a Framework project.
        public static string GetIncPath(string engineRoot, string projectName)
            => Path.Combine(GetProjectPath(engineRoot, projectName), "Inc");

        // Returns the full path to the Src folder of a Framework project.
        public static string GetSrcPath(string engineRoot, string projectName)
            => Path.Combine(GetProjectPath(engineRoot, projectName), "Src");

        // Returns the filename of the umbrella header for a given project.
        public static string GetUmbrellaHeaderName(string projectName)
        {
            if (UmbrellaHeaderOverrides.TryGetValue(projectName, out string? overrideName))
                return overrideName;

            return $"{projectName}.h";
        }

        // Returns the full path to the umbrella header file for a project.
        public static string GetUmbrellaHeaderPath(string engineRoot, string projectName)
            => Path.Combine(GetIncPath(engineRoot, projectName), GetUmbrellaHeaderName(projectName));

        // Returns the full path to the .vcxproj file for a Framework project.
        public static string GetVcxprojPath(string engineRoot, string projectName)
            => Path.Combine(GetProjectPath(engineRoot, projectName), $"{projectName}.vcxproj");


        // Returns the full path to the .vcxproj.filters file for a Framework project.
        public static string GetVcxprojFiltersPath(string engineRoot, string projectName)
            => Path.Combine(GetProjectPath(engineRoot, projectName), $"{projectName}.vcxproj.filters");

        // Error checking
        // Validates that all expected folders and key files exist for a given project.
        public static List<string> ValidateProjectPaths(string engineRoot, string projectName)
        {
            var issues = new List<string>();

            string projectPath = GetProjectPath(engineRoot, projectName);
            string incPath = GetIncPath(engineRoot, projectName);
            string srcPath = GetSrcPath(engineRoot, projectName);
            string vcxproj = GetVcxprojPath(engineRoot, projectName);
            string filters = GetVcxprojFiltersPath(engineRoot, projectName);
            string umbrella = GetUmbrellaHeaderPath(engineRoot, projectName);

            if (!Directory.Exists(projectPath)) issues.Add($"Project folder not found: {projectPath}");
            if (!Directory.Exists(incPath)) issues.Add($"Inc folder not found: {incPath}");
            if (!Directory.Exists(srcPath)) issues.Add($"Src folder not found: {srcPath}");
            if (!File.Exists(vcxproj)) issues.Add($".vcxproj not found: {vcxproj}");
            if (!File.Exists(filters)) issues.Add($".vcxproj.filters not found: {filters}");
            if (!File.Exists(umbrella)) issues.Add($"Umbrella header not found: {umbrella}");

            return issues;
        }
    }
}
