using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Text.RegularExpressions;
using ProjectCreator.Models;

// Writes a new project entry into an existing .sln file.
// Handles all three required insertion points:
//   1. Project(...) block in the project list
//   2. Configuration platform entries in ProjectConfigurationPlatforms
//   3. NestedProjects entry linking it to its VGP folder
namespace ProjectCreator.Services
{
    public static class SolutionWriterService
    {
        // The C++ project type GUID — always this value for .vcxproj projects
        private const string CppTypeGuid = "{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}";


        // Inserts the new project into the .sln file.
        // Reads the file, performs all three insertions, then writes it back.
        // Throws on any failure so the caller can handle and report cleanly.
        public static void Write(ProjectCreationRequest req)
        {
            string[] lines = File.ReadAllLines(req.SlnPath);

            lines = InsertProjectBlock(lines, req);
            lines = InsertConfigPlatformEntries(lines, req);
            lines = InsertNestedProjectEntry(lines, req);

            // Write back preserving Windows line endings to match
            // the existing .sln format exactly
            File.WriteAllLines(req.SlnPath, lines,
                new UTF8Encoding(true)); // SLN files use UTF-8 with BOM
        }

        // Step 1: Insert Project(...) block

        // Inserts the Project(...)...EndProject block into the project list.
        // Inserts it directly after the last existing project in the same
        // VGP folder, keeping the solution file organised by folder.
        // Falls back to inserting before the Global section if no sibling found.
        private static string[] InsertProjectBlock(
            string[] lines, ProjectCreationRequest req)
        {
            // Relative path as it appears in the .sln
            string relativePath =
                $@"{req.VgpFolderName}\{req.ProjectName}\{req.ProjectName}.vcxproj";

            var block = new[]
            {
                $"Project(\"{CppTypeGuid}\") = " +
                $"\"{req.ProjectName}\", " +
                $"\"{relativePath}\", " +
                $"\"{req.ProjectGuid}\"",
                "EndProject"
            };

            // Find the last EndProject line that belongs to a sibling in the
            // same VGP folder by scanning NestedProjects for the folder GUID
            int insertAfter = FindInsertionPointForProjectBlock(lines, req);

            return InsertLinesAfter(lines, insertAfter, block);
        }


