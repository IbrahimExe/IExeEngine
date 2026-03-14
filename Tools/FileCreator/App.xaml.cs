using System.Configuration;
using System.Data;
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

            if (!ConfigService.ConfigExists())
            {
                var dialog = new FirstRunDialog();
                bool? result = dialog.ShowDialog();

                // exit cleanly is user closes early
                if (result != true)
                {
                    Shutdown();
                    return;
                }

                var config = new EngineConfig { EngineName = dialog.EngineName };
                ConfigService.Save(config);
            }

            var mainWindow = new MainWindow();
            mainWindow.Show();
        }
    }
}
