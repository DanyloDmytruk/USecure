using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

using System.IO;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;


namespace USecure
{
    public partial class ProactiveProtectionForm : Form
    {
        public ProactiveProtectionForm()
        {
            InitializeComponent();
        }

       

        static void CreateSettingsFile(string filePath, object settings)
        {
            // Serialize settings object to JSON
            string json = JsonConvert.SerializeObject(settings, Formatting.Indented);

            // Write JSON to file
            File.WriteAllText(filePath, json);
        }

        static dynamic LoadSettings(string filePath)
        {
            // Read JSON from file and deserialize it
            string json = File.ReadAllText(filePath);
            return JsonConvert.DeserializeObject(json);
        }

        private void button1_Click(object sender, EventArgs e)
        {
            // Define the default settings
            var defaultSettings = new
            {
                agent_autostart = checkBox2.Checked,
                virustotal_downloads_send = checkBox1.Checked,
                agent_background_check = checkBox3.Checked,
                agent_processes_access_check = checkBox4.Checked
            };

            // Specify the file path
            string filePath = "settings.json";

            // Check if the file exists
            if (!File.Exists(filePath))
            {
                // Create the file with default settings
                CreateSettingsFile(filePath, defaultSettings);
            }
            else
            {
                // Read the existing JSON file or create a new one if it doesn't exist
                JObject jsonObj = JsonConvert.DeserializeObject<JObject>(File.ReadAllText(filePath));

                foreach (var property in defaultSettings.GetType().GetProperties())
                {
                    // Check if the property exists in the JSON object
                    if (jsonObj.ContainsKey(property.Name))
                    {
                        // If the property exists, update its value from defaultSettings
                        jsonObj[property.Name] = JToken.FromObject(property.GetValue(defaultSettings));
                    }
                    else
                    {
                        // If the property does not exist, add it to the JSON object
                        jsonObj.Add(new JProperty(property.Name, property.GetValue(defaultSettings)));
                    }
                }


                // Save the updated JSON back to the file
                File.WriteAllText(filePath, jsonObj.ToString());

            }
        }


        private void ApplySettings()
        {
            try
            {
                // Read JSON from file and deserialize it
                string json = File.ReadAllText("settings.json");
                dynamic settings = JsonConvert.DeserializeObject(json);

                // Set CheckBox states based on the loaded settings
                checkBox1.Checked = settings.virustotal_downloads_send;
                checkBox2.Checked = settings.agent_autostart;
                checkBox3.Checked = settings.agent_background_check;
                checkBox4.Checked = settings.agent_processes_access_check;
            }
            catch(Exception ex) { }
        }

        private void ProactiveProtectionForm_Load(object sender, EventArgs e)
        {
            if (File.Exists("settings.json"))
            {
                // Load and apply settings
                ApplySettings();
            }
        }

        private void checkBox2_CheckedChanged(object sender, EventArgs e)
        {

        }
    }
}
