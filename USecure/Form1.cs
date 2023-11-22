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
using System.IO;
using Newtonsoft.Json;

namespace USecure
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {

            try
            {
                Process.Start("https://savelife.in.ua/");
            }
            catch (Exception ex)
            {

            }
        }

        private void button2_Click(object sender, EventArgs e)
        {

            if (checkBox1.Checked == true)
            {
                string filePath = "settings.json";

                if (File.Exists(filePath))
                {
                    // File exists, so open and parse the JSON
                    string json = File.ReadAllText(filePath);
                    dynamic settings = JsonConvert.DeserializeObject(json);

                    // Set the "helpzsu" variable to false
                    settings.helpzsu = false;

                    // Serialize the modified object back to JSON
                    string updatedJson = JsonConvert.SerializeObject(settings, Formatting.Indented);

                    // Write the updated JSON back to the file
                    File.WriteAllText(filePath, updatedJson);
                }
                else
                {
                    // File doesn't exist, so create a new JSON object
                    dynamic settings = new { helpzsu = false };

                    // Serialize the object to JSON
                    string newJson = JsonConvert.SerializeObject(settings, Formatting.Indented);

                    // Write the JSON to the new file
                    File.WriteAllText(filePath, newJson);
                }
            }

            

            Form2 form2 = new Form2();
            form2.Show();
            this.Hide();
        }

        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            e.Cancel = true;
            this.Hide();
            Form2 form2 = new Form2();
            form2.Show();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
           
        }
    }
}
