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
    public partial class FirstRunDialog : Window
    {
        public string EngineName { get; private set; } = "IExeEngine";

        public FirstRunDialog() => InitializeComponent();

        private void OnConfirmClicked(object sender, RoutedEventArgs e)
        {
            string name = EngineNameBox.Text.Trim();

            if (string.IsNullOrWhiteSpace(name))
            {
                MessageBox.Show("Please enter a valid engine name.",
                    "Validation Error", MessageBoxButton.OK, MessageBoxImage.Warning);
                return;
            }

            EngineName = name;
            DialogResult = true;
            Close();
        }
    }
}
