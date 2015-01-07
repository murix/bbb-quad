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



        drone_data drone = new drone_data();

        private void textBox_ip_TextChanged(object sender, EventArgs e)
        {
            //atualizar configuracoes
            Properties.Settings.Default.ip = textBox_ip.Text;
        }


        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            try
            {
              
                //salvar configuracoes
                Properties.Settings.Default.Save();

                timer_animacao.Stop();
                timer_charts.Stop();
                backgroundWorker_joystick.CancelAsync();

                // Thread.Sleep(100);

                //Environment.Exit(0);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }

        private void Form1_Load(object sender, EventArgs e)
        {
           //carregar configuracoes
            textBox_ip.Text = (string) Properties.Settings.Default["ip"];


            chart_rtt.Series.Clear();
            chart_rtt.Series.Add(drone.serie_rtt);

            chart_vbat.Series.Clear();
            chart_vbat.Series.Add(drone.serie_vbat);

            chart_motor.Series.Clear();
            chart_motor.Series.Add(drone.serie_motor_fl);
            chart_motor.Series.Add(drone.serie_motor_fr);
            chart_motor.Series.Add(drone.serie_motor_rl);
            chart_motor.Series.Add(drone.serie_motor_rr);

            chart_baro_h.Series.Clear();
            chart_baro_h.Series.Add(drone.serie_baro_h);
            chart_baro_h.Series.Add(drone.serie_baro_hema);

            chart_baro_p.Series.Clear();
            chart_baro_p.Series.Add(drone.serie_baro_p);

            chart_baro_t.Series.Clear();
            chart_baro_t.Series.Add(drone.serie_baro_t);
            chart_baro_t.Series.Add(drone.serie_mpu6050_temp);
            

            chart_hz.Series.Clear();
            chart_hz.Series.Add(drone.serie_i2c_hz);
            chart_hz.Series.Add(drone.serie_adc_hz);
            chart_hz.Series.Add(drone.serie_pru_hz);
            chart_hz.Series.Add(drone.serie_pilot_hz);

            chart_speed.Series.Clear();
            chart_speed.Series.Add(drone.serie_gyrox);
            chart_speed.Series.Add(drone.serie_gyroy);
            chart_speed.Series.Add(drone.serie_gyroz);

            chart_accel.Series.Clear();
            chart_accel.Series.Add(drone.serie_accx);
            chart_accel.Series.Add(drone.serie_accy);
            chart_accel.Series.Add(drone.serie_accz);

            chart_gps.Series.Clear();
            chart_gps.Series.Add(drone.serie_gps_lat);
            chart_gps.Series.Add(drone.serie_gps_long);

            chart_mag.Series.Clear();
            chart_mag.Series.Add(drone.serie_magx);
            chart_mag.Series.Add(drone.serie_magy);
            chart_mag.Series.Add(drone.serie_magz);

            chart_pitch.Series.Clear();
            chart_pitch.Series.Add(drone.serie_gyro_pitch);
            chart_pitch.Series.Add(drone.serie_acc_pitch);
            chart_pitch.Series.Add(drone.serie_fusion_pitch);

            chart_roll.Series.Clear();
            chart_roll.Series.Add(drone.serie_gyro_roll);
            chart_roll.Series.Add(drone.serie_acc_roll);
            chart_roll.Series.Add(drone.serie_fusion_roll);

            chart_yaw.Series.Clear();
            chart_yaw.Series.Add(drone.serie_gyro_yaw);
            chart_yaw.Series.Add(drone.serie_mag_head);


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
            drone.rtt = DateTime.Now.Subtract(start).TotalMilliseconds;
            return returnData;
        }


        bool controle_conectado = false;
       

        int rxcount = 0;
        private void backgroundWorker1_DoWork(object sender, DoWorkEventArgs e)
        {

            string fromdrone = "";

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



                    string ip = Properties.Settings.Default.ip;
                  

                    fromdrone = sendrecv(ip,jsonjoy.ToString(), 200);
                    if (fromdrone.Length > 0)
                    {
                        //Console.WriteLine(fromdrone);

                        JObject json = JObject.Parse(fromdrone);

                        drone.vbat = (double)json["vbat"];

                        drone.motor_fl = (double)json["motor_fl"];
                        drone.motor_fr = (double)json["motor_fr"];
                        drone.motor_rl = (double)json["motor_rl"];
                        drone.motor_rr = (double)json["motor_rr"];


                        drone.gyro_pitch = (double)json["pitch_gyro"];
                        drone.gyro_roll = (double)json["roll_gyro"];
                        drone.gyro_yaw = (double)json["yaw_gyro"];

                        drone.fusion_pitch = (double)json["fusion_pitch"];
                        drone.fusion_roll = (double)json["fusion_roll"];

                        drone.acc_pitch = (double)json["pitch_acc"];
                        drone.acc_roll = (double)json["roll_acc"];



                        drone.baro_hema = (double)json["baro_hema"];
                        drone.baro_h = (double)json["baro_h"];
                        drone.baro_t = (double)json["baro_t"];
                        drone.baro_p = (double)json["baro_p"];

                        drone.i2c_hz = (double)json["i2c_hz"];
                        drone.adc_hz = (double)json["adc_hz"];
                        drone.pru_hz = (double)json["pru_hz"];
                        drone.pilot_hz = (double)json["pilot_hz"];

                        drone.gyrox = (double)json["gyro_x"] ;
                        drone.gyroy = (double)json["gyro_y"] ;
                        drone.gyroz = (double)json["gyro_z"] ;

                        drone.accx = (double)json["acc_x"];
                        drone.accy = (double)json["acc_y"];
                        drone.accz = (double)json["acc_z"];

                        drone.magx = (double)json["mag_x"];
                        drone.magy = (double)json["mag_y"];
                        drone.magz = (double)json["mag_z"];

                        drone.mag_head = (double)json["mag_head"];


                        drone.pilot_pitch = (double)json["pilot_pitch"];
                        drone.pilot_roll = (double)json["pilot_roll"];
                        drone.pilot_yaw = (double)json["pilot_yaw"];

                        drone.e0 = (double)json["e0"];
                        drone.e1 = (double)json["e1"];
                        drone.e2 = (double)json["e2"];

                        drone.mpu6050_temp = (double)json["mpu6050_temp"]-1.2;

                        //Console.WriteLine(fusion_pitch);
                       // Console.WriteLine(fusion_roll);

                        rxcount++;
                    }


                    Thread.Sleep((int)( 1000.0 / 60.0));

                   
                      
                  
                    
                }
                catch (Exception ex)
                {
                    Console.WriteLine(fromdrone);
                    Console.WriteLine(ex.ToString());
                }

            }
        }

    




        private void timer_charts_Tick(object sender, EventArgs e)
        {
            try
            {
                if (rxcount == 0) return;

                int items = 300;

                drone.serie_vbat.circular_append_y(drone.vbat, items);
                drone.serie_rtt.circular_append_y(drone.rtt, items);

                drone.serie_motor_fl.circular_append_y(drone.motor_fl / 1000.0, items);
                drone.serie_motor_fr.circular_append_y(drone.motor_fr / 1000.0, items);
                drone.serie_motor_rl.circular_append_y(drone.motor_rl / 1000.0, items);
                drone.serie_motor_rr.circular_append_y(drone.motor_rr / 1000.0, items);

                drone.serie_baro_p.circular_append_y(drone.baro_p, items);
                drone.serie_baro_t.circular_append_y(drone.baro_t, items);
                drone.serie_baro_h.circular_append_y(drone.baro_h, items);
                drone.serie_baro_hema.circular_append_y(drone.baro_hema, items);

                drone.serie_adc_hz.circular_append_y(drone.adc_hz, items);
                drone.serie_pru_hz.circular_append_y(drone.pru_hz, items);
                drone.serie_i2c_hz.circular_append_y(drone.i2c_hz, items);
                drone.serie_pilot_hz.circular_append_y(drone.pilot_hz, items);

                drone.serie_gyrox.circular_append_y(drone.gyrox, items);
                drone.serie_gyroy.circular_append_y(drone.gyroy, items);
                drone.serie_gyroz.circular_append_y(drone.gyroz, items);

                drone.serie_accx.circular_append_y(drone.accx, items);
                drone.serie_accy.circular_append_y(drone.accy, items);
                drone.serie_accz.circular_append_y(drone.accz, items);

                drone.serie_gps_lat.circular_append_y(drone.gps_lat, items);
                drone.serie_gps_long.circular_append_y(drone.gps_long, items);

                drone.serie_magx.circular_append_y(drone.magx, items);
                drone.serie_magy.circular_append_y(drone.magy, items);
                drone.serie_magz.circular_append_y(drone.magz, items);

                drone.serie_mpu6050_temp.circular_append_y(drone.mpu6050_temp, items);


                ///////////////////////////////////////////////////////////////////

                drone.serie_gyro_pitch.circular_append_y(drone.gyro_pitch * 180.0 / Math.PI, items);
                drone.serie_acc_pitch.circular_append_y(drone.acc_pitch * 180.0 / Math.PI, items);
                drone.serie_fusion_pitch.circular_append_y(drone.fusion_pitch * 180.0 / Math.PI, items);


                drone.serie_gyro_roll.circular_append_y(drone.gyro_roll * 180.0 / Math.PI, items);
                drone.serie_acc_roll.circular_append_y(drone.acc_roll * 180.0 / Math.PI, items);
                drone.serie_fusion_roll.circular_append_y(drone.fusion_roll * 180.0 / Math.PI, items);

                drone.serie_gyro_yaw.circular_append_y(drone.gyro_yaw * 180.0 / Math.PI, items);

                drone.serie_mag_head.circular_append_y(drone.mag_head * 180.0 / Math.PI, items);


                ////////////////////////////////////////////////////////////////////////////////


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

                chart_mag.ChartAreas[0].AxisY.IsStartedFromZero = false;
                chart_mag.ChartAreas[0].RecalculateAxesScale();

                chart_gps.ChartAreas[0].AxisY.IsStartedFromZero = false;
                chart_gps.ChartAreas[0].RecalculateAxesScale();

                chart_pitch.ChartAreas[0].AxisY.IsStartedFromZero = false;
                chart_pitch.ChartAreas[0].RecalculateAxesScale();

                chart_roll.ChartAreas[0].AxisY.IsStartedFromZero = false;
                chart_roll.ChartAreas[0].RecalculateAxesScale();

                chart_yaw.ChartAreas[0].AxisY.IsStartedFromZero = false;
                chart_yaw.ChartAreas[0].RecalculateAxesScale();

                chart_hz.ChartAreas[0].AxisY.IsStartedFromZero = false;
                chart_hz.ChartAreas[0].RecalculateAxesScale();

                chart_speed.ChartAreas[0].AxisY.IsStartedFromZero = false;
                chart_speed.ChartAreas[0].RecalculateAxesScale();

                chart_accel.ChartAreas[0].AxisY.IsStartedFromZero = false;
                chart_accel.ChartAreas[0].RecalculateAxesScale();

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

            double aspect_ratio = Width / (double)Height;
            Matrix4 perpective = Matrix4.CreatePerspectiveFieldOfView(MathHelper.PiOver4,(float) aspect_ratio, 1, 64);
            GL.LoadMatrix(ref perpective);
        }

      
     
        


        TextRenderer renderer;
        Font serif = new Font(FontFamily.GenericSerif, 24);
        Font sans = new Font(FontFamily.GenericSansSerif, 24);
        Font mono = new Font(FontFamily.GenericMonospace, 24);


        private void Render()
        {
            Matrix4 lookat2 = Matrix4.LookAt(new Vector3(12,0,0), Vector3.Zero, Vector3.UnitY);
            GL.MatrixMode(MatrixMode.Modelview);
            GL.LoadMatrix(ref lookat2);

            OpenTK.Quaternion quad = new Quaternion();
            
            //GL.Rotate()

            //rotateZ(-Euler[2]);
            //rotateX(-Euler[1]);
            //rotateY(-Euler[0]);

            //GL.Rotate((float)(-drone.pilot_pitch * 180.0 / Math.PI), Vector3.UnitZ);
            //GL.Rotate((float)(-drone.pilot_yaw * 180.0 / Math.PI), Vector3.UnitY);
            //GL.Rotate((float)(-drone.pilot_roll * 180.0 / Math.PI), Vector3.UnitX);

            GL.Rotate((float)(-drone.e2 * 180.0 / Math.PI), Vector3.UnitZ);
            GL.Rotate((float)(-drone.e1 * 180.0 / Math.PI), Vector3.UnitX);
            GL.Rotate((float)(-drone.e0 * 180.0 / Math.PI), Vector3.UnitY);

            GL.Clear(ClearBufferMask.ColorBufferBit | ClearBufferMask.DepthBufferBit);


            int glen = 5;

            GL.LineWidth(5.0f);

            GL.Begin(BeginMode.Lines);
            GL.Color3(Color.Red);
            GL.Vertex3(0, 0, 0);
            GL.Vertex3(-glen * drone.accx, 0, 0); /////// X
            GL.End();

            GL.Begin(BeginMode.Lines);
            GL.Color3(Color.Green);
            GL.Vertex3(0, 0, 0);
            GL.Vertex3(0, -glen * drone.accz, 0); /////////// Y
            GL.End();

            GL.Begin(BeginMode.Lines);
            GL.Color3(Color.Blue);
            GL.Vertex3(0, 0, 0);
            GL.Vertex3(0, 0, glen*drone.accy); ///////////// z
            GL.End();

            GL.Begin(BeginMode.Lines);
            GL.Color3(Color.White);
            GL.Vertex3(0, 0, 0);
            GL.Vertex3(-glen * drone.accx, -glen * drone.accz, glen * drone.accy); ///////////// z
            GL.End();



            GL.Begin(BeginMode.Quads);
            //left
            GL.Color3(Color.Yellow);
            GL.Vertex3(-1.0f, -1.0f, -1.0f);
            GL.Vertex3(-1.0f, 1.0f, -1.0f);
            GL.Vertex3(1.0f, 1.0f, -1.0f);
            GL.Vertex3(1.0f, -1.0f, -1.0f);
            //bottom
            GL.Color3(Color.Black);
            GL.Vertex3(-1.0f, -1.0f, -1.0f);
            GL.Vertex3(1.0f, -1.0f, -1.0f);
            GL.Vertex3(1.0f, -1.0f, 1.0f);
            GL.Vertex3(-1.0f, -1.0f, 1.0f);
            //back
            GL.Color3(Color.Blue);
            GL.Vertex3(-1.0f, -1.0f, -1.0f);
            GL.Vertex3(-1.0f, -1.0f, 1.0f);
            GL.Vertex3(-1.0f, 1.0f, 1.0f);
            GL.Vertex3(-1.0f, 1.0f, -1.0f);
            //right
            GL.Color3(Color.Green);
            GL.Vertex3(-1.0f, -1.0f, 1.0f);
            GL.Vertex3(1.0f, -1.0f, 1.0f);
            GL.Vertex3(1.0f, 1.0f, 1.0f);
            GL.Vertex3(-1.0f, 1.0f, 1.0f);
            //top
            GL.Color3(Color.Red);
            GL.Vertex3(-1.0f, 1.0f, -1.0f);
            GL.Vertex3(-1.0f, 1.0f, 1.0f);
            GL.Vertex3(1.0f, 1.0f, 1.0f);
            GL.Vertex3(1.0f, 1.0f, -1.0f);
            //front
            GL.Color3(Color.White);
            GL.Vertex3(1.0f, -1.0f, -1.0f);
            GL.Vertex3(1.0f, 1.0f, -1.0f);
            GL.Vertex3(1.0f, 1.0f, 1.0f);
            GL.Vertex3(1.0f, -1.0f, 1.0f);
            //
            GL.End();

            glControl1.SwapBuffers();
        }



        #endregion

  


    }
}
