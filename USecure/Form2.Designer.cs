
namespace USecure
{
    partial class Form2
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form2));
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.порадиToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.проактивнаЗащитаToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.проПЗToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.progressBar1 = new System.Windows.Forms.ProgressBar();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.menuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.порадиToolStripMenuItem,
            this.проактивнаЗащитаToolStripMenuItem,
            this.проПЗToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(369, 24);
            this.menuStrip1.TabIndex = 0;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // порадиToolStripMenuItem
            // 
            this.порадиToolStripMenuItem.Name = "порадиToolStripMenuItem";
            this.порадиToolStripMenuItem.Size = new System.Drawing.Size(61, 20);
            this.порадиToolStripMenuItem.Text = "Поради";
            this.порадиToolStripMenuItem.Click += new System.EventHandler(this.порадиToolStripMenuItem_Click);
            // 
            // проактивнаЗащитаToolStripMenuItem
            // 
            this.проактивнаЗащитаToolStripMenuItem.Name = "проактивнаЗащитаToolStripMenuItem";
            this.проактивнаЗащитаToolStripMenuItem.Size = new System.Drawing.Size(130, 20);
            this.проактивнаЗащитаToolStripMenuItem.Text = "Проактивний захист";
            this.проактивнаЗащитаToolStripMenuItem.Click += new System.EventHandler(this.проактивнаЗащитаToolStripMenuItem_Click);
            // 
            // проПЗToolStripMenuItem
            // 
            this.проПЗToolStripMenuItem.Name = "проПЗToolStripMenuItem";
            this.проПЗToolStripMenuItem.Size = new System.Drawing.Size(61, 20);
            this.проПЗToolStripMenuItem.Text = "Про ПЗ";
            this.проПЗToolStripMenuItem.Click += new System.EventHandler(this.проПЗToolStripMenuItem_Click);
            // 
            // progressBar1
            // 
            this.progressBar1.Location = new System.Drawing.Point(15, 84);
            this.progressBar1.Name = "progressBar1";
            this.progressBar1.Size = new System.Drawing.Size(342, 37);
            this.progressBar1.TabIndex = 1;
            this.progressBar1.Value = 90;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Arial", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label1.Location = new System.Drawing.Point(65, 47);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(227, 18);
            this.label1.TabIndex = 2;
            this.label1.Text = "Рівень захисту вашої системи:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Arial", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label2.Location = new System.Drawing.Point(160, 137);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(40, 18);
            this.label2.TabIndex = 3;
            this.label2.Text = "90%";
            // 
            // Form2
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(369, 174);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.progressBar1);
            this.Controls.Add(this.menuStrip1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "Form2";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "USecure";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form2_FormClosing);
            this.Load += new System.EventHandler(this.Form2_Load);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem порадиToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem проактивнаЗащитаToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem проПЗToolStripMenuItem;
        private System.Windows.Forms.ProgressBar progressBar1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
    }
}