using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ProjectCreator.Models
{
    // Represents a single project entry parsed from the .sln file.
    public class SlnProjectEntry
    {
        public string TypeGuid { get; init; } = string.Empty;

        // The display name of the project or folder e.g. "21_HelloProceduralAnimation"
        public string Name { get; init; } = string.Empty;

        // The relative path to the .vcxproj file, or just the folder name for solution folders.
        public string RelativePath { get; init; } = string.Empty;

        // The unique project GUID e.g. {F44C114B-...}
        public string Guid { get; init; } = string.Empty;

        // True if this is a solution folder (type GUID 2150E333-...) rather than a real project.
        public bool IsSolutionFolder =>
            TypeGuid.Equals("{2150E333-8FDC-42A3-9474-1A3956D46DE8}",
                            StringComparison.OrdinalIgnoreCase);
    }
}