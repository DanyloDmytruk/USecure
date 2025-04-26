using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

using System.Diagnostics;

namespace USecure
{
    public partial class CheckFile : Form
    {
        public string filePath;

        public CheckFile()
        {
            InitializeComponent();
        }

        private void CheckFile_Load(object sender, EventArgs e)
        {
             
        }

        private void button2_Click(object sender, EventArgs e)
        {
            Clipboard.SetText(this.richTextBox1.Text);
        }

        public static string RunCommand(string exeName, string arguments)
        {
            var process = new Process();
            process.StartInfo.FileName = exeName;
            process.StartInfo.Arguments = arguments;
            process.StartInfo.RedirectStandardOutput = true;
            process.StartInfo.RedirectStandardError = true;
            process.StartInfo.UseShellExecute = false;
            process.StartInfo.CreateNoWindow = true;

            process.Start();

            string output = process.StandardOutput.ReadToEnd();
            string error = process.StandardError.ReadToEnd();

            process.WaitForExit();

            return output + (string.IsNullOrWhiteSpace(error) ? "" : "\nError:\n" + error);
        }

        private void button1_Click(object sender, EventArgs e)
        {
            // Create and configure the OpenFileDialog
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Filter = "Executable files (*.exe)|*.exe"; 
            openFileDialog.Title = "Select an executable file";
            
            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                filePath = openFileDialog.FileName;
                label1.Text = filePath;
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            // Check file with tools
            this.richTextBox1.Text = "Йде перевірка..";
            string result = "";

            if (label1.Text != "Не обрано") 
            {
                // Check file with PyYaraDetector
                string yaraDetectorResult = RunCommand("pyyaradetector.exe", label1.Text);
                result += yaraDetectorResult;
            }

            this.richTextBox1.Text = result;
        }
    }
}
