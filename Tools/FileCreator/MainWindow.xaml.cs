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
            // TODO;
            Log("[Create] Not yet implemented — coming in Step 3.");
        }

        // Helpers 


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