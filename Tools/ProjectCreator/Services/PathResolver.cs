using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace ProjectCreator.Services
{
    /// <summary>
    /// Resolves all on-disk paths needed by the tool.
    /// Finds the engine root by walking up from the executable until
    /// a directory containing both a "Framework" folder and a ".sln" file is found.
    /// </summary>
    public static class PathResolver
    {
        /// <summary>
        /// Walks up the directory tree from the executable to find the engine root.
        /// The engine root is the directory that contains a Framework subfolder
        /// and at least one .sln file.
        /// Returns null if not found.
        /// </summary>
        public static string? FindEngineRoot()
        {
            var dir = new DirectoryInfo(AppDomain.CurrentDomain.BaseDirectory);

            while (dir != null)
            {
                bool hasFramework = Directory.Exists(
                    System.IO.Path.Combine(dir.FullName, "Framework"));
                bool hasSln = dir.GetFiles("*.sln").Length > 0;

                if (hasFramework && hasSln)
                    return dir.FullName;

                dir = dir.Parent;
            }

            return null;
        }

        /// <summary>
        /// Returns the full path to the .sln file at the engine root.
        /// Returns null if none found.
        /// </summary>
        public static string? FindSlnFile(string engineRoot)
        {
            string[] slnFiles = Directory.GetFiles(engineRoot, "*.sln");
            return slnFiles.Length > 0 ? slnFiles[0] : null;
        }

        /// <summary>
        /// Returns the full path to the props file in VSProps/.
        /// e.g. .../IExeEngine/VSProps/IExeEngine.props
        /// </summary>
        public static string GetPropsPath(string engineRoot, string engineName)
            => System.IO.Path.Combine(engineRoot, "VSProps", $"{engineName}.props");

        /// <summary>
        /// Returns the full path to a VGP semester folder.
        /// e.g. .../IExeEngine/VGP334
        /// </summary>
        public static string GetVgpFolderPath(string engineRoot, string vgpFolderName)
            => System.IO.Path.Combine(engineRoot, vgpFolderName);

        /// <summary>
        /// Returns the full path to the new project folder.
        /// e.g. .../IExeEngine/VGP334/22_HelloNewProject
        /// </summary>
        public static string GetNewProjectPath(string engineRoot, string vgpFolderName, string projectName)
            => System.IO.Path.Combine(engineRoot, vgpFolderName, projectName);

        /// <summary>
        /// Returns the props import path as it appears inside the .vcxproj —
        /// always two levels up from the project folder to the VSProps folder.
        /// e.g. "..\..\VSProps\IExeEngine.props"
        /// </summary>
        public static string GetPropsRelativePath(string engineName)
            => $@"..\..\VSProps\{engineName}.props";

        /// <summary>
        /// Validates all paths and pre-conditions before project creation begins.
        /// Returns a list of issues — empty means all clear.
        /// </summary>
        public static List<string> Validate(
            string engineRoot,
            string engineName,
            string vgpFolderName,
            string projectName,
            string slnPath)
        {
            var issues = new List<string>();

            string vgpPath = GetVgpFolderPath(engineRoot, vgpFolderName);
            string projectPath = GetNewProjectPath(engineRoot, vgpFolderName, projectName);
            string propsPath = GetPropsPath(engineRoot, engineName);

            if (!Directory.Exists(vgpPath))
                issues.Add($"VGP folder not found: {vgpPath}");

            if (!File.Exists(propsPath))
                issues.Add($"Props file not found: {propsPath}");

            if (!File.Exists(slnPath))
                issues.Add($"Solution file not found: {slnPath}");

            if (Directory.Exists(projectPath))
                issues.Add($"A project folder with this name already exists: {projectPath}");

            return issues;
        }
    }
}