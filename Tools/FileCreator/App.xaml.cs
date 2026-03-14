using System.Windows;
using FileCreator.Models;
using FileCreator.Services;
using FileCreator.Views;

namespace FileCreator
{
    public partial class App : Application
    {
        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);

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