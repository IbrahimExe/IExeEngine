using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using FileCreator.Models;

namespace FileCreator.Services
{
    public static class UndoService
    {
        // Result of an undo operation, describing what was and wasn't reversed.
        public class UndoResult
        {
            public bool Success { get; init; }
            public string ErrorMessage { get; init; } = string.Empty;
            public List<string> Steps { get; init; } = new();
        }


        // Reverses a creation operation described by the given record.
        // Each step is attempted independently so partial failures are reported.
        public static UndoResult Undo(CreationRecord record)
        {
            var steps = new List<string>();
            var errors = new List<string>();

            // 1. Delete header file
            if (File.Exists(record.HeaderPath))
            {
                File.Delete(record.HeaderPath);
                steps.Add($"[OK] Deleted header  : {record.HeaderPath}");
            }
            else
            {
                steps.Add($"[!!] Header not found (already deleted?): {record.HeaderPath}");
            }

            // 2. Delete cpp file
            if (record.HadCpp)
            {
                if (record.CppPath != null && File.Exists(record.CppPath))
                {
                    File.Delete(record.CppPath);
                    steps.Add($"[OK] Deleted source  : {record.CppPath}");
                }
                else
                {
                    steps.Add($"[!!] Source not found (already deleted?): {record.CppPath}");
                }
            }

            // 3. Remove #include from umbrella header
            try
            {
                bool removed = UmbrellaHeaderService.RemoveInclude(
                    record.UmbrellaPath, record.FileName);

                steps.Add(removed
                    ? $"[OK] Removed include from umbrella header"
                    : $"[!!] Include not found in umbrella header — skipped");
            }
            catch (Exception ex)
            {
                errors.Add($"Failed to update umbrella header: {ex.Message}");
            }

            // 4. Remove from .vcxproj.filters
            try
            {
                VcxprojService.RemoveFromFilters(
                    record.FiltersPath, record.FileName, record.HadCpp);
                steps.Add($"[OK] Cleaned .vcxproj.filters");
            }
            catch (Exception ex)
            {
                errors.Add($"Failed to update .vcxproj.filters: {ex.Message}");
            }

            // 5. Remove from .vcxproj
            try
            {
                VcxprojService.RemoveFromVcxproj(
                    record.VcxprojPath, record.FileName, record.HadCpp);
                steps.Add($"[OK] Cleaned .vcxproj");
            }
            catch (Exception ex)
            {
                errors.Add($"Failed to update .vcxproj: {ex.Message}");
            }

            // Compile result
            if (errors.Count > 0)
            {
                string errorSummary = string.Join("\n", errors);
                return new UndoResult
                {
                    Success = false,
                    ErrorMessage = errorSummary,
                    Steps = steps
                };
            }

            return new UndoResult { Success = true, Steps = steps };
        }
    }
}