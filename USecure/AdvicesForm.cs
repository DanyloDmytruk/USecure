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

using System.Management;
using WUApiLib;

namespace USecure
{
    public partial class AdvicesForm : Form
    {
        public AdvicesForm()
        {
            InitializeComponent();
            InitializeDataGridView();
        }

        public static bool IsLatestUpdatesInstalled()
        {
            try
            {
                // First way
                try
                {
                    UpdateSession updateSession = new UpdateSession();
                    IUpdateSearcher updateSearcher = updateSession.CreateUpdateSearcher();

                    // Search only for updates that are not installed
                    ISearchResult searchResult = updateSearcher.Search("IsInstalled=0");

                    int updateCount = searchResult.Updates.Count;
                    if (updateCount > 0)
                    {
                        return false;
                    }
                }
                catch (Exception ex)
                {
                    
                }

                // Second way
                // Query the Win32_QuickFixEngineering class
                ManagementObjectSearcher searcher = new ManagementObjectSearcher("SELECT * FROM Win32_QuickFixEngineering");

                // Retrieve the collection of installed updates
                ManagementObjectCollection updates = searcher.Get();

                // Check if there are any installed updates
                if (updates.Count > 0)
                {
                    return false;
                    
                }
                else
                {
                    return true;
                }
            }
            catch (Exception ex)
            {
                return false;
            }

            return false;
        }


        public static bool IsAntivirusInstalled()
        {
            try
            {
                // First way
                string wmipath = @"\\" + Environment.MachineName + @"\root\SecurityCenter2";
                ManagementObjectSearcher searcher0 = new ManagementObjectSearcher(wmipath, "SELECT * FROM AntiVirusProduct");
                ManagementObjectCollection antivirusProducts0 = searcher0.Get();
                if (antivirusProducts0.Count > 0)
                {
                    return true;
                }

                // Second way
                // Query the AntivirusProduct class
                ManagementObjectSearcher searcher = new ManagementObjectSearcher("SELECT * FROM AntivirusProduct");

                // Retrieve the collection of antivirus products
                ManagementObjectCollection antivirusProducts = searcher.Get();

                // Check if any antivirus products are installed
                if (antivirusProducts.Count > 0)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            catch (Exception ex)
            {
                return false;
            }
        }


        public static bool IsFirewallInstalled()
        {
            try
            {
                // First way
                string query = "SELECT * FROM FirewallProduct";
                string wmiNamespace = @"\\.\root\SecurityCenter2";
                ManagementScope scope = new ManagementScope(wmiNamespace);
                scope.Connect();

                ObjectQuery wmiQuery = new ObjectQuery(query);
                ManagementObjectSearcher searcher0 = new ManagementObjectSearcher(scope, wmiQuery);
                ManagementObjectCollection results0 = searcher0.Get();

                if (results0.Count == 0)
                {
                    return false;
                }

                // Second way
                // Query the FirewallProduct class
                ManagementObjectSearcher searcher = new ManagementObjectSearcher("SELECT * FROM FirewallProduct");

                // Retrieve the collection of firewall products
                ManagementObjectCollection firewallProducts = searcher.Get();

                // Check if any firewall products are installed
                if (firewallProducts.Count > 0)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            catch (Exception ex)
            {
                return false;
            }
        }

        public static bool IsBitLockerUsed()
        {
            try
            {
                // Query the Win32_EncryptableVolume class
                ManagementObjectSearcher searcher = new ManagementObjectSearcher("SELECT * FROM Win32_EncryptableVolume");

                // Retrieve the collection of encryptable volumes
                ManagementObjectCollection encryptableVolumes = searcher.Get();

                // Check if any volumes are encrypted with BitLocker
                if (encryptableVolumes.Count > 0)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
            catch (Exception ex)
            {
                return false;
            }
        }

        public static string GetLastBackupShadowCopy()
        {

            try
            {
                // Query the Win32_ShadowCopy class
                ManagementObjectSearcher searcher = new ManagementObjectSearcher("SELECT * FROM Win32_ShadowCopy");

                // Retrieve the collection of shadow copies
                ManagementObjectCollection shadowCopies = searcher.Get();

                // Find the latest shadow copy
                DateTime lastShadowCopyDate = DateTime.MinValue;

                foreach (ManagementObject shadowCopy in shadowCopies)
                {
                    DateTime creationTime = ManagementDateTimeConverter.ToDateTime(shadowCopy["InstallDate"].ToString());

                    if (creationTime > lastShadowCopyDate)
                    {
                        lastShadowCopyDate = creationTime;
                    }
                }

                // Check if any shadow copies were found
                if (lastShadowCopyDate != DateTime.MinValue)
                {
                    return ($"Остання тіньова копія: {lastShadowCopyDate}");
                }
                else
                {
                    return "Тіньових копій не знайдено";
                }
            }
            catch (Exception ex)
            {
                return ($"Немає можливості знайти тіньові копії");
            }
        }

        private void InitializeDataGridView()
        {
            // Create columns
            DataGridViewTextBoxColumn column1 = new DataGridViewTextBoxColumn();
            column1.HeaderText = "Порада";
            column1.Name = "AdviceColumn";

            DataGridViewTextBoxColumn column2 = new DataGridViewTextBoxColumn();
            column2.HeaderText = "Статус";
            column2.Name = "StatusColumn";

            // Add columns to DataGridView
            dataGridView1.Columns.Add(column1);
            dataGridView1.Columns.Add(column2);

            // Add sample data
            dataGridView1.Rows.Add("Встановлення останніх оновлень системи", (IsLatestUpdatesInstalled()) ? "Виконано ✓" : "Не виконано ✖");
            dataGridView1.Rows.Add("Встановлення антивірусного ПЗ", (IsAntivirusInstalled()) ? "Виконано ✓" : "Не виконано ✖");
            dataGridView1.Rows.Add("Використання фаєрвола", (IsFirewallInstalled()) ? "Виконано ✓" : "Не виконано ✖");
            dataGridView1.Rows.Add("Шифрування диску (BitLocker)",(IsBitLockerUsed()) ? "Виконано ✓" : "Не виконано ✖");
            dataGridView1.Rows.Add("Резервне копіювання (остання тіньова копія)", GetLastBackupShadowCopy());

            // Set DataGridView properties
            dataGridView1.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.Fill;
            dataGridView1.AllowUserToAddRows = false; // Optional: Disable the extra "new row" at the bottom
        }

        private void AdvicesForm_Load(object sender, EventArgs e)
        {

        }
    }
}
