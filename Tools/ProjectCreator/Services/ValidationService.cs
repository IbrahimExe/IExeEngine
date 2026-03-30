using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Text.RegularExpressions;
using ProjectCreator.Models;

namespace ProjectCreator.Services
{
    // Runs all pre-flight checks before any files are touched.
    public static class ValidationService
    {
        public static List<string> Validate(
            string engineRoot,
            string engineName,
            string vgpFolderName,
            string projectName,
            string slnPath)
        {
            var issues = new List<string>();

            // Project name must be a valid identifier
            if (string.IsNullOrWhiteSpace(projectName))
            {
                issues.Add("Project name cannot be empty.");
                return issues;
            }

            // Allow digits, letters, underscores and hyphens
            if (!Regex.IsMatch(projectName, @"^[\w\-]+$"))
            {
                issues.Add($"'{projectName}' contains invalid characters. " +
                            "Use only letters, numbers, underscores and hyphens.");
            }

            // Check all required paths exist
            issues.AddRange(PathResolver.Validate(
                engineRoot, engineName, vgpFolderName, projectName, slnPath));

            return issues;
        }
    }
}


//if(!Regex.IsMatch(projectName, @" "))
//            {
//    issues.Add($"'{projectName}' contains invalid characters. " +
//                "Use only abc123_-");
//}