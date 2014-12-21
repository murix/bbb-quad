namespace beaglebone_quadcopter
{
    partial class Form1
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
            this.components = new System.ComponentModel.Container();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea4 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Legend legend4 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.Series series4 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea5 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Legend legend5 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.Series series5 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea6 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Legend legend6 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.Series series6 = new System.Windows.Forms.DataVisualization.Charting.Series();
            this.backgroundWorker1 = new System.ComponentModel.BackgroundWorker();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.chart_rtt = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.chart_motor = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.radioButton_ether = new System.Windows.Forms.RadioButton();
            this.radioButton_wifi = new System.Windows.Forms.RadioButton();
            this.chart_vbat = new System.Windows.Forms.DataVisualization.Charting.Chart();
            ((System.ComponentModel.ISupportInitialize)(this.chart_rtt)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.chart_motor)).BeginInit();
            this.groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.chart_vbat)).BeginInit();
            this.SuspendLayout();
            // 
            // backgroundWorker1
            // 
            this.backgroundWorker1.DoWork += new System.ComponentModel.DoWorkEventHandler(this.backgroundWorker1_DoWork);
            // 
            // timer1
            // 
            this.timer1.Enabled = true;

            // 
            // chart_rtt
            // 
            chartArea4.Name = "ChartArea1";
            this.chart_rtt.ChartAreas.Add(chartArea4);
            legend4.Name = "Legend1";
            this.chart_rtt.Legends.Add(legend4);
            this.chart_rtt.Location = new System.Drawing.Point(12, 51);
            this.chart_rtt.Name = "chart_rtt";
            series4.ChartArea = "ChartArea1";
            series4.Legend = "Legend1";
            series4.Name = "Series1";
            this.chart_rtt.Series.Add(series4);
            this.chart_rtt.Size = new System.Drawing.Size(665, 110);
            this.chart_rtt.TabIndex = 1;
            this.chart_rtt.Text = "chart1";
            // 
            // chart_motor
            // 
            chartArea5.Name = "ChartArea1";
            this.chart_motor.ChartAreas.Add(chartArea5);
            legend5.Name = "Legend1";
            this.chart_motor.Legends.Add(legend5);
            this.chart_motor.Location = new System.Drawing.Point(12, 167);
            this.chart_motor.Name = "chart_motor";
            series5.ChartArea = "ChartArea1";
            series5.Legend = "Legend1";
            series5.Name = "Series1";
            this.chart_motor.Series.Add(series5);
            this.chart_motor.Size = new System.Drawing.Size(665, 120);
            this.chart_motor.TabIndex = 3;
            this.chart_motor.Text = "chart2";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.radioButton_ether);
            this.groupBox1.Controls.Add(this.radioButton_wifi);
            this.groupBox1.Location = new System.Drawing.Point(333, 7);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(270, 38);
            this.groupBox1.TabIndex = 4;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Conexão";
            // 
            // radioButton_ether
            // 
            this.radioButton_ether.AutoSize = true;
            this.radioButton_ether.Checked = true;
            this.radioButton_ether.Location = new System.Drawing.Point(127, 15);
            this.radioButton_ether.Name = "radioButton_ether";
            this.radioButton_ether.Size = new System.Drawing.Size(137, 17);
            this.radioButton_ether.TabIndex = 1;
            this.radioButton_ether.TabStop = true;
            this.radioButton_ether.Text = "Ethernet (192.168.9.51)";
            this.radioButton_ether.UseVisualStyleBackColor = true;
            // 
            // radioButton_wifi
            // 
            this.radioButton_wifi.AutoSize = true;
            this.radioButton_wifi.Location = new System.Drawing.Point(6, 15);
            this.radioButton_wifi.Name = "radioButton_wifi";
            this.radioButton_wifi.Size = new System.Drawing.Size(115, 17);
            this.radioButton_wifi.TabIndex = 0;
            this.radioButton_wifi.Text = "Wifi (192.168.9.50)";
            this.radioButton_wifi.UseVisualStyleBackColor = true;
            // 
            // chart_vbat
            // 
            chartArea6.Name = "ChartArea1";
            this.chart_vbat.ChartAreas.Add(chartArea6);
            legend6.Name = "Legend1";
            this.chart_vbat.Legends.Add(legend6);
            this.chart_vbat.Location = new System.Drawing.Point(12, 293);
            this.chart_vbat.Name = "chart_vbat";
            series6.ChartArea = "ChartArea1";
            series6.Legend = "Legend1";
            series6.Name = "Series1";
            this.chart_vbat.Series.Add(series6);
            this.chart_vbat.Size = new System.Drawing.Size(665, 110);
            this.chart_vbat.TabIndex = 5;
            this.chart_vbat.Text = "chart3";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(689, 408);
            this.Controls.Add(this.chart_vbat);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.chart_motor);
            this.Controls.Add(this.chart_rtt);
            this.Name = "Form1";
            this.Text = "Beaglebone Quadcopter";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.Form1_FormClosed);
            this.Load += new System.EventHandler(this.Form1_Load);
            ((System.ComponentModel.ISupportInitialize)(this.chart_rtt)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.chart_motor)).EndInit();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.chart_vbat)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.ComponentModel.BackgroundWorker backgroundWorker1;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.DataVisualization.Charting.Chart chart_rtt;
        private System.Windows.Forms.DataVisualization.Charting.Chart chart_motor;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.RadioButton radioButton_ether;
        private System.Windows.Forms.RadioButton radioButton_wifi;
        private System.Windows.Forms.DataVisualization.Charting.Chart chart_vbat;
    }
}

