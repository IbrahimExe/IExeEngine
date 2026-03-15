using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Text.Json;
using FileCreator.Models;

namespace FileCreator.Services
{
    /// <summary>
    /// Manages loading and saving the EngineConfig to a config.json
    /// file stored next to the running executable.
    /// </summary>
    public static class ConfigService
    {
        private static readonly string ConfigPath = Path.Combine(
            AppDomain.CurrentDomain.BaseDirectory,
            "config.json"
        );

        private static readonly JsonSerializerOptions JsonOptions = new()
        {
            WriteIndented = true
        };

        /// <summary>
        /// Returns true if a config file already exists on disk.
        /// Used to decide whether to show the first-run setup dialog.
        /// </summary>
        public static bool ConfigExists() => File.Exists(ConfigPath);

        /// <summary>
        /// Loads and returns the EngineConfig from disk.
        /// Returns a default config if the file is missing or malformed.
        /// </summary>
        public static EngineConfig Load()
        {
            try
            {
                if (!File.Exists(ConfigPath))
                    return new EngineConfig();

                string json = File.ReadAllText(ConfigPath);
                return JsonSerializer.Deserialize<EngineConfig>(json) ?? new EngineConfig();
            }
            catch
            {
                // If anything goes wrong reading config, return safe defaults
                return new EngineConfig();
            }
        }

        /// <summary>
        /// Saves the given EngineConfig to disk as config.json.
        /// </summary>
        public static void Save(EngineConfig config)
        {
            string json = JsonSerializer.Serialize(config, JsonOptions);
            File.WriteAllText(ConfigPath, json);
        }
    }
}