        // Finds the best line index to insert the new Project block after.
        // Looks for the last EndProject of a sibling in the same VGP folder.
        // Falls back to the line before "Global" if no good spot found.
        private static int FindInsertionPointForProjectBlock(
            string[] lines, ProjectCreationRequest req)
        {
            // Collect all project GUIDs that are siblings in the same VGP folder
            // by scanning NestedProjects
            var siblingGuids = new HashSet<string>(StringComparer.OrdinalIgnoreCase);
            bool inNested = false;

            foreach (string line in lines)
            {
                if (line.Contains("GlobalSection(NestedProjects)"))
                { inNested = true; continue; }
                if (inNested && line.Contains("EndGlobalSection"))
                { inNested = false; continue; }

                if (!inNested) continue;

                // Line format:  \t\t{CHILD} = {PARENT}
                var m = Regex.Match(line,
                    @"(\{[^}]+\})\s*=\s*(\{[^}]+\})");
                if (!m.Success) continue;

                string parent = m.Groups[2].Value;
                if (parent.Equals(req.VgpFolderGuid,
                    StringComparison.OrdinalIgnoreCase))
                {
                    siblingGuids.Add(m.Groups[1].Value);
                }
            }

            // Walk forward through lines to find the last EndProject
            // that follows a Project line whose GUID is a sibling
            int lastSiblingEndProject = -1;
            bool inSiblingProject = false;

            for (int i = 0; i < lines.Length; i++)
            {
                string trimmed = lines[i].Trim();

                if (trimmed.StartsWith("Project("))
                {
                    var m = Regex.Match(trimmed,
                        @"""(\{[^}]+\})""$");
                    if (m.Success && siblingGuids.Contains(m.Groups[1].Value))
                        inSiblingProject = true;
                }

                if (inSiblingProject && trimmed == "EndProject")
                {
                    lastSiblingEndProject = i;
                    inSiblingProject = false;
                }
            }

            if (lastSiblingEndProject >= 0)
                return lastSiblingEndProject;

            // Fallback: insert before the Global section
            for (int i = 0; i < lines.Length; i++)
            {
                if (lines[i].TrimStart().StartsWith("Global"))
                    return i - 1;
            }

            return lines.Length - 1;
        }

        // Steep 2: Insert ProjectConfigurationPlatforms entries

        // Inserts the 12 configuration/platform mapping lines for the new project
        // into the ProjectConfigurationPlatforms GlobalSection.
        // Inserts them after the last existing entry in that section.
        private static string[] InsertConfigPlatformEntries(
            string[] lines, ProjectCreationRequest req)
        {
            string guid = req.ProjectGuid;

            // These 12 lines match exactly what Visual Studio generates for a
            // C++ project supporting all 6 solution configurations
            var entries = new[]
            {
                $"\t\t{guid}.Debug|Any CPU.ActiveCfg = Debug|x64",
                $"\t\t{guid}.Debug|Any CPU.Build.0 = Debug|x64",
                $"\t\t{guid}.Debug|x64.ActiveCfg = Debug|x64",
                $"\t\t{guid}.Debug|x64.Build.0 = Debug|x64",
                $"\t\t{guid}.Debug|x86.ActiveCfg = Debug|Win32",
                $"\t\t{guid}.Debug|x86.Build.0 = Debug|Win32",
                $"\t\t{guid}.Release|Any CPU.ActiveCfg = Release|x64",
                $"\t\t{guid}.Release|Any CPU.Build.0 = Release|x64",
                $"\t\t{guid}.Release|x64.ActiveCfg = Release|x64",
                $"\t\t{guid}.Release|x64.Build.0 = Release|x64",
                $"\t\t{guid}.Release|x86.ActiveCfg = Release|Win32",
                $"\t\t{guid}.Release|x86.Build.0 = Release|Win32",
            };

            // Find the EndGlobalSection that closes ProjectConfigurationPlatforms
            int insertBefore = -1;
            bool inSection = false;

            for (int i = 0; i < lines.Length; i++)
            {
                if (lines[i].Contains("GlobalSection(ProjectConfigurationPlatforms)"))
                { inSection = true; continue; }

                if (inSection && lines[i].Contains("EndGlobalSection"))
                {
                    insertBefore = i;
                    break;
                }
            }

            if (insertBefore < 0)
                throw new InvalidOperationException(
                    "Could not find ProjectConfigurationPlatforms section in .sln file.");

            return InsertLinesBefore(lines, insertBefore, entries);
        }

        // Step 3: Insert NestedProjects entry

        // Inserts the {ProjectGuid} = {VgpFolderGuid} line into NestedProjects,
        // placing it alongside existing siblings in the same VGP folder.
        private static string[] InsertNestedProjectEntry(
            string[] lines, ProjectCreationRequest req)
        {
            string entry =
                $"\t\t{req.ProjectGuid} = {req.VgpFolderGuid}";

            // Find the last NestedProjects entry for a sibling in the same folder
            // and insert after it — keeps folder members grouped together
            int insertAfter = FindNestedInsertionPoint(lines, req);

            return InsertLinesAfter(lines, insertAfter, new[] { entry });
        }

        // Finds the best line to insert the new NestedProjects entry after.
        // Looks for the last existing entry with the same parent VGP folder GUID.
        // Falls back to just before the EndGlobalSection of NestedProjects.
        private static int FindNestedInsertionPoint(
            string[] lines, ProjectCreationRequest req)
        {
            bool inSection = false;
            int lastSiblingLine = -1;
            int endSectionLine = -1;

            for (int i = 0; i < lines.Length; i++)
            {
                if (lines[i].Contains("GlobalSection(NestedProjects)"))
                { inSection = true; continue; }

                if (inSection && lines[i].Contains("EndGlobalSection"))
                {
                    endSectionLine = i;
                    inSection = false;
                    break;
                }

                if (!inSection) continue;

                var m = Regex.Match(lines[i],
                    @"(\{[^}]+\})\s*=\s*(\{[^}]+\})");
                if (!m.Success) continue;

                string parent = m.Groups[2].Value;
                if (parent.Equals(req.VgpFolderGuid,
                    StringComparison.OrdinalIgnoreCase))
                {
                    lastSiblingLine = i;
                }
            }

            if (lastSiblingLine >= 0)
                return lastSiblingLine;

            // Fallback: insert just before EndGlobalSection
            if (endSectionLine >= 0)
                return endSectionLine - 1;

            throw new InvalidOperationException(
                "Could not find NestedProjects section in .sln file.");
        }

        // Generic line insertion helpers

        private static string[] InsertLinesAfter(
            string[] original, int index, string[] newLines)
        {
            var result = new List<string>(original);
            result.InsertRange(index + 1, newLines);
            return result.ToArray();
        }

        private static string[] InsertLinesBefore(
            string[] original, int index, string[] newLines)
        {
            var result = new List<string>(original);
            result.InsertRange(index, newLines);
            return result.ToArray();
        }
    }
}