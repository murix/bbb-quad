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
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea4 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Legend legend4 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.Series series4 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea5 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Legend legend5 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.Series series5 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea6 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Legend legend6 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.Series series6 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea7 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Legend legend7 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.Series series7 = new System.Windows.Forms.DataVisualization.Charting.Series();
            this.backgroundWorker_joystick = new System.ComponentModel.BackgroundWorker();
            this.chart_rtt = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.chart_motor = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.radioButton_ether = new System.Windows.Forms.RadioButton();
            this.radioButton_wifi = new System.Windows.Forms.RadioButton();
            this.chart_vbat = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.timer_charts = new System.Windows.Forms.Timer(this.components);
            this.button_controle = new System.Windows.Forms.Button();
            this.glControl1 = new OpenTK.GLControl();
            this.chart_baro_h = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.chart_baro_t = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.chart_baro_p = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.chart_hz = new System.Windows.Forms.DataVisualization.Charting.Chart();
            ((System.ComponentModel.ISupportInitialize)(this.chart_rtt)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.chart_motor)).BeginInit();
            this.groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.chart_vbat)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.chart_baro_h)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.chart_baro_t)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.chart_baro_p)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.chart_hz)).BeginInit();
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
            this.chart_vbat.Size = new System.Drawing.Size(535, 98);
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
            // glControl1
            // 
            this.glControl1.BackColor = System.Drawing.Color.Black;
            this.glControl1.Location = new System.Drawing.Point(553, 51);
            this.glControl1.Name = "glControl1";
            this.glControl1.Size = new System.Drawing.Size(449, 340);
            this.glControl1.TabIndex = 7;
            this.glControl1.VSync = false;
            this.glControl1.Load += new System.EventHandler(this.glControl1_Load);
            this.glControl1.Paint += new System.Windows.Forms.PaintEventHandler(this.glControl1_Paint);
            this.glControl1.Resize += new System.EventHandler(this.glControl1_Resize);
            // 
            // chart_baro_h
            // 
            chartArea4.Name = "ChartArea1";
            this.chart_baro_h.ChartAreas.Add(chartArea4);
            legend4.Name = "Legend1";
            this.chart_baro_h.Legends.Add(legend4);
            this.chart_baro_h.Location = new System.Drawing.Point(1008, 16);
            this.chart_baro_h.Name = "chart_baro_h";
            series4.ChartArea = "ChartArea1";
            series4.Legend = "Legend1";
            series4.Name = "Series1";
            this.chart_baro_h.Series.Add(series4);
            this.chart_baro_h.Size = new System.Drawing.Size(498, 145);
            this.chart_baro_h.TabIndex = 8;
            this.chart_baro_h.Text = "chart3";
            // 
            // chart_baro_t
            // 
            chartArea5.Name = "ChartArea1";
            this.chart_baro_t.ChartAreas.Add(chartArea5);
            legend5.Name = "Legend1";
            this.chart_baro_t.Legends.Add(legend5);
            this.chart_baro_t.Location = new System.Drawing.Point(1008, 167);
            this.chart_baro_t.Name = "chart_baro_t";
            series5.ChartArea = "ChartArea1";
            series5.Legend = "Legend1";
            series5.Name = "Series1";
            this.chart_baro_t.Series.Add(series5);
            this.chart_baro_t.Size = new System.Drawing.Size(498, 149);
            this.chart_baro_t.TabIndex = 9;
            this.chart_baro_t.Text = "chart3";
            // 
            // chart_baro_p
            // 
            chartArea6.Name = "ChartArea1";
            this.chart_baro_p.ChartAreas.Add(chartArea6);
            legend6.Name = "Legend1";
            this.chart_baro_p.Legends.Add(legend6);
            this.chart_baro_p.Location = new System.Drawing.Point(1008, 322);
            this.chart_baro_p.Name = "chart_baro_p";
            series6.ChartArea = "ChartArea1";
            series6.Legend = "Legend1";
            series6.Name = "Series1";
            this.chart_baro_p.Series.Add(series6);
            this.chart_baro_p.Size = new System.Drawing.Size(498, 158);
            this.chart_baro_p.TabIndex = 10;
            this.chart_baro_p.Text = "chart3";
            // 
            // chart_hz
            // 
            chartArea7.Name = "ChartArea1";
            this.chart_hz.ChartAreas.Add(chartArea7);
            legend7.Name = "Legend1";
            this.chart_hz.Legends.Add(legend7);
            this.chart_hz.Location = new System.Drawing.Point(12, 397);
            this.chart_hz.Name = "chart_hz";
            series7.ChartArea = "ChartArea1";
            series7.Legend = "Legend1";
            series7.Name = "Series1";
            this.chart_hz.Series.Add(series7);
            this.chart_hz.Size = new System.Drawing.Size(535, 128);
            this.chart_hz.TabIndex = 11;
            this.chart_hz.Text = "chart3";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1518, 701);
            this.Controls.Add(this.chart_hz);
            this.Controls.Add(this.chart_baro_p);
            this.Controls.Add(this.chart_baro_t);
            this.Controls.Add(this.chart_baro_h);
            this.Controls.Add(this.glControl1);
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
            ((System.ComponentModel.ISupportInitialize)(this.chart_baro_h)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.chart_baro_t)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.chart_baro_p)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.chart_hz)).EndInit();
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
        private OpenTK.GLControl glControl1;
        private System.Windows.Forms.DataVisualization.Charting.Chart chart_baro_h;
        private System.Windows.Forms.DataVisualization.Charting.Chart chart_baro_t;
        private System.Windows.Forms.DataVisualization.Charting.Chart chart_baro_p;
        private System.Windows.Forms.DataVisualization.Charting.Chart chart_hz;
    }
}

