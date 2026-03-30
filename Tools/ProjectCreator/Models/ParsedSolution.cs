using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ProjectCreator.Models
{
    // The complete parsed result of a .sln file.
    // Contains all the data needed to register a new project correctly.
    public class ParsedSolution
    {
        // All project and folder entries found in the solution.
        public List<SlnProjectEntry> AllEntries { get; init; } = new();

        // Only the solution folder entries (type GUID 2150E333-...).
        public List<SlnProjectEntry> SolutionFolders { get; init; } = new();

        // folders whose names start with "VGP".
        public List<SlnProjectEntry> VgpFolders { get; init; } = new();

        // Maps each solution folder GUID to the list of project entries nested inside it.
        // Built from the NestedProjects section of the .sln.
        // Key = parent folder GUID (uppercase), Value = list of child entries.
        public Dictionary<string, List<SlnProjectEntry>> ProjectsByFolder { get; init; } = new();

        // The GUID of the main engine project.
        // Used when generating the ProjectReference in new .vcxproj files.
        public string EngineProjectGuid { get; init; } = string.Empty;

        // The name of the engine .props file without extension.
        // Detected by scanning existing .vcxproj files for their props import path.
        public string PropsFileName { get; init; } = string.Empty;

        // The GUID of the engine's main .vcxproj project type.
        // Always {8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942} for C++ projects.
        public const string CppProjectTypeGuid = "{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}";

        // The solution folder type GUID.
        // Always {2150E333-8FDC-42A3-9474-1A3956D46DE8}.
        public const string FolderTypeGuid = "{2150E333-8FDC-42A3-9474-1A3956D46DE8}";
    }
}
