using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace ProjectCreator.Views
{
    public partial class SettingsDialog : Window
    {
        public string EngineName { get; private set; } = string.Empty;

        public SettingsDialog(string currentEngineName, string engineRoot)
        {
            InitializeComponent();
            EngineNameBox.Text = currentEngineName;
            EngineRootBox.Text = engineRoot;
        }

        private void OnSaveClicked(object sender, RoutedEventArgs e)
        {
            string name = EngineNameBox.Text.Trim();
            if (string.IsNullOrWhiteSpace(name))
            {
                MessageBox.Show("Engine name cannot be empty.",
                    "Validation Error", MessageBoxButton.OK, MessageBoxImage.Warning);
                return;
            }
            EngineName = name;
            DialogResult = true;
            Close();
        }

        private void OnCancelClicked(object sender, RoutedEventArgs e)
        {
            DialogResult = false;
            Close();
        }
    }
}
