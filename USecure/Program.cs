using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Diagnostics;
using System.IO;
using Newtonsoft.Json;


namespace USecure
{
    static class Program
    {
        /// <summary>
        /// Главная точка входа для приложения.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);


            try
            {
                string jsonFilePath = "settings.json";

                if (!File.Exists(jsonFilePath))
                {
                    var defaultSettings = new
                    {
                        helpzsu = true
                    };

                    string json = JsonConvert.SerializeObject(defaultSettings, Formatting.Indented);
                    File.WriteAllText(jsonFilePath, json);

                    Application.Run(new Form1());
                }
                else
                {
                    string jsonContent = File.ReadAllText(jsonFilePath);

                    // Deserialize the JSON content into a dynamic object
                    dynamic settings = JsonConvert.DeserializeObject(jsonContent);

                    // Check the value of the "helpzsu" parameter
                    bool helpzsuValue = settings.helpzsu;

                    // If "helpzsu" is true, show a message box
                    if (helpzsuValue == false)
                    {
                        Application.Run(new Form2());
                    }
                    else
                    {
                        Application.Run(new Form1());
                    }
                }
            }
            catch (Exception ex)
            {
                Application.Run(new Form2());
            }

           
        }
    }
}
