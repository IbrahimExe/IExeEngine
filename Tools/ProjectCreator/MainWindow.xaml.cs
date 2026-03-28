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

        // startup:

        private void OnWindowLoaded(object sender, RoutedEventArgs e)
        {
            _config = ConfigService.Load();
            _engineRoot = PathResolver.FindEngineRoot();

            if (_engineRoot == null)
            {
                MessageBox.Show(
                    "Could not locate the engine root folder.\n\n" +
                    "Make sure this tool is placed inside your engine's directory " +
                    "tree and that a Framework folder and .sln file exist at the root.",
                    "Engine Root Not Found",
                    MessageBoxButton.OK, MessageBoxImage.Error);
                Application.Current.Shutdown();
                return;
            }

            _slnPath = PathResolver.FindSlnFile(_engineRoot);

            if (_slnPath == null)
            {
                MessageBox.Show(
                    $"Could not find a .sln file at the engine root.\n\nDetected root: {_engineRoot}",
                    "Solution File Not Found",
                    MessageBoxButton.OK, MessageBoxImage.Error);
                Application.Current.Shutdown();
                return;
            }

            try
            {
                _parsedSolution = SolutionParserService.Parse(_slnPath, _engineRoot);
            }
            catch (Exception ex)
            {
                MessageBox.Show(
                    $"Failed to parse the solution file:\n\n{ex.Message}",
                    "Parse Error",
                    MessageBoxButton.OK, MessageBoxImage.Error);
                Application.Current.Shutdown();
                return;
            }

            // Populate VGP folder dropdown from parsed solution
            foreach (SlnProjectEntry folder in _parsedSolution.VgpFolders)
            {
                VgpFolderComboBox.Items.Add(new ComboBoxItem
                {
                    Content = folder.Name,
                    Tag = folder
                });
            }

            Title = $"{_config.EngineName} — Project Creator";
            TitleLabel.Text = $"{_config.EngineName} Project Creator";
            EngineLabel.Text = $"Engine: {_config.EngineName}   Root: {_engineRoot}";

            Log("Project Creator ready.");
            Log($"Engine name  : {_config.EngineName}");
            Log($"Engine root  : {_engineRoot}");
            Log($"Solution     : {Path.GetFileName(_slnPath)}");
            Log($"VGP folders  : {string.Join(", ", _parsedSolution.VgpFolders.Select(f => f.Name))}");
            Log($"Engine GUID  : {_parsedSolution.EngineProjectGuid}");
            Log($"Props file   : {_parsedSolution.PropsFileName}.props");
            Log("──────────────────────────────────────────");
        }

        // form events:

        private void OnVgpFolderChanged(object sender, SelectionChangedEventArgs e)
        {
            // Repopulate the source project dropdown when the folder changes
            SourceProjectComboBox.Items.Clear();

            if (VgpFolderComboBox.SelectedItem is ComboBoxItem item
                && item.Tag is SlnProjectEntry folder
                && _parsedSolution != null)
            {
                var projects = _parsedSolution.ProjectsByFolder
                    .GetValueOrDefault(folder.Guid);

                if (projects != null)
                {
                    foreach (SlnProjectEntry proj in projects)
                    {
                        SourceProjectComboBox.Items.Add(new ComboBoxItem
                        {
                            Content = proj.Name,
                            Tag = proj
                        });
                    }

                    // Default to the most recent project
                    if (SourceProjectComboBox.Items.Count > 0)
                        SourceProjectComboBox.SelectedIndex =
                            SourceProjectComboBox.Items.Count - 1;
                }
            }

            ValidateForm();
            UpdatePreview();
        }

        private void OnProjectNameChanged(object sender, TextChangedEventArgs e)
        {
            ValidateForm();
            UpdatePreview();
        }

        private void OnCopyFilesCheckedChanged(object sender, RoutedEventArgs e)
        {
            // Show or hide the source project panel based on checkbox
            SourceProjectPanel.Visibility =
                CopyFilesCheckBox.IsChecked == true
                    ? Visibility.Visible
                    : Visibility.Collapsed;

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

            if (dialog.ShowDialog() != true) return;

            _config.EngineName = dialog.EngineName;
            ConfigService.Save(_config);

            Title = $"{_config.EngineName} — Project Creator";
            TitleLabel.Text = $"{_config.EngineName} Project Creator";
            EngineLabel.Text = $"Engine: {_config.EngineName}   Root: {_engineRoot}";

            Log($"[Settings] Engine name updated to: {_config.EngineName}");
            StatusLabel.Text = "Settings saved.";
        }

        // Create:

        private void OnCreateClicked(object sender, RoutedEventArgs e)
        {
            if (_parsedSolution == null || _engineRoot == null || _slnPath == null)
                return;

            string projectName = ProjectNameBox.Text.Trim();
            bool copyFiles = CopyFilesCheckBox.IsChecked == true;

            var folderItem = VgpFolderComboBox.SelectedItem as ComboBoxItem;
            var vgpFolder = folderItem?.Tag as SlnProjectEntry;

            if (vgpFolder == null) return;

            SlnProjectEntry? sourceProject = null;
            if (copyFiles && SourceProjectComboBox.SelectedItem is ComboBoxItem srcItem)
                sourceProject = srcItem.Tag as SlnProjectEntry;

            // Validate
            List<string> issues = ValidationService.Validate(
                _engineRoot, _config.EngineName,
                vgpFolder.Name, projectName, _slnPath);

            if (issues.Count > 0)
            {
                Log("[ERROR] Validation failed:");
                foreach (string issue in issues)
                    Log($"  • {issue}");

                MessageBox.Show(
                    "Project creation cannot proceed:\n\n" + string.Join("\n", issues),
                    "Validation Failed",
                    MessageBoxButton.OK, MessageBoxImage.Warning);

                StatusLabel.Text = "Creation failed — see log for details.";
                return;
            }

            Log("──────────────────────────────────────────");
            Log($"Creating project '{projectName}' in {vgpFolder.Name}...");

            try
            {
                RunCreation(vgpFolder, projectName, copyFiles, sourceProject);
            }
            catch (Exception ex)
            {
                Log($"[ERROR] {ex.Message}");
                MessageBox.Show(
                    $"An unexpected error occurred:\n\n{ex.Message}",
                    "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                StatusLabel.Text = "Creation failed — see log for details.";
            }
        }

        private void RunCreation(
            SlnProjectEntry vgpFolder,
            string projectName,
            bool copyFiles,
            SlnProjectEntry? sourceProject)
        {
            string projectFolderPath = Path.Combine(
                _engineRoot!, vgpFolder.Name, projectName);

            string displayName = StarterFileService.DeriveDisplayName(projectName);

            var req = new ProjectCreationRequest
            {
                EngineName = _config.EngineName,
                VgpFolderName = vgpFolder.Name,
                VgpFolderGuid = vgpFolder.Guid,
                ProjectName = projectName,
                ProjectGuid = $"{{{Guid.NewGuid().ToString().ToUpper()}}}",
                EngineProjectGuid = _parsedSolution!.EngineProjectGuid,
                PropsFileName = _parsedSolution.PropsFileName,
                EngineRoot = _engineRoot!,
                ProjectFolderPath = projectFolderPath,
                SlnPath = _slnPath!,
                CopyStarterFiles = copyFiles,
                SourceProject = sourceProject,
                AppDisplayName = displayName
            };

            // 1. Create the project folder
            Directory.CreateDirectory(projectFolderPath);
            Log($"Created folder     : {projectFolderPath}");

            // 2. Generate .vcxproj
            string vcxprojPath = VcxprojGeneratorService.Generate(req);
            Log($"Generated .vcxproj : {vcxprojPath}");

            // 3. Generate .vcxproj.filters
            string filtersPath = FiltersGeneratorService.Generate(req);
            Log($"Generated .filters : {filtersPath}");

            // 4. Copy and patch starter files if requested
            if (copyFiles && sourceProject != null)
            {
                string sourceFolderPath = StarterFileService.GetSourceProjectFolder(
                    _engineRoot!, sourceProject);

                List<string> copyLog = StarterFileService.CopyAndPatch(req, sourceFolderPath);
                foreach (string entry in copyLog)
                    Log(entry);
            }

            // 5. Write to .sln
            SolutionWriterService.Write(req);
            Log($"Updated .sln");

            // Done
            Log($"[DONE] '{projectName}' created successfully.");
            Log($"  → Reload the solution in Visual Studio to see the new project.");

            StatusLabel.Text = $"✓ {projectName} created successfully.";

            // Refresh the source project dropdown with the new project included
            RefreshSourceProjectDropdown(vgpFolder);

            MessageBox.Show(
                $"'{projectName}' was created successfully in {vgpFolder.Name}.\n\n" +
                "Visual Studio will prompt you to reload the solution; click Reload!",
                "Project Created",
                MessageBoxButton.OK, MessageBoxImage.Information);
        }

        // ─── Helpers ──────────────────────────────────────────────────────────────

        private void ValidateForm()
        {
            if (CreateButton == null) return;

            bool folderSelected = VgpFolderComboBox.SelectedItem != null;
            bool nameEntered = !string.IsNullOrWhiteSpace(ProjectNameBox.Text);

            CreateButton.IsEnabled = folderSelected && nameEntered;
        }

        private void UpdatePreview()
        {
            if (PreviewLabel == null) return;

            bool folderSelected = VgpFolderComboBox.SelectedItem != null;
            bool nameEntered = !string.IsNullOrWhiteSpace(ProjectNameBox.Text);
            bool copyFiles = CopyFilesCheckBox.IsChecked == true;

            if (!folderSelected || !nameEntered)
            {
                PreviewLabel.Text = "";
                return;
            }

            var folderItem = VgpFolderComboBox.SelectedItem as ComboBoxItem;
            string folderName = (folderItem?.Content as string) ?? "";
            string projectName = ProjectNameBox.Text.Trim();
            string displayName = StarterFileService.DeriveDisplayName(projectName);

            string filesNote = copyFiles ? "  |  starter files copied" : "  |  empty project";

            PreviewLabel.Text =
                $"Will create → {folderName}/{projectName}/  " +
                $"|  appName: \"{displayName}\"{filesNote}";
        }

        // Re-parses the solution and refreshes the source dropdown after a creation
        private void RefreshSourceProjectDropdown(SlnProjectEntry vgpFolder)
        {
            if (_engineRoot == null || _slnPath == null) return;

            try
            {
                _parsedSolution = SolutionParserService.Parse(_slnPath, _engineRoot);

                SourceProjectComboBox.Items.Clear();
                var projects = _parsedSolution.ProjectsByFolder
                    .GetValueOrDefault(vgpFolder.Guid);

                if (projects == null) return;

                foreach (SlnProjectEntry proj in projects)
                {
                    SourceProjectComboBox.Items.Add(new ComboBoxItem
                    {
                        Content = proj.Name,
                        Tag = proj
                    });
                }

                if (SourceProjectComboBox.Items.Count > 0)
                    SourceProjectComboBox.SelectedIndex =
                        SourceProjectComboBox.Items.Count - 1;
            }
            catch
            {
                // Non-fatal — dropdown just won't update until next launch
            }
        }

        public void Log(string message)
        {
            string line = $"[{DateTime.Now:HH:mm:ss}] {message}";
            LogBox.AppendText(line + "\n");
            LogBox.ScrollToEnd();
        }
    }
}