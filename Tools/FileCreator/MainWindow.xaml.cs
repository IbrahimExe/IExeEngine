using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.IO;

using FileCreator.Models;
using FileCreator.Services;
using FileCreator.Views;

namespace FileCreator
{
    public partial class MainWindow : Window
    {
        private EngineConfig _config = new();
        private string? _engineRoot;
        private CreationRecord? _lastCreation; // Stores the last operation for undo

        public MainWindow()
        {
            InitializeComponent();
            Loaded += OnWindowLoaded;
        }

        // Startup
        private void OnWindowLoaded(object sender, RoutedEventArgs e)
        {
            // Config is guaranteed to exist by the time MainWindow opens
            _config = ConfigService.Load();

            // Locate the engine root on disk
            _engineRoot = PathResolver.FindEngineRoot();

            if (_engineRoot == null)
            {
                MessageBox.Show(
                    "Could not locate the engine root folder.\n\n" +
                    "Make sure this tool is inside the engine's directory tree " +
                    "and that a 'Framework' folder exists at the engine root.",
                    "Engine Root Not Found",
                    MessageBoxButton.OK,
                    MessageBoxImage.Error
                );
                Application.Current.Shutdown();
                return;
            }

            // Update UI labels with loaded config
            Title = $"{_config.EngineName} — File Creator";
            TitleLabel.Text = $"{_config.EngineName} File Creator";
            EngineLabel.Text = $"Engine: {_config.EngineName}   Root: {_engineRoot}";

            Log($"File Creator ready.");
            Log($"Engine name   : {_config.EngineName}");
            Log($"Engine root   : {_engineRoot}");
            Log($"───────────────────────────────────────────────────────────────────────────────");
        }

        // UI event handlers 
        private void OnProjectSelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            ValidateForm();
        }

        private void OnFileNameChanged(object sender, TextChangedEventArgs e)
        {
            ValidateForm();
        }

        private void OnCreateClicked(object sender, RoutedEventArgs e)
        {
            string projectName = ((ComboBoxItem)ProjectComboBox.SelectedItem).Content.ToString()!;
            string fileName = FileNameBox.Text.Trim();
            bool includeCpp = IncludeCppCheckBox.IsChecked == true;

            Log($"──────────────────────────────────────────────────────────────────────────────");
            Log($"Starting file creation...");
            Log($"  Project : {projectName}");
            Log($"  File    : {fileName}");
            Log($"  Add .cpp: {includeCpp}");

            // Validation
            List<string> issues = ValidationService.Validate(
                _engineRoot!, projectName, fileName, includeCpp);

            if (issues.Count > 0)
            {
                Log($"[ERROR] Validation failed:");
                foreach (string issue in issues)
                    Log($"  • {issue}");

                MessageBox.Show(
                    "File creation cannot proceed:\n\n" + string.Join("\n", issues),
                    "Validation Failed",
                    MessageBoxButton.OK,
                    MessageBoxImage.Warning);

                StatusLabel.Text = "Creation Failed! Check log for details.";
                return;
            }

            // Creation 
            try
            {
                RunCreation(projectName, fileName, includeCpp);
            }
            catch (Exception ex)
            {
                Log($"[ERROR] Unexpected error: {ex.Message}");
                MessageBox.Show(
                    $"An unexpected error occurred:\n\n{ex.Message}",
                    "Error",
                    MessageBoxButton.OK,
                    MessageBoxImage.Error);

                StatusLabel.Text = "Creation failed! see log for details.";
            }
        }

        private void OnUndoClicked(object sender, RoutedEventArgs e)
        {
            if (_lastCreation == null) return;

            // Confirm before doing anything destructive
            var confirm = MessageBox.Show(
                $"This will permanently delete '{_lastCreation.FileName}.h'" +
                (_lastCreation.HadCpp ? $" and '{_lastCreation.FileName}.cpp'" : "") +
                $" from the {_lastCreation.ProjectName} project and reverse all changes.\n\n" +
                "Are you sure?",
                "Confirm Undo",
                MessageBoxButton.YesNo,
                MessageBoxImage.Warning);

            if (confirm != MessageBoxResult.Yes) return;

            Log($"──────────────────────────────────────────────────────────────────────────────");
            Log($"Undoing creation of '{_lastCreation.FileName}' in {_lastCreation.ProjectName}...");

            UndoService.UndoResult result = UndoService.Undo(_lastCreation);

            // Log each step regardless of success
            foreach (string step in result.Steps)
                Log(step);

            if (result.Success)
            {
                Log($"[DONE] Undo complete. '{_lastCreation.FileName}' has been removed.");
                StatusLabel.Text = $"↩ Undo complete — '{_lastCreation.FileName}' removed.";

                MessageBox.Show(
                    $"'{_lastCreation.FileName}' has been fully removed from the " +
                    $"{_lastCreation.ProjectName} project.\n\n" +
                    $"Reload the {_lastCreation.ProjectName} project in Visual Studio to confirm.",
                    "Undo Complete",
                    MessageBoxButton.OK,
                    MessageBoxImage.Information);
            }
            else
            {
                Log($"[ERROR] Undo completed with errors:");
                Log($"  {result.ErrorMessage}");
                StatusLabel.Text = "Undo completed with errors — see log.";

                MessageBox.Show(
                    $"Undo completed but some steps had errors:\n\n{result.ErrorMessage}",
                    "Undo Errors",
                    MessageBoxButton.OK,
                    MessageBoxImage.Warning);
            }

            // Clear the record either way
            _lastCreation = null;
            UndoButton.IsEnabled = false;
        }

