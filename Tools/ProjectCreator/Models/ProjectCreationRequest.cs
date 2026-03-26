using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ProjectCreator.Models
{
    // Describes a complete project creation request.
    // Passed between all services so they share a single source.
    public class ProjectCreationRequest
    {
        public string EngineName { get; init; } = string.Empty;

        // The VGP folder name.
        public string VgpFolderName { get; init; } = string.Empty;


        // The GUID of the VGP solution folder in the .sln.
        public string VgpFolderGuid { get; init; } = string.Empty;

        /// The new project name.
        public string ProjectName { get; init; } = string.Empty;

        // A freshly generated GUID for the new project.
        // Generated once and reused across all services.
        public string ProjectGuid { get; init; } = string.Empty;

        // The GUID of the engine's main .vcxproj for the ProjectReference.
        public string EngineProjectGuid { get; init; } = string.Empty;

        // The props file name without extension.
        public string PropsFileName { get; init; } = string.Empty;

        // Absolute path to the engine root folder.
        public string EngineRoot { get; init; } = string.Empty;

        // Absolute path to the new project folder.
        public string ProjectFolderPath { get; init; } = string.Empty;

        // Absolute path to the .sln file.
        public string SlnPath { get; init; } = string.Empty;

        // copy starter files from a previous project?
        public bool CopyStarterFiles { get; init; }

        // The project to copy starter files from.
        // Null if CopyStarterFiles is false.
        public SlnProjectEntry? SourceProject { get; init; }

        // The display name used in WinMain appName e.g. "Hello New Project".
        // Derived from the project name by stripping the number prefix.
        public string AppDisplayName { get; init; } = string.Empty;
    }
}