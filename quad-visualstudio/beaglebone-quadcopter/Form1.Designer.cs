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
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea1 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Legend legend1 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.Series series1 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea2 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Legend legend2 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.Series series2 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea3 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Legend legend3 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.Series series3 = new System.Windows.Forms.DataVisualization.Charting.Series();
            this.backgroundWorker_joystick = new System.ComponentModel.BackgroundWorker();
            this.chart_rtt = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.chart_motor = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.radioButton_ether = new System.Windows.Forms.RadioButton();
            this.radioButton_wifi = new System.Windows.Forms.RadioButton();
            this.chart_vbat = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.timer_charts = new System.Windows.Forms.Timer(this.components);
            this.button_controle = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.chart_rtt)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.chart_motor)).BeginInit();
            this.groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.chart_vbat)).BeginInit();
            this.SuspendLayout();
            // 
            // backgroundWorker_joystick
            // 
            this.backgroundWorker_joystick.DoWork += new System.ComponentModel.DoWorkEventHandler(this.backgroundWorker1_DoWork);
            // 
            // chart_rtt
            // 
            chartArea1.Name = "ChartArea1";
            this.chart_rtt.ChartAreas.Add(chartArea1);
            legend1.Name = "Legend1";
            this.chart_rtt.Legends.Add(legend1);
            this.chart_rtt.Location = new System.Drawing.Point(12, 51);
            this.chart_rtt.Name = "chart_rtt";
            series1.ChartArea = "ChartArea1";
            series1.Legend = "Legend1";
            series1.Name = "Series1";
            this.chart_rtt.Series.Add(series1);
            this.chart_rtt.Size = new System.Drawing.Size(535, 110);
            this.chart_rtt.TabIndex = 1;
            this.chart_rtt.Text = "chart1";
            // 
            // chart_motor
            // 
            chartArea2.Name = "ChartArea1";
            this.chart_motor.ChartAreas.Add(chartArea2);
            legend2.Name = "Legend1";
            this.chart_motor.Legends.Add(legend2);
            this.chart_motor.Location = new System.Drawing.Point(12, 167);
            this.chart_motor.Name = "chart_motor";
            series2.ChartArea = "ChartArea1";
            series2.Legend = "Legend1";
            series2.Name = "Series1";
            this.chart_motor.Series.Add(series2);
            this.chart_motor.Size = new System.Drawing.Size(535, 120);
            this.chart_motor.TabIndex = 3;
            this.chart_motor.Text = "chart2";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.radioButton_ether);
            this.groupBox1.Controls.Add(this.radioButton_wifi);
            this.groupBox1.Location = new System.Drawing.Point(12, 7);
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
            chartArea3.Name = "ChartArea1";
            this.chart_vbat.ChartAreas.Add(chartArea3);
            legend3.Name = "Legend1";
            this.chart_vbat.Legends.Add(legend3);
            this.chart_vbat.Location = new System.Drawing.Point(12, 293);
            this.chart_vbat.Name = "chart_vbat";
            series3.ChartArea = "ChartArea1";
            series3.Legend = "Legend1";
            series3.Name = "Series1";
            this.chart_vbat.Series.Add(series3);
            this.chart_vbat.Size = new System.Drawing.Size(535, 110);
            this.chart_vbat.TabIndex = 5;
            this.chart_vbat.Text = "chart3";
            // 
            // timer_charts
            // 
            this.timer_charts.Tick += new System.EventHandler(this.timer_charts_Tick);
            // 
            // button_controle
            // 
            this.button_controle.ForeColor = System.Drawing.SystemColors.ControlDarkDark;
            this.button_controle.Location = new System.Drawing.Point(297, 16);
            this.button_controle.Name = "button_controle";
            this.button_controle.Size = new System.Drawing.Size(164, 23);
            this.button_controle.TabIndex = 6;
            this.button_controle.Text = "Controle Xbox (conectado)";
            this.button_controle.UseVisualStyleBackColor = true;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(894, 408);
            this.Controls.Add(this.button_controle);
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

        private System.ComponentModel.BackgroundWorker backgroundWorker_joystick;
        private System.Windows.Forms.DataVisualization.Charting.Chart chart_rtt;
        private System.Windows.Forms.DataVisualization.Charting.Chart chart_motor;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.RadioButton radioButton_ether;
        private System.Windows.Forms.RadioButton radioButton_wifi;
        private System.Windows.Forms.DataVisualization.Charting.Chart chart_vbat;
        private System.Windows.Forms.Timer timer_charts;
        private System.Windows.Forms.Button button_controle;
    }
}

