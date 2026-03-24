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

            Title = $"{_config.EngineName} — Project Creator";
            TitleLabel.Text = $"{_config.EngineName} Project Creator";
            EngineLabel.Text = $"Engine: {_config.EngineName}   Root: {_engineRoot}";

            Log("Project Creator ready.");
            Log($"Engine name : {_config.EngineName}");
            Log($"Engine root : {_engineRoot}");
            Log($"Solution    : {System.IO.Path.GetFileName(_slnPath)}");
            Log("───────────────────────────────────────────────────────────────────");
        }

        private void OnSettingsClicked(object sender, RoutedEventArgs e)
        {
            Title = $"{_config.EngineName} — Project Creator";
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
    }
}