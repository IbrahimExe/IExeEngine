using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FileCreator.Models
{
    // Records the details of a successful file creation operation
    // so it can be fully reversed by the undo system
    public class CreationRecord
    {
        public string ProjectName { get; init; } = string.Empty;
        public string FileName { get; init; } = string.Empty;
        public string HeaderPath { get; init; } = string.Empty;
        public string? CppPath { get; init; } // null if no .cpp was created
        public string UmbrellaPath { get; init; } = string.Empty;
        public string VcxprojPath { get; init; } = string.Empty;
        public string FiltersPath { get; init; } = string.Empty;
        public bool HadCpp { get; init; }
    }
}
