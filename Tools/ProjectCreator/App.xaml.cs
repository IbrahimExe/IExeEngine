using System.Windows;
using ProjectCreator.Models;
using ProjectCreator.Services;
using ProjectCreator.Views;

namespace ProjectCreator
{
    public partial class App : Application
    {
        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);

            // Prevent WPF shutting down when the setup dialog closes
            ShutdownMode = ShutdownMode.OnExplicitShutdown;

            if (!ConfigService.ConfigExists())
            {
                var dialog = new FirstRunDialog();
                bool? result = dialog.ShowDialog();

                if (result != true)
                {
                    Shutdown();
                    return;
                }

                var config = new EngineConfig { EngineName = dialog.EngineName };
                ConfigService.Save(config);
            }

            ShutdownMode = ShutdownMode.OnMainWindowClose;

            var mainWindow = new MainWindow();
            mainWindow.Show();
        }
    }
}