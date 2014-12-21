using Newtonsoft.Json.Linq;
using SlimDX;
using SlimDX.XInput;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;

namespace beaglebone_quadcopter
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }
            

        double motor1 = 0.0;
        double motor2 = 0.0;
        double motor3 = 0.0;
        double motor4 = 0.0;
        double vbat = 0.0;
        double rtt = 0.0;

        murix_series serie_vbat = new murix_series("vbat");
        murix_series serie_rtt = new murix_series("rtt_ms");
        murix_series serie_motor1 = new murix_series("motor1");
        murix_series serie_motor2 = new murix_series("motor2");
        murix_series serie_motor3 = new murix_series("motor3");
        murix_series serie_motor4 = new murix_series("motor4");


        private void Form1_Load(object sender, EventArgs e)
        {
            chart_rtt.Series.Clear();
            chart_rtt.Series.Add(serie_rtt);

            chart_vbat.Series.Clear();
            chart_vbat.Series.Add(serie_vbat);

            chart_motor.Series.Clear();
            chart_motor.Series.Add(serie_motor1);
            chart_motor.Series.Add(serie_motor2);
            chart_motor.Series.Add(serie_motor3);
            chart_motor.Series.Add(serie_motor4);

            backgroundWorker1.WorkerReportsProgress = true;
            backgroundWorker1.ProgressChanged += backgroundWorker1_ProgressChanged;
            backgroundWorker1.RunWorkerAsync();

        }


        void backgroundWorker1_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            try
            {
                serie_vbat.circular_append_y(vbat, 1000);
                serie_rtt.circular_append_y(rtt, 1000);
                serie_motor1.circular_append_y(motor1 / 1000.0, 1000);
                serie_motor2.circular_append_y(motor2 / 1000.0, 1000);
                serie_motor3.circular_append_y(motor3 / 1000.0, 1000);
                serie_motor4.circular_append_y(motor4 / 1000.0, 1000);

                
                chart_rtt.ChartAreas[0].AxisY.IsStartedFromZero = false;
                chart_rtt.ChartAreas[0].RecalculateAxesScale();

                chart_vbat.ChartAreas[0].AxisY.IsStartedFromZero = false;
                chart_vbat.ChartAreas[0].RecalculateAxesScale();

                chart_motor.ChartAreas[0].AxisY.IsStartedFromZero = false;
                chart_motor.ChartAreas[0].RecalculateAxesScale();

            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }


        

        public string sendrecv(string ip,string json, int timeout)
        {
            string returnData = "";
            try
            {
                IPEndPoint RemoteIpEndPoint = new IPEndPoint(IPAddress.Any, 0);


                Byte[] sendBytes = Encoding.ASCII.GetBytes(json);
                
                DateTime start = DateTime.Now;


                UdpClient udpClient = new UdpClient(ip, 32000);
                udpClient.Client.SendTimeout = 1000;
                udpClient.Client.ReceiveTimeout = 1000;
                udpClient.Send(sendBytes, sendBytes.Length);
                Byte[] receiveBytes = udpClient.Receive(ref RemoteIpEndPoint);
                udpClient.Close();


                rtt = DateTime.Now.Subtract(start).TotalMilliseconds;

                returnData = Encoding.ASCII.GetString(receiveBytes);


            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.StackTrace);
              
            }
            return returnData;
        }



        private void backgroundWorker1_DoWork(object sender, DoWorkEventArgs e)
        {

            while (true)
            {
                try
                {
                    //atenção com versao do slimdx no nuget, verifique se o runtime é 4.0 e instale tambem o slimdx runtime 4.0
                    //se a versão do runtime for 2.0 é preciso adicionar no app.config <startup useLegacyV2RuntimeActivationPolicy="true">
                    Gamepad_State_SlimDX joy = new Gamepad_State_SlimDX(UserIndex.One);
                    joy.Update();

                    JObject jsonjoy = new JObject();

                    jsonjoy["joy_start"] = joy.Start;
                    jsonjoy["joy_back"] = joy.Back;

                    jsonjoy["joy_x"] = joy.X;
                    jsonjoy["joy_y"] = joy.Y;
                    jsonjoy["joy_a"] = joy.A;
                    jsonjoy["joy_b"] = joy.B;

                    jsonjoy["joy_dpad_down"] = joy.DPad.Down;
                    jsonjoy["joy_dpad_up"] = joy.DPad.Up;
                    jsonjoy["joy_dpad_left"] = joy.DPad.Left;
                    jsonjoy["joy_dpad_right"] = joy.DPad.Right;

                    jsonjoy["joy_left_s"] = joy.LeftShoulder;
                    jsonjoy["joy_left_c"] = joy.LeftStick.Clicked;
                    jsonjoy["joy_left_x"] = joy.LeftStick.Position.X;
                    jsonjoy["joy_left_y"] = joy.LeftStick.Position.Y;
                    jsonjoy["joy_left_t"] = joy.LeftTrigger;

                    jsonjoy["joy_right_s"] = joy.RightShoulder;
                    jsonjoy["joy_right_c"] = joy.RightStick.Clicked;
                    jsonjoy["joy_right_x"] = joy.RightStick.Position.X;
                    jsonjoy["joy_right_y"] = joy.RightStick.Position.Y;
                    jsonjoy["joy_right_t"] = joy.RightTrigger;



                    string ip="192.168.9.50";
                    if (radioButton_ether.Checked) {
                        ip = "192.168.9.51";
                    }

                    string fromdrone = sendrecv(ip,jsonjoy.ToString(), 1000);
                    if (fromdrone.Length > 0)
                    {
                        JObject json = JObject.Parse(fromdrone);

                        vbat = (double)json["vbat"];

                        motor1 = (double)json["motor1"];
                        motor2 = (double)json["motor2"];
                        motor3 = (double)json["motor3"];
                        motor4 = (double)json["motor4"];

                        backgroundWorker1.ReportProgress(1);
                    }

                    Thread.Yield();
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.ToString());
                }

            }
        }

        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            Environment.Exit(0);
        }
    }
}
