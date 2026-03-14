// Updates both the .vcxproj and .vcxproj.filters XML files
// so Visual Studio registers the new files without needing a manual reload.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Xml;

namespace FileCreator.Services
{
    // Handles updating .vcxproj and .vcxproj.filters XML files
    // to register new .h and .cpp files with Visual Studio.
    public static class VcxprojService
    {
        private const string MsBuildNs = "http://schemas.microsoft.com/developer/msbuild/2003";

        // Public API

        // Registers the new files in the .vcxproj.filters file,
        // assigning them to the correct Inc or Src filter.
        public static void UpdateFilters(
            string filtersPath,
            string fileName,
            bool includeCpp)
        {
            XmlDocument doc = LoadXml(filtersPath);
            XmlNamespaceManager ns = BuildNsManager(doc);

            // Add header to Inc filter
            EnsureClIncludeInFilters(doc, ns, $"Inc\\{fileName}.h", "Inc");

            // Add cpp to Src filter if check box is ticked
            if (includeCpp)
                EnsureClCompileInFilters(doc, ns, $"Src\\{fileName}.cpp", "Src");

            SaveXml(doc, filtersPath);
        }

        // Registers the new files in the .vcxproj file so Visual Studio
        // includes them in the build.
        public static void UpdateVcxproj(
            string vcxprojPath,
            string fileName,
            bool includeCpp)
        {
            XmlDocument doc = LoadXml(vcxprojPath);
            XmlNamespaceManager ns = BuildNsManager(doc);

            // Add header
            EnsureClIncludeInVcxproj(doc, ns, $"Inc\\{fileName}.h");

            // Add cpp if requested
            if (includeCpp)
                EnsureClCompileInVcxproj(doc, ns, $"Src\\{fileName}.cpp");

            SaveXml(doc, vcxprojPath);
        }

        // Filters helpers
        private static void EnsureClIncludeInFilters(
            XmlDocument doc,
            XmlNamespaceManager ns,
            string relativePath,
            string filterName)
        {
            // Check if this entry already exists
            string xpath = $"//ms:ClInclude[@Include='{relativePath}']";
            if (doc.SelectSingleNode(xpath, ns) != null)
                return;

            // Find an existing ClInclude ItemGroup to append to,
            // or create a new one if none exists
            XmlNode? itemGroup = FindOrCreateItemGroupForClInclude(doc, ns);

            XmlElement elem = doc.CreateElement("ClInclude", MsBuildNs);
            elem.SetAttribute("Include", relativePath);

            XmlElement filter = doc.CreateElement("Filter", MsBuildNs);
            filter.InnerText = filterName;
            elem.AppendChild(filter);

            itemGroup.AppendChild(elem);
        }

        private static void EnsureClCompileInFilters(
            XmlDocument doc,
            XmlNamespaceManager ns,
            string relativePath,
            string filterName)
        {
            string xpath = $"//ms:ClCompile[@Include='{relativePath}']";
            if (doc.SelectSingleNode(xpath, ns) != null)
                return;

            XmlNode? itemGroup = FindOrCreateItemGroupForClCompile(doc, ns);

            XmlElement elem = doc.CreateElement("ClCompile", MsBuildNs);
            elem.SetAttribute("Include", relativePath);

            XmlElement filter = doc.CreateElement("Filter", MsBuildNs);
            filter.InnerText = filterName;
            elem.AppendChild(filter);

            itemGroup.AppendChild(elem);
        }

        // .vcxproj helpers 

        private static void EnsureClIncludeInVcxproj(
            XmlDocument doc,
            XmlNamespaceManager ns,
            string relativePath)
        {
            string xpath = $"//ms:ClInclude[@Include='{relativePath}']";
            if (doc.SelectSingleNode(xpath, ns) != null)
                return;

            XmlNode itemGroup = FindOrCreateItemGroupForClInclude(doc, ns);

            XmlElement elem = doc.CreateElement("ClInclude", MsBuildNs);
            elem.SetAttribute("Include", relativePath);
            itemGroup.AppendChild(elem);
        }

        private static void EnsureClCompileInVcxproj(
            XmlDocument doc,
            XmlNamespaceManager ns,
            string relativePath)
        {
            string xpath = $"//ms:ClCompile[@Include='{relativePath}']";
            if (doc.SelectSingleNode(xpath, ns) != null)
                return;

            XmlNode itemGroup = FindOrCreateItemGroupForClCompile(doc, ns);

            XmlElement elem = doc.CreateElement("ClCompile", MsBuildNs);
            elem.SetAttribute("Include", relativePath);
            itemGroup.AppendChild(elem);
        }

        // ItemGroup finders

        // Finds the first ItemGroup that contains ClInclude elements.
        // Creates a new one at the end of the Project node if none exists.
        private static XmlNode FindOrCreateItemGroupForClInclude(
            XmlDocument doc,
            XmlNamespaceManager ns)
        {
            // Find an ItemGroup that already has at least one ClInclude in it
            XmlNode? existing = doc.SelectSingleNode(
                "//ms:ItemGroup[ms:ClInclude]", ns);

            if (existing != null)
                return existing;

            return CreateNewItemGroup(doc);
        }


        /// Finds the first ItemGroup that contains ClCompile elements.
        /// Creates a new one at the end of the Project node if none exists.
        private static XmlNode FindOrCreateItemGroupForClCompile(
            XmlDocument doc,
            XmlNamespaceManager ns)
        {
            XmlNode? existing = doc.SelectSingleNode(
                "//ms:ItemGroup[ms:ClCompile]", ns);

            if (existing != null)
                return existing;

            return CreateNewItemGroup(doc);
        }

        private static XmlNode CreateNewItemGroup(XmlDocument doc)
        {
            XmlElement newGroup = doc.CreateElement("ItemGroup", MsBuildNs);
            doc.DocumentElement!.AppendChild(newGroup);
            return newGroup;
        }

        // XML utilities

        private static XmlDocument LoadXml(string path)
        {
            var doc = new XmlDocument();
            doc.PreserveWhitespace = false;
            doc.Load(path);
            return doc;
        }

        private static XmlNamespaceManager BuildNsManager(XmlDocument doc)
        {
            var ns = new XmlNamespaceManager(doc.NameTable);
            ns.AddNamespace("ms", MsBuildNs);
            return ns;
        }

        private static void SaveXml(XmlDocument doc, string path)
        {
            // Use XmlWriterSettings to keep the output clean and correctly indented
            var settings = new XmlWriterSettings
            {
                Indent = true,
                IndentChars = "  ",
                Encoding = System.Text.Encoding.UTF8,
                OmitXmlDeclaration = false
            };

            using XmlWriter writer = XmlWriter.Create(path, settings);
            doc.Save(writer);
        }
    }
}