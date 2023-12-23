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


using Newtonsoft.Json.Linq;


namespace USecure
{
    public partial class Form2 : Form
    {
        public Form2()
        {
            InitializeComponent();
        }

        private void Form2_FormClosing(object sender, FormClosingEventArgs e)
        {
            Application.ExitThread();
        }

        private void порадиToolStripMenuItem_Click(object sender, EventArgs e)
        {
            AdvicesForm advicesForm = new AdvicesForm();
            advicesForm.Show();
        }

        private void проактивнаЗащитаToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ProactiveProtectionForm proactiveProtectionForm = new ProactiveProtectionForm();
            proactiveProtectionForm.Show();
        }

        private void проПЗToolStripMenuItem_Click(object sender, EventArgs e)
        {
            AboutForm aboutForm = new AboutForm();
            aboutForm.Show();
        }

        private void Form2_Load(object sender, EventArgs e)
        {
            int protectionsActivated = 0;


            //Advices
            if (AdvicesForm.IsLatestUpdatesInstalled() == true) { protectionsActivated++; };
            if (AdvicesForm.IsAntivirusInstalled() == true) { protectionsActivated++; };
            if (AdvicesForm.IsFirewallInstalled() == true) { protectionsActivated++; };
            if (AdvicesForm.IsBitLockerUsed() == true) { protectionsActivated++; };

            //Proactive
            if (File.Exists("settings.json"))
            {
                string jsonContent = File.ReadAllText("settings.json");

                // Parse the JSON content
                JObject jsonObj = JObject.Parse(jsonContent);

                if (jsonObj.TryGetValue("agent_autostart", out var autostartValue) && autostartValue.Type == JTokenType.Boolean && (bool)autostartValue)
                {
                    protectionsActivated++;
                }

                if (jsonObj.TryGetValue("virustotal_downloads_send", out var vtdownloadValue) && vtdownloadValue.Type == JTokenType.Boolean && (bool)vtdownloadValue)
                {
                    protectionsActivated++;
                }

                if (jsonObj.TryGetValue("agent_background_check", out var agentBackgroundValue) && agentBackgroundValue.Type == JTokenType.Boolean && (bool)agentBackgroundValue)
                {
                    protectionsActivated++;
                }

                if (jsonObj.TryGetValue("agent_processes_access_check", out var agentProcessesAccessValue) && agentProcessesAccessValue.Type == JTokenType.Boolean && (bool)agentProcessesAccessValue)
                {
                    protectionsActivated++;
                }
            }

            //Summary:
            int SystemProtectionPercentage = (int)Math.Round((((float)protectionsActivated / 8.0) * 100));

            if(SystemProtectionPercentage == 100)
            {
                SystemProtectionPercentage--; //There's no absolutely protected system
            }

            label2.Text = SystemProtectionPercentage.ToString()+"%";
            progressBar1.Value = SystemProtectionPercentage;


        }
    }
}
