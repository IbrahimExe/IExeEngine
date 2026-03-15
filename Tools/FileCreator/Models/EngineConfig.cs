using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FileCreator.Models
{
    /// <summary>
    /// Stores user-specific configuration that persists between sessions.
    /// Saved as config.json next to the executable.
    /// </summary>
    public class EngineConfig
    {
        public string EngineName { get; set; } = "IExeEngine";
    }
}