        // Helper s
        private void RunCreation(string projectName, string fileName, bool includeCpp)
        {
            string engineRoot = _engineRoot!;
            string engineName = _config.EngineName;
            string incPath = PathResolver.GetIncPath(engineRoot, projectName);
            string srcPath = PathResolver.GetSrcPath(engineRoot, projectName);
            string vcxproj = PathResolver.GetVcxprojPath(engineRoot, projectName);
            string filters = PathResolver.GetVcxprojFiltersPath(engineRoot, projectName);
            string umbrella = PathResolver.GetUmbrellaHeaderPath(engineRoot, projectName);

            // 1. Create .h file
            string headerPath = FileGeneratorService.CreateHeaderFile(
                incPath, fileName, engineName, projectName);
            Log($"[OK] Created header  : {headerPath}");

            // 2. Create .cpp file
            if (includeCpp)
            {
                string cppPath = FileGeneratorService.CreateCppFile(
                    srcPath, fileName, engineName, projectName);
                Log($"[OK] Created source  : {cppPath}");
            }

            // 3. Inject #include into umbrella header
            bool injected = UmbrellaHeaderService.InjectInclude(umbrella, fileName);
            if (injected)
                Log($"[OK] Injected include into: {PathResolver.GetUmbrellaHeaderName(projectName)}");
            else
                Log($"[!!] Include already present in {PathResolver.GetUmbrellaHeaderName(projectName)} — skipped.");

            // 4. Update .vcxproj.filters
            VcxprojService.UpdateFilters(filters, fileName, includeCpp);
            Log($"[OK] Updated .vcxproj.filters");

            // 5. Update .vcxproj
            VcxprojService.UpdateVcxproj(vcxproj, fileName, includeCpp);
            Log($"[OK] Updated .vcxproj");

            // Store record for undo 
            _lastCreation = new CreationRecord
            {
                ProjectName = projectName,
                FileName = fileName,
                HeaderPath = System.IO.Path.Combine(incPath, $"{fileName}.h"),
                CppPath = includeCpp ? System.IO.Path.Combine(srcPath, $"{fileName}.cpp") : null,
                UmbrellaPath = umbrella,
                VcxprojPath = vcxproj,
                FiltersPath = filters,
                HadCpp = includeCpp
            };

            UndoButton.IsEnabled = true;

            // Success 
            Log($"[DONE] '{fileName}' successfully added to {projectName}.");
            Log($"  → Reload the {projectName} project in Visual Studio to see the new files.");
            StatusLabel.Text = $"✓ {fileName} added to {projectName} successfully.";

            MessageBox.Show(
                $"'{fileName}' was successfully created in the {projectName} project.\n\n" +
                $"Right-click the {projectName} project in Visual Studio\n" +
                $"and select 'Reload Project' to see the new files.",
                "Files Created",
                MessageBoxButton.OK,
                MessageBoxImage.Information);
        }

        /// Updates the preview label below the form fields to show exactly
        /// which files will be created, so the user can verify before clicking.
        private void UpdatePreview()
        {
            if (PreviewLabel == null) return;

            bool projectSelected = ProjectComboBox.SelectedItem != null;
            bool nameEntered = !string.IsNullOrWhiteSpace(FileNameBox.Text);
            bool includeCpp = IncludeCppCheckBox.IsChecked == true;

            if (!projectSelected || !nameEntered)
            {
                PreviewLabel.Text = "";
                return;
            }

            string project = ((ComboBoxItem)ProjectComboBox.SelectedItem).Content.ToString()!;
            string name = FileNameBox.Text.Trim();

            string header = $"Framework/{project}/Inc/{name}.h";
            string cpp = $"Framework/{project}/Src/{name}.cpp";
            string umbrella = PathResolver.GetUmbrellaHeaderName(project);

            string preview = includeCpp
                ? $"Will create → {header}  |  {cpp}  |  include injected into {umbrella}"
                : $"Will create → {header}  |  include injected into {umbrella}";

            PreviewLabel.Text = preview;
        }

        private void OnCppCheckChanged(object sender, RoutedEventArgs e)
        {
            UpdatePreview();
        }

        private void OnClearLogClicked(object sender, RoutedEventArgs e)
        {
            LogBox.Clear();
            Log("Log cleared.");
        }

        private void OnSettingsClicked(object sender, RoutedEventArgs e)
        {
            var dialog = new SettingsDialog(_config.EngineName, _engineRoot ?? "Not found");
            dialog.Owner = this;
            bool? result = dialog.ShowDialog();

            if (result != true) return;

            // Save the updated engine name
            _config.EngineName = dialog.EngineName;
            ConfigService.Save(_config);

            // Refresh UI labels
            Title = $"{_config.EngineName} — File Creator";
            TitleLabel.Text = $"{_config.EngineName} File Creator";
            EngineLabel.Text = $"Engine: {_config.EngineName}   Root: {_engineRoot}";

            Log($"[Settings] Engine name updated to: {_config.EngineName}");
            StatusLabel.Text = "Settings saved.";
        }

        // Enables the Create button only when both a project and a valid
        // file name have been entered.
        private void ValidateForm()
        {
            if (CreateButton == null) return;

            bool projectSelected = ProjectComboBox.SelectedItem != null;
            bool nameEntered = !string.IsNullOrWhiteSpace(FileNameBox.Text);

            CreateButton.IsEnabled = projectSelected && nameEntered;

            UpdatePreview();
        }

        // Appends a timestamped line to the log output box.
        public void Log(string message)
        {
            string line = $"[{DateTime.Now:HH:mm:ss}] {message}";
            LogBox.AppendText(line + "\n");
            LogBox.ScrollToEnd();
        }
    }
}