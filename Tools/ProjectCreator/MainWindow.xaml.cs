using System;
using System.IO;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
//using System.Windows.Shapes;
using ProjectCreator.Models;
using ProjectCreator.Services;
using ProjectCreator.Views;

namespace ProjectCreator
{
    public partial class MainWindow : Window
    {
        private EngineConfig _config = new();
        private string? _engineRoot;
        private string? _slnPath;

        private ParsedSolution? _parsedSolution;

        public MainWindow()
        {
            InitializeComponent();
            Loaded += OnWindowLoaded;
        }

        private void OnWindowLoaded(object sender, RoutedEventArgs e)
        {
            _config = ConfigService.Load();
            _engineRoot = PathResolver.FindEngineRoot();

            if (_engineRoot == null)
            {
                MessageBox.Show(
                    "Could not locate the engine root folder.\n\n" +
                    "Make sure this tool is placed inside your engine's directory tree " +
                    "and that a Framework folder and .sln file exist at the engine root.",
                    "Engine Root Not Found",
                    MessageBoxButton.OK,
                    MessageBoxImage.Error);

                Application.Current.Shutdown();
                return;
            }

            _slnPath = PathResolver.FindSlnFile(_engineRoot);

            if (_slnPath == null)
            {
                MessageBox.Show(
                    "Could not find a .sln file at the engine root.\n\n" +
                    $"Engine root detected as: {_engineRoot}",
                    "Solution File Not Found",
                    MessageBoxButton.OK,
                    MessageBoxImage.Error);

                Application.Current.Shutdown();
                return;
            }

            // Parse the solution file
            try
            {
                _parsedSolution = SolutionParserService.Parse(_slnPath, _engineRoot);
            }
            catch (Exception ex)
            {
                MessageBox.Show(
                    $"Failed to parse the solution file:\n\n{ex.Message}",
                    "Parse Error",
                    MessageBoxButton.OK,
                    MessageBoxImage.Error);

                Application.Current.Shutdown();
                return;
            }

            Title = $"{_config.EngineName} - Project Creator";
            TitleLabel.Text = $"{_config.EngineName} Project Creator";
            EngineLabel.Text = $"Engine: {_config.EngineName}   Root: {_engineRoot}";

            Log("Project Creator ready.");
            Log($"Engine name : {_config.EngineName}");
            Log($"Engine root : {_engineRoot}");
            Log($"Solution    : {System.IO.Path.GetFileName(_slnPath)}");
            Log($"VGP folders  : {string.Join(", ", _parsedSolution.VgpFolders.Select(f => f.Name))}");
            Log($"Engine GUID  : {_parsedSolution.EngineProjectGuid}");
            Log($"Props file   : {_parsedSolution.PropsFileName}.props");
            Log("─────────────────────────────────────────────────────────────────────────");
        }

        private void OnSettingsClicked(object sender, RoutedEventArgs e)
        {
            var dialog = new SettingsDialog(_config.EngineName, _engineRoot ?? "Not found");
            dialog.Owner = this;
            bool? result = dialog.ShowDialog();

            if (result != true) return;

            _config.EngineName = dialog.EngineName;
            ConfigService.Save(_config);

            Title = $"{_config.EngineName} - Project Creator";
            TitleLabel.Text = $"{_config.EngineName} Project Creator";
            EngineLabel.Text = $"Engine: {_config.EngineName}   Root: {_engineRoot}";

            Log($"[Settings] Engine name updated to: {_config.EngineName}");
            StatusLabel.Text = "Settings saved.";
        }

        public void Log(string message)
        {
            string line = $"[{DateTime.Now:HH:mm:ss}] {message}";
            LogBox.AppendText(line + "\n");
            LogBox.ScrollToEnd();
        }

        private void OnTestGenClicked(object sender, RoutedEventArgs e)
        {
            if (_parsedSolution == null || _engineRoot == null || _slnPath == null) return;

            var vgpFolder = _parsedSolution.VgpFolders
                .FirstOrDefault(f => f.Name == "VGP334");

            if (vgpFolder == null)
            {
                Log("VGP334 folder not found in parsed solution.");
                return;
            }

            string testProjectName = "99_TestProject";
            string testFolderPath = Path.Combine(_engineRoot, "VGP334", testProjectName);

            Directory.CreateDirectory(testFolderPath);

            string displayName = StarterFileService.DeriveDisplayName(testProjectName);

            var req = new ProjectCreationRequest
            {
                EngineName = _config.EngineName,
                VgpFolderName = "VGP334",
                VgpFolderGuid = vgpFolder.Guid,
                ProjectName = testProjectName,
                ProjectGuid = $"{{{Guid.NewGuid().ToString().ToUpper()}}}",
                EngineProjectGuid = _parsedSolution.EngineProjectGuid,
                PropsFileName = _parsedSolution.PropsFileName,
                EngineRoot = _engineRoot,
                ProjectFolderPath = testFolderPath,
                SlnPath = _slnPath,
                CopyStarterFiles = true,
                AppDisplayName = displayName
            };

            // Use the most recent VGP334 project as the source
            var sourceProject = _parsedSolution.ProjectsByFolder
                .GetValueOrDefault(vgpFolder.Guid)
                ?.LastOrDefault();

            try
            {
                string vcxprojPath = VcxprojGeneratorService.Generate(req);
                string filtersPath = FiltersGeneratorService.Generate(req);

                if (req.CopyStarterFiles && sourceProject != null)
                {
                    string sourceFolderPath = StarterFileService.GetSourceProjectFolder(
                        _engineRoot, sourceProject);

                    List<string> copyLog = StarterFileService.CopyAndPatch(req, sourceFolderPath);
                    foreach (string entry in copyLog)
                        Log($"{entry}");
                }

                SolutionWriterService.Write(req);

                Log($".vcxproj created  : {vcxprojPath}");
                Log($".filters created  : {filtersPath}");
                Log($".sln updated      : {_slnPath}");
                Log($"Source project    : {sourceProject?.Name ?? "none"}");
                Log($"Display name      : {displayName}");
            }
            catch (Exception ex)
            {
                Log($"error: {ex.Message}");
            }
        }
    }
}