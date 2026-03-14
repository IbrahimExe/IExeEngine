// Runs validation checks before file creation begins.
// All issues are shown in a UI list so that a user is 
// clearly informed of any problems that need fixing before they can proceed.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace FileCreator.Services
{
    public static class ValidationService
    {
        // Validates that the requested file creation operation is safe to proceed.
        // Returns a list of error messages
        public static List<string> Validate(
            string engineRoot,
            string projectName,
            string fileName,
            bool includeCpp)
        {
            var issues = new List<string>();

            // 1. Validate the file name is a legal C++ identifier
            if (string.IsNullOrWhiteSpace(fileName))
            {
                issues.Add("File name cannot be empty.");
                return issues; // No point continuing
            }

            if (!IsValidIdentifier(fileName))
            {
                issues.Add($"'{fileName}' is not a valid file name. " +
                            "Use only letters, numbers and underscores, " +
                            "and do not start with a number.");
            }

            // 2. Validate that the expected project paths exist on disk
            List<string> pathIssues = PathResolver.ValidateProjectPaths(engineRoot, projectName);
            issues.AddRange(pathIssues);

            // If basic paths are broken, skip file-existence checks
            if (issues.Count > 0)
                return issues;

            // 3. Warn if files already exist (would be overwritten)
            string incPath = PathResolver.GetIncPath(engineRoot, projectName);
            string srcPath = PathResolver.GetSrcPath(engineRoot, projectName);

            string headerPath = Path.Combine(incPath, $"{fileName}.h");
            string cppPath = Path.Combine(srcPath, $"{fileName}.cpp");

            if (File.Exists(headerPath))
                issues.Add($"Header file already exists: {headerPath}");

            if (includeCpp && File.Exists(cppPath))
                issues.Add($"Source file already exists: {cppPath}");

            return issues;
        }

        // Returns true if the name is a valid C++ identifier:
        // letters, digits and underscores only, not starting with a digit.
        private static bool IsValidIdentifier(string name)
        {
            if (string.IsNullOrEmpty(name)) return false;
            if (char.IsDigit(name[0])) return false;

            foreach (char c in name)
            {
                if (!char.IsLetterOrDigit(c) && c != '_')
                    return false;
            }

            return true;
        }
    }
}