using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Xml;
using ProjectCreator.Models;

// Generates a .vcxproj.filters file for a new semester project.
// Uses the standard Source Files / Header Files / Resource Files
// filter structure that Visual Studio creates for Desktop projects.
namespace ProjectCreator.Services
{
    public static class FiltersGeneratorService
    {
        private const string MsBuildNs = "http://schemas.microsoft.com/developer/msbuild/2003";

        // These GUIDs are the standard ones Visual Studio always uses
        // for these three filters in a Windows Desktop Wizard project
        private const string SourceFilesGuid = "{4FC737F1-C7A5-4376-A066-2A32D752A2FF}";
        private const string HeaderFilesGuid = "{93995380-89BD-4b04-88EB-625FBE52EBFB}";
        private const string ResourceFilesGuid = "{67DA6AB6-F800-4c08-8B7A-83BB121AAD01}";


        // Generates and writes the .vcxproj.filters file to disk.
        // Returns the full path of the written file.
        public static string Generate(ProjectCreationRequest req)
        {
            string outputPath = System.IO.Path.Combine(
                req.ProjectFolderPath, $"{req.ProjectName}.vcxproj.filters");

            var settings = new XmlWriterSettings
            {
                Indent = true,
                IndentChars = "  ",
                Encoding = System.Text.Encoding.UTF8,
                OmitXmlDeclaration = false
            };

            using XmlWriter w = XmlWriter.Create(outputPath, settings);

            w.WriteStartDocument();
            w.WriteStartElement("Project", MsBuildNs);
            w.WriteAttributeString("ToolsVersion", "4.0");

            // Filter definitions
            w.WriteStartElement("ItemGroup", MsBuildNs);

            WriteFilter(w, "Source Files", SourceFilesGuid,
                "cpp;c;cc;cxx;c++;cppm;ixx;def;odl;idl;hpj;bat;asm;asmx");
            WriteFilter(w, "Header Files", HeaderFilesGuid,
                "h;hh;hpp;hxx;h++;hm;inl;inc;ipp;xsd");
            WriteFilter(w, "Resource Files", ResourceFilesGuid,
                "rc;ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe;resx;" +
                "tiff;tif;png;wav;mfcribbon-ms");

            w.WriteEndElement(); // ItemGroup

            // cpp file entries
            w.WriteStartElement("ItemGroup", MsBuildNs);

            WriteClCompile(w, "GameState.cpp", "Source Files");
            WriteClCompile(w, "WinMain.cpp", "Source Files");

            w.WriteEndElement(); // ItemGroup

            // header file entries
            w.WriteStartElement("ItemGroup", MsBuildNs);

            WriteClInclude(w, "GameState.h", "Header Files");

            w.WriteEndElement(); // ItemGroup

            w.WriteEndElement(); // Project
            w.WriteEndDocument();

            return outputPath;
        }

        // ─── Private XML helpers ─────────────────────────────────────────────────

        private static void WriteFilter(
            XmlWriter w, string name, string guid, string extensions)
        {
            w.WriteStartElement("Filter", MsBuildNs);
            w.WriteAttributeString("Include", name);

            w.WriteStartElement("UniqueIdentifier", MsBuildNs);
            w.WriteString(guid);
            w.WriteEndElement();

            w.WriteStartElement("Extensions", MsBuildNs);
            w.WriteString(extensions);
            w.WriteEndElement();

            w.WriteEndElement(); // Filter
        }

        private static void WriteClCompile(
            XmlWriter w, string fileName, string filter)
        {
            w.WriteStartElement("ClCompile", MsBuildNs);
            w.WriteAttributeString("Include", fileName);

            w.WriteStartElement("Filter", MsBuildNs);
            w.WriteString(filter);
            w.WriteEndElement();

            w.WriteEndElement(); // ClCompile
        }

        private static void WriteClInclude(
            XmlWriter w, string fileName, string filter)
        {
            w.WriteStartElement("ClInclude", MsBuildNs);
            w.WriteAttributeString("Include", fileName);

            w.WriteStartElement("Filter", MsBuildNs);
            w.WriteString(filter);
            w.WriteEndElement();

            w.WriteEndElement(); // ClInclude
        }
    }
}