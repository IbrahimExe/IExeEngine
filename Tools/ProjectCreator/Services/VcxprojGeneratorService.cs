using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Xml;
using ProjectCreator.Models;

// Generates a .vcxproj file for a new semester project from scratch.
// The output matches the structure of existing semester projects exactly.
namespace ProjectCreator.Services
{
    public static class VcxprojGeneratorService
    {
        private const string MsBuildNs = "http://schemas.microsoft.com/developer/msbuild/2003";

        // Generates and writes the .vcxproj file to disk.
        // Returns the full path of the written file
        public static string Generate(ProjectCreationRequest req)
        {
            string outputPath = System.IO.Path.Combine(
                req.ProjectFolderPath, $"{req.ProjectName}.vcxproj");

            // Strip curly braces from the GUID for the RootNamespace value
            string rootNamespace = "My" + string.Concat(
                req.ProjectName.Split(
                    new[] { '_', '-', ' ' },
                    StringSplitOptions.RemoveEmptyEntries));

            // The engine GUID without curly braces for the ProjectReference element
            string engineGuidNoBraces = req.EngineProjectGuid
                .Trim('{', '}').ToLower();

            // Props relative path
            string propsPath = $@"..\..\VSProps\{req.PropsFileName}.props";

            var settings = new XmlWriterSettings
            {
                Indent = true,
                IndentChars = "  ",
                Encoding = new System.Text.UTF8Encoding(false),
                OmitXmlDeclaration = false
            };

            using XmlWriter w = XmlWriter.Create(outputPath, settings);

            w.WriteStartDocument();
            w.WriteStartElement("Project", MsBuildNs);
            w.WriteAttributeString("DefaultTargets", "Build");

            // ProjectConfigurations
            WriteItemGroup(w, "ProjectConfigurations", () =>
            {
                foreach (var (cfg, plat) in new[]
                {
                    ("Debug",   "Win32"),
                    ("Release", "Win32"),
                    ("Debug",   "x64"),
                    ("Release", "x64")
                })
                {
                    w.WriteStartElement("ProjectConfiguration", MsBuildNs);
                    w.WriteAttributeString("Include", $"{cfg}|{plat}");
                    WriteElement(w, "Configuration", cfg);
                    WriteElement(w, "Platform", plat);
                    w.WriteEndElement();
                }
            });

            // Globals
            WritePropertyGroup(w, "Globals", () =>
            {
                WriteElement(w, "VCProjectVersion", "17.0");
                WriteElement(w, "Keyword", "Win32Proj");
                WriteElement(w, "ProjectGuid", req.ProjectGuid);
                WriteElement(w, "RootNamespace", rootNamespace);
                WriteElement(w, "WindowsTargetPlatformVersion", "10.0");
            });

            // Default props import
            w.WriteStartElement("Import", MsBuildNs);
            w.WriteAttributeString("Project", @"$(VCTargetsPath)\Microsoft.Cpp.Default.props");
            w.WriteEndElement();

            // Configuration PropertyGroups
            WriteConfigPropertyGroup(w, "Debug", "Win32", false);
            WriteConfigPropertyGroup(w, "Release", "Win32", true);
            WriteConfigPropertyGroup(w, "Debug", "x64", false);
            WriteConfigPropertyGroup(w, "Release", "x64", true);

            // Microsoft.Cpp.props import
            w.WriteStartElement("Import", MsBuildNs);
            w.WriteAttributeString("Project", @"$(VCTargetsPath)\Microsoft.Cpp.props");
            w.WriteEndElement();

            // ExtensionSettings
            WriteImportGroup(w, "ExtensionSettings", null);

            // Shared
            WriteImportGroup(w, "Shared", null);

            // PropertySheets (one per config/platform)
            foreach (var (cfg, plat) in new[]
            {
                ("Debug",   "Win32"),
                ("Release", "Win32"),
                ("Debug",   "x64"),
                ("Release", "x64")
            })
            {
                w.WriteStartElement("ImportGroup", MsBuildNs);
                w.WriteAttributeString("Label", "PropertySheets");
                w.WriteAttributeString("Condition",
                    $"'$(Configuration)|$(Platform)'=='{cfg}|{plat}'");

                // User props
                w.WriteStartElement("Import", MsBuildNs);
                w.WriteAttributeString("Project",
                    @"$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props");
                w.WriteAttributeString("Condition",
                    @"exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')");
                w.WriteAttributeString("Label", "LocalAppDataPlatform");
                w.WriteEndElement();

                // Engine props
                w.WriteStartElement("Import", MsBuildNs);
                w.WriteAttributeString("Project", propsPath);
                w.WriteEndElement();

                w.WriteEndElement(); // ImportGroup
            }

            // UserMacros
            WritePropertyGroup(w, "UserMacros", null);

            // ItemDefinitionGroups (compiler + linker settings)
            WriteItemDefinitionGroup(w, "Debug", "Win32", false);
            WriteItemDefinitionGroup(w, "Release", "Win32", true);
            WriteItemDefinitionGroup(w, "Debug", "x64", false);
            WriteItemDefinitionGroup(w, "Release", "x64", true);

            // Project reference to engine
            WriteItemGroup(w, null, () =>
            {
                w.WriteStartElement("ProjectReference", MsBuildNs);
                w.WriteAttributeString("Include",
                    $@"..\..\Engine\{req.EngineName}\{req.EngineName}.vcxproj");
                WriteElement(w, "Project", $"{{{engineGuidNoBraces}}}");
                w.WriteEndElement();
            });

            //Source files
            WriteItemGroup(w, null, () =>
            {
                w.WriteStartElement("ClCompile", MsBuildNs);
                w.WriteAttributeString("Include", "GameState.cpp");
                w.WriteEndElement();

                w.WriteStartElement("ClCompile", MsBuildNs);
                w.WriteAttributeString("Include", "WinMain.cpp");
                w.WriteEndElement();
            });

            // Header files
            WriteItemGroup(w, null, () =>
            {
                w.WriteStartElement("ClInclude", MsBuildNs);
                w.WriteAttributeString("Include", "GameState.h");
                w.WriteEndElement();
            });

            // Microsoft.Cpp.targets import
            w.WriteStartElement("Import", MsBuildNs);
            w.WriteAttributeString("Project", @"$(VCTargetsPath)\Microsoft.Cpp.targets");
            w.WriteEndElement();

            WriteImportGroup(w, "ExtensionTargets", null);

            w.WriteEndElement(); // Project
            w.WriteEndDocument();

            return outputPath;
        }

        // ─── Private XML helpers ─────────────────────────────────────────────────

        private static void WriteConfigPropertyGroup(
            XmlWriter w, string cfg, string plat, bool isRelease)
        {
            w.WriteStartElement("PropertyGroup", MsBuildNs);
            w.WriteAttributeString("Condition",
                $"'$(Configuration)|$(Platform)'=='{cfg}|{plat}'");
            w.WriteAttributeString("Label", "Configuration");

            WriteElement(w, "ConfigurationType", "Application");
            WriteElement(w, "UseDebugLibraries", isRelease ? "false" : "true");
            WriteElement(w, "PlatformToolset", "v143");

            if (isRelease)
                WriteElement(w, "WholeProgramOptimization", "true");

            WriteElement(w, "CharacterSet", "Unicode");
            w.WriteEndElement();
        }

        private static void WriteItemDefinitionGroup(
            XmlWriter w, string cfg, string plat, bool isRelease)
        {
            string preprocessor = plat == "Win32"
                ? (isRelease
                    ? "WIN32;NDEBUG;_WINDOWS;%(PreprocessorDefinitions)"
                    : "WIN32;_DEBUG;_WINDOWS;%(PreprocessorDefinitions)")
                : (isRelease
                    ? "NDEBUG;_WINDOWS;%(PreprocessorDefinitions)"
                    : "_DEBUG;_WINDOWS;%(PreprocessorDefinitions)");

            w.WriteStartElement("ItemDefinitionGroup", MsBuildNs);
            w.WriteAttributeString("Condition",
                $"'$(Configuration)|$(Platform)'=='{cfg}|{plat}'");

            // ClCompile
            w.WriteStartElement("ClCompile", MsBuildNs);
            WriteElement(w, "WarningLevel", "Level3");

            if (isRelease)
            {
                WriteElement(w, "FunctionLevelLinking", "true");
                WriteElement(w, "IntrinsicFunctions", "true");
            }

            WriteElement(w, "SDLCheck", "true");
            WriteElement(w, "PreprocessorDefinitions", preprocessor);
            WriteElement(w, "ConformanceMode", "true");
            w.WriteEndElement(); // ClCompile

            // Link
            w.WriteStartElement("Link", MsBuildNs);
            WriteElement(w, "SubSystem", "Windows");

            if (isRelease)
            {
                WriteElement(w, "EnableCOMDATFolding", "true");
                WriteElement(w, "OptimizeReferences", "true");
            }

            WriteElement(w, "GenerateDebugInformation", "true");
            w.WriteEndElement(); // Link

            w.WriteEndElement(); // ItemDefinitionGroup
        }

        private static void WriteItemGroup(
            XmlWriter w, string? label, Action? content)
        {
            w.WriteStartElement("ItemGroup", MsBuildNs);
            if (label != null)
                w.WriteAttributeString("Label", label);
            content?.Invoke();
            w.WriteEndElement();
        }

        private static void WritePropertyGroup(
            XmlWriter w, string? label, Action? content)
        {
            w.WriteStartElement("PropertyGroup", MsBuildNs);
            if (label != null)
                w.WriteAttributeString("Label", label);
            content?.Invoke();
            w.WriteEndElement();
        }

        private static void WriteImportGroup(
            XmlWriter w, string label, Action? content)
        {
            w.WriteStartElement("ImportGroup", MsBuildNs);
            w.WriteAttributeString("Label", label);
            content?.Invoke();
            w.WriteEndElement();
        }

        private static void WriteElement(XmlWriter w, string name, string value)
        {
            w.WriteStartElement(name, MsBuildNs);
            w.WriteString(value);
            w.WriteEndElement();
        }
    }
}