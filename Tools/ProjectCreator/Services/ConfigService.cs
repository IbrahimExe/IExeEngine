using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;
using System.IO;
using ProjectCreator.Models;

namespace ProjectCreator.Services
{
    // Loads and saves EngineConfig to config.json next to the executable.
    public static class ConfigService
    {
        private static readonly string ConfigPath = System.IO.Path.Combine(
            AppDomain.CurrentDomain.BaseDirectory,
            "config.json"
        );

        private static readonly JsonSerializerOptions JsonOptions = new()
        {
            WriteIndented = true
        };

        public static bool ConfigExists() => File.Exists(ConfigPath);

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
                return new EngineConfig();
            }
        }

        public static void Save(EngineConfig config)
        {
            string json = JsonSerializer.Serialize(config, JsonOptions);
            File.WriteAllText(ConfigPath, json);
        }
    }
}
