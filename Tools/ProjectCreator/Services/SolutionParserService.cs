using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Text.RegularExpressions;
using System.Xml;
using ProjectCreator.Models;

// Need a way to read and parse tehe .sln files into a Parsed Solution
// ParsedSolution
namespace ProjectCreator.Services
{
    public static class SolutionParserService
    {
        // Matches:  Project("{TYPE}") = "Name", "Path", "{GUID}"
        private static readonly Regex ProjectLineRegex = new(
            @"Project\(""(\{[^}]+\})""\)\s*=\s*""([^""]+)""\s*,\s*""([^""]+)""\s*,\s*""(\{[^}]+\})""",
            RegexOptions.Compiled);

        // Matches a line inside NestedProjects:  {CHILD} = {PARENT}
        private static readonly Regex NestedProjectRegex = new(
            @"\s*(\{[^}]+\})\s*=\s*(\{[^}]+\})",
            RegexOptions.Compiled);

        // Parses the .sln file at the given path and returns a fully populated ParsedSolution.
        // Throws if the file cannot be read or is malformed.
        public static ParsedSolution Parse(string slnPath, string engineRoot)
        {
            string[] lines = File.ReadAllLines(slnPath);

            var allEntries = new List<SlnProjectEntry>();
            var nestedMap = new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);

            // collect all Project(...) entries
            foreach (string line in lines)
            {
                Match m = ProjectLineRegex.Match(line);
                if (!m.Success) continue;

                allEntries.Add(new SlnProjectEntry
                {
                    TypeGuid = m.Groups[1].Value.ToUpper(),
                    Name = m.Groups[2].Value,
                    RelativePath = m.Groups[3].Value,
                    Guid = m.Groups[4].Value.ToUpper()
                });
            }

            // collect NestedProjects mappings
            bool inNestedSection = false;

            foreach (string line in lines)
            {
                if (line.Contains("GlobalSection(NestedProjects)"))
                {
                    inNestedSection = true;
                    continue;
                }

                if (inNestedSection && line.Contains("EndGlobalSection"))
                {
                    inNestedSection = false;
                    continue;
                }

                if (!inNestedSection) continue;

                Match m = NestedProjectRegex.Match(line);
                if (!m.Success) continue;

                string childGuid = m.Groups[1].Value.ToUpper();
                string parentGuid = m.Groups[2].Value.ToUpper();
                nestedMap[childGuid] = parentGuid;
            }

            // Build lookup by GUID
            var byGuid = allEntries.ToDictionary(
                e => e.Guid,
                e => e,
                StringComparer.OrdinalIgnoreCase);

            // Identify solution folders and VGP folders
            var solutionFolders = allEntries
                .Where(e => e.IsSolutionFolder)
                .ToList();

            var vgpFolders = solutionFolders
                .Where(e => e.Name.StartsWith("VGP", StringComparison.OrdinalIgnoreCase))
                .OrderBy(e => e.Name)
                .ToList();

            // Build ProjectsByFolder map
            // For each VGP folder, find all C++ projects nested inside it
            var projectsByFolder = new Dictionary<string, List<SlnProjectEntry>>(
                StringComparer.OrdinalIgnoreCase);

            foreach (SlnProjectEntry folder in vgpFolders)
            {
                var children = new List<SlnProjectEntry>();

                foreach (var kvp in nestedMap)
                {
                    string childGuid = kvp.Key;
                    string parentGuid = kvp.Value;

                    if (!parentGuid.Equals(folder.Guid, StringComparison.OrdinalIgnoreCase))
                        continue;

                    if (!byGuid.TryGetValue(childGuid, out SlnProjectEntry? child))
                        continue;

                    // Only include real C++ projects, not sub folders
                    if (!child.IsSolutionFolder)
                        children.Add(child);
                }

                // Sort by name so they appear in numeric order in the dropdown
                children.Sort((a, b) => string.Compare(a.Name, b.Name,
                    StringComparison.OrdinalIgnoreCase));

                projectsByFolder[folder.Guid] = children;
            }

            //Find engine project GUID
            // The engine project is the C++ project whose .vcxproj path contains
            // "Engine\" and whose name matches the engine folder structure
            string engineGuid = FindEngineProjectGuid(allEntries, engineRoot);

            //Detect props file name
            // Scan the first few existing semester project .vcxproj files to find
            // the props import line and extract the filename
            string propsName = DetectPropsFileName(allEntries, engineRoot, slnPath);

            return new ParsedSolution
            {
                AllEntries = allEntries,
                SolutionFolders = solutionFolders,
                VgpFolders = vgpFolders,
                ProjectsByFolder = projectsByFolder,
                EngineProjectGuid = engineGuid,
                PropsFileName = propsName
            };
        }

        // ─── Private helpers ─────────────────────────────────────────────────────

        // Finds the GUID of the main engine .vcxproj (e.g. IExeEngine.vcxproj).
        // Identifies it as the C++ project whose relative path contains "Engine\"
        // and whose vcxproj name matches the parent folder name.
        private static string FindEngineProjectGuid(
            List<SlnProjectEntry> entries,
            string engineRoot)
        {
            foreach (SlnProjectEntry entry in entries)
            {
                if (entry.IsSolutionFolder) continue;

                // Engine project path looks like: Engine\IExeEngine\IExeEngine.vcxproj
                string normalised = entry.RelativePath.Replace('/', '\\');
                if (!normalised.StartsWith("Engine\\", StringComparison.OrdinalIgnoreCase))
                    continue;

                // Confirm the vcxproj actually exists on disk
                string fullPath = System.IO.Path.Combine(engineRoot, normalised);
                if (File.Exists(fullPath))
                    return entry.Guid;
            }

            return string.Empty;
        }

        // Scans existing semester project .vcxproj files to find the props import
        // line and extract the props filename.
        // Returns the engine name portion from
        // "..\..\VSProps\IExeEngine.props"
        private static string DetectPropsFileName(
            List<SlnProjectEntry> entries,
            string engineRoot,
            string slnPath)
        {
            string slnDir = System.IO.Path.GetDirectoryName(slnPath) ?? engineRoot;

            // Regex to find the props import path inside a .vcxproj
            var propsRegex = new Regex(
                @"VSProps\\([^\\""]+)\.props",
                RegexOptions.IgnoreCase | RegexOptions.Compiled);

            foreach (SlnProjectEntry entry in entries)
            {
                if (entry.IsSolutionFolder) continue;
                if (!entry.RelativePath.EndsWith(".vcxproj",
                    StringComparison.OrdinalIgnoreCase)) continue;

                // Only look at semester projects (paths starting with VGP)
                if (!entry.RelativePath.StartsWith("VGP",
                    StringComparison.OrdinalIgnoreCase)) continue;

                string fullPath = System.IO.Path.Combine(slnDir, entry.RelativePath);
                if (!File.Exists(fullPath)) continue;

                try
                {
                    string content = File.ReadAllText(fullPath);
                    Match m = propsRegex.Match(content);
                    if (m.Success)
                        return m.Groups[1].Value;
                }
                catch
                {
                    // Skip unreadable files and try the next one
                }
            }

            return string.Empty;
        }
    }
}