using Newtonsoft.Json.Linq;
using OpenTK;
using OpenTK.Graphics;
using OpenTK.Graphics.OpenGL;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
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
            

        double motor_fl = 0.0;
        double motor_fr = 0.0;
        double motor_rl = 0.0;
        double motor_rr = 0.0;
        double vbat = 0.0;
        double rtt = 0.0;
        double baro_p = 0.0;
        double baro_h = 0.0;
        double baro_t = 0.0;

        murix_series serie_vbat = new murix_series("vbat");
        murix_series serie_rtt = new murix_series("rtt_ms");
        murix_series serie_motor_fl = new murix_series("motor_fl");
        murix_series serie_motor_fr = new murix_series("motor_fr");
        murix_series serie_motor_rl = new murix_series("motor_rl");
        murix_series serie_motor_rr = new murix_series("motor_rr");
        murix_series serie_baro_p = new murix_series("baro_p");
        murix_series serie_baro_h = new murix_series("baro_h");
        murix_series serie_baro_t = new murix_series("baro_t");

        double i2c_hz = 0;
        double pru_hz = 0;
        double adc_hz = 0;
        double pilot_hz = 0;
        murix_series serie_i2c_hz = new murix_series("i2c_hz");
        murix_series serie_pru_hz = new murix_series("pru_hz");
        murix_series serie_adc_hz = new murix_series("adc_hz");
        murix_series serie_pilot_hz = new murix_series("pilot_hz");


        private void Form1_Load(object sender, EventArgs e)
        {
            chart_rtt.Series.Clear();
            chart_rtt.Series.Add(serie_rtt);

            chart_vbat.Series.Clear();
            chart_vbat.Series.Add(serie_vbat);

            chart_motor.Series.Clear();
            chart_motor.Series.Add(serie_motor_fl);
            chart_motor.Series.Add(serie_motor_fr);
            chart_motor.Series.Add(serie_motor_rl);
            chart_motor.Series.Add(serie_motor_rr);

            chart_baro_h.Series.Clear();
            chart_baro_h.Series.Add(serie_baro_h);

            chart_baro_p.Series.Clear();
            chart_baro_p.Series.Add(serie_baro_p);

            chart_baro_t.Series.Clear();
            chart_baro_t.Series.Add(serie_baro_t);

            chart_hz.Series.Clear();
            chart_hz.Series.Add(serie_i2c_hz);
            chart_hz.Series.Add(serie_adc_hz);
            chart_hz.Series.Add(serie_pru_hz);
            chart_hz.Series.Add(serie_pilot_hz);


            backgroundWorker_joystick.RunWorkerAsync();

            timer_charts.Interval = (int)( 1000.0 / 60.0);
            timer_charts.Start();
        }
       

        public string sendrecv(string ip,string json, int timeout)
        {
            DateTime start = DateTime.Now;
            string returnData = "";
            try
            {
                IPEndPoint RemoteIpEndPoint = new IPEndPoint(IPAddress.Any, 0);


                Byte[] sendBytes = Encoding.ASCII.GetBytes(json);
                
                


                UdpClient udpClient = new UdpClient(ip, 32000);
                udpClient.Client.SendTimeout = 1000;
                udpClient.Client.ReceiveTimeout = 1000;
                udpClient.Send(sendBytes, sendBytes.Length);
                Byte[] receiveBytes = udpClient.Receive(ref RemoteIpEndPoint);
                udpClient.Close();


               

                returnData = Encoding.ASCII.GetString(receiveBytes);


            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.StackTrace);
              
            }
            rtt = DateTime.Now.Subtract(start).TotalMilliseconds;
            return returnData;
        }


        bool controle_conectado = false;

        int rxcount = 0;
        private void backgroundWorker1_DoWork(object sender, DoWorkEventArgs e)
        {

            while (true)
            {
                try
                {
                    //atenção com versao do slimdx no nuget, verifique se o runtime é 4.0 e instale tambem o slimdx runtime 4.0
                    //se a versão do runtime for 2.0 é preciso adicionar no app.config <startup useLegacyV2RuntimeActivationPolicy="true">
                    Gamepad_State_SlimDX joy = new Gamepad_State_SlimDX(SlimDX.XInput.UserIndex.One);
                    joy.Update();

                    controle_conectado=joy.Connected;

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

                    string fromdrone = sendrecv(ip,jsonjoy.ToString(), 200);
                    if (fromdrone.Length > 0)
                    {
                        Console.WriteLine(fromdrone);

                        JObject json = JObject.Parse(fromdrone);

                        vbat = (double)json["vbat"];

                        motor_fl = (double)json["motor_fl"];
                        motor_fr = (double)json["motor_fr"];
                        motor_rl = (double)json["motor_rl"];
                        motor_rr = (double)json["motor_rr"];


                        pitch_gyro = (float)json["pitch_gyro"];
                        roll_gyro = (float)json["roll_gyro"];
                        yaw_gyro = (float)json["yaw_gyro"];

                        fusion_pitch = (float)json["fusion_pitch"];
                        fusion_roll = (float)json["fusion_roll"];

                        pitch_acc = (float)json["pitch_acc"];
                        roll_acc = (float)json["roll_acc"];

                        baro_h = (float)json["baro_h"];
                        baro_t = (float)json["baro_t"];
                        baro_p = (float)json["baro_p"];

                        i2c_hz = (float)json["i2c_hz"];
                        adc_hz = (float)json["adc_hz"];
                        pru_hz = (float)json["pru_hz"];
                        pilot_hz = (float)json["pilot_hz"];

                        //Console.WriteLine(fusion_pitch);
                       // Console.WriteLine(fusion_roll);

                        rxcount++;
                    }


                    Thread.Sleep((int)( 1000.0 / 60.0));

                   
                      
                  
                    
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.ToString());
                }

            }
        }

        //
        float pitch_gyro = 0;
        float roll_gyro = 0;
        float yaw_gyro = 0;
        //
        float fusion_pitch = 0;
        float fusion_roll = 0;
        //
        float pitch_acc = 0;
        float roll_acc = 0;


        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            Environment.Exit(0);
        }


        private void timer_charts_Tick(object sender, EventArgs e)
        {
            try
            {
                if (rxcount == 0) return;

                serie_vbat.circular_append_y(vbat, 1000);
                serie_rtt.circular_append_y(rtt, 1000);

                serie_motor_fl.circular_append_y(motor_fl / 1000.0, 1000);
                serie_motor_fr.circular_append_y(motor_fr / 1000.0, 1000);
                serie_motor_rl.circular_append_y(motor_rl / 1000.0, 1000);
                serie_motor_rr.circular_append_y(motor_rr / 1000.0, 1000);

                serie_baro_p.circular_append_y(baro_p, 1000);
                serie_baro_t.circular_append_y(baro_t, 1000);
                serie_baro_h.circular_append_y(baro_h, 1000);

                serie_adc_hz.circular_append_y(adc_hz, 1000);
                serie_pru_hz.circular_append_y(pru_hz, 1000);
                serie_i2c_hz.circular_append_y(i2c_hz, 1000);
                serie_pilot_hz.circular_append_y(pilot_hz, 1000);


                chart_rtt.ChartAreas[0].AxisY.IsStartedFromZero = false;
                chart_rtt.ChartAreas[0].RecalculateAxesScale();

                chart_vbat.ChartAreas[0].AxisY.IsStartedFromZero = false;
                chart_vbat.ChartAreas[0].RecalculateAxesScale();

                chart_motor.ChartAreas[0].AxisY.IsStartedFromZero = false;
                chart_motor.ChartAreas[0].RecalculateAxesScale();

                chart_baro_h.ChartAreas[0].AxisY.IsStartedFromZero = false;
                chart_baro_h.ChartAreas[0].RecalculateAxesScale();

                chart_baro_t.ChartAreas[0].AxisY.IsStartedFromZero = false;
                chart_baro_t.ChartAreas[0].RecalculateAxesScale();

                chart_baro_p.ChartAreas[0].AxisY.IsStartedFromZero = false;
                chart_baro_p.ChartAreas[0].RecalculateAxesScale();


                chart_hz.ChartAreas[0].AxisY.IsStartedFromZero = false;
                chart_hz.ChartAreas[0].RecalculateAxesScale();


                if (controle_conectado)
                {
                    button_controle.BackColor = Color.Green;
                    button_controle.ForeColor = Color.White;
                    button_controle.Text = "Controle Xbox conectado";
                }
                else {
                    button_controle.BackColor = Color.Red;
                    button_controle.ForeColor = Color.White;
                    button_controle.Text = "Controle Xbox desconectado";
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }


        #region OpenGL

        void timer_animacao_Tick(object sender, EventArgs e)
        {
            try
            {
                    Render();
            }
            catch (Exception ex) { 
            
            }
        }
        bool loaded = false;
        System.Windows.Forms.Timer timer_animacao = new System.Windows.Forms.Timer();

        private void glControl1_Load(object sender, EventArgs e)
        {
            loaded = true;
            timer_animacao.Tick+=timer_animacao_Tick;
            timer_animacao.Interval = 30;
            timer_animacao.Start();
            
            GL.ClearColor(Color.MidnightBlue);
            GL.Enable(EnableCap.DepthTest);

            // Ensure that the viewport and projection matrix are set correctly.
            glControl_Resize(glControl1, EventArgs.Empty);
      
        }

        private void glControl1_Paint(object sender, PaintEventArgs e)
        {
            if (!loaded)
                return;
            Render();
        }

        private void glControl1_Resize(object sender, EventArgs e)
        {
            if (!loaded)
                return;
            glControl_Resize(sender, e);
        }

        void glControl_Resize(object sender, EventArgs e)
        {
            OpenTK.GLControl c = sender as OpenTK.GLControl;

            if (c.ClientSize.Height == 0)
                c.ClientSize = new System.Drawing.Size(c.ClientSize.Width, 1);

            GL.Viewport(0, 0, c.ClientSize.Width, c.ClientSize.Height);


            GL.MatrixMode(MatrixMode.Projection);

            float aspect_ratio = Width / (float)Height;
            Matrix4 perpective = Matrix4.CreatePerspectiveFieldOfView(MathHelper.PiOver4, aspect_ratio, 1, 64);
            GL.LoadMatrix(ref perpective);
        }

        private void DrawCube()
        {
            GL.Begin(BeginMode.Quads);

            //
            GL.Color3(Color.Cyan);
            GL.Vertex3(-1.0f, -1.0f, -1.0f);
            GL.Vertex3(-1.0f, 1.0f, -1.0f);
            GL.Vertex3(1.0f, 1.0f, -1.0f);
            GL.Vertex3(1.0f, -1.0f, -1.0f);

            GL.Color3(Color.Honeydew);
            GL.Vertex3(-1.0f, -1.0f, -1.0f);
            GL.Vertex3(1.0f, -1.0f, -1.0f);
            GL.Vertex3(1.0f, -1.0f, 1.0f);
            GL.Vertex3(-1.0f, -1.0f, 1.0f);

            GL.Color3(Color.Moccasin);

            GL.Vertex3(-1.0f, -1.0f, -1.0f);
            GL.Vertex3(-1.0f, -1.0f, 1.0f);
            GL.Vertex3(-1.0f, 1.0f, 1.0f);
            GL.Vertex3(-1.0f, 1.0f, -1.0f);

            GL.Color3(Color.Red);
            GL.Vertex3(-1.0f, -1.0f, 1.0f);
            GL.Vertex3(1.0f, -1.0f, 1.0f);
            GL.Vertex3(1.0f, 1.0f, 1.0f);
            GL.Vertex3(-1.0f, 1.0f, 1.0f);

            GL.Color3(Color.PaleVioletRed);
            GL.Vertex3(-1.0f, 1.0f, -1.0f);
            GL.Vertex3(-1.0f, 1.0f, 1.0f);
            GL.Vertex3(1.0f, 1.0f, 1.0f);
            GL.Vertex3(1.0f, 1.0f, -1.0f);

            GL.Color3(Color.ForestGreen);
            GL.Vertex3(1.0f, -1.0f, -1.0f);
            GL.Vertex3(1.0f, 1.0f, -1.0f);
            GL.Vertex3(1.0f, 1.0f, 1.0f);
            GL.Vertex3(1.0f, -1.0f, 1.0f);

            GL.End();
        }


        TextRenderer renderer;
        Font serif = new Font(FontFamily.GenericSerif, 24);
        Font sans = new Font(FontFamily.GenericSansSerif, 24);
        Font mono = new Font(FontFamily.GenericMonospace, 24);


        private void Render()
        {
            Matrix4 lookat2 = Matrix4.LookAt(new Vector3(0,5,30), Vector3.Zero, Vector3.UnitY);
            GL.MatrixMode(MatrixMode.Modelview);
            GL.LoadMatrix(ref lookat2);

            GL.Rotate((float)(fusion_pitch * 180.0/Math.PI), Vector3.UnitX);
            GL.Rotate((float)(fusion_roll * 180.0 / Math.PI), Vector3.UnitZ);
            //GL.Rotate((float)(yaw_gyro * 180.0 / Math.PI), Vector3.UnitY);
            

            GL.Clear(ClearBufferMask.ColorBufferBit | ClearBufferMask.DepthBufferBit);


            GL.Begin(BeginMode.Lines);
            GL.Color3(Color.Red);
            GL.Vertex3(0, 0, 0);
            GL.Vertex3(10, 0, 0);
            GL.End();

            GL.Begin(BeginMode.Lines);
            GL.Color3(Color.Green);
            GL.Vertex3(0, 0, 0);
            GL.Vertex3(0, 10, 0);
            GL.End();

            GL.Begin(BeginMode.Lines);
            GL.Color3(Color.Blue);
            GL.Vertex3(0, 0, 0);
            GL.Vertex3(0, 0, 10);
            GL.End();

            

            DrawCube();

            glControl1.SwapBuffers();
        }



        #endregion


    }
}
