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

using FileCreator.Models;
using FileCreator.Services;
using FileCreator.Views;

namespace FileCreator
{
    public partial class MainWindow : Window
    {
        private EngineConfig _config = new();
        private string? _engineRoot;

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

                StatusLabel.Text = "Creation failed — see log for details.";
            }
        }

        // Helper 
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

        // Enables the Create button only when both a project and a valid
        // file name have been entered.
        private void ValidateForm()
        {
            if (CreateButton == null) return;

            bool projectSelected = ProjectComboBox.SelectedItem != null;
            bool nameEntered = !string.IsNullOrWhiteSpace(FileNameBox.Text);

            CreateButton.IsEnabled = projectSelected && nameEntered;
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