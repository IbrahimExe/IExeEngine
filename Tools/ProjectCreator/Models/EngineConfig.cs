using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ProjectCreator.Models
{
    /// <summary>
    /// Persisted user configuration saved to config.json next to the executable.
    /// </summary>
    public class EngineConfig
    {
        /// <summary>
        /// The engine namespace and props file name, e.g. "IExeEngine".
        /// </summary>
        public string EngineName { get; set; } = "IExeEngine";
    }
}
