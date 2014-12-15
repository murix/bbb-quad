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

        

        private void timer1_Tick(object sender, EventArgs e)
        {
            
            
      

            

        }


        public class TextBoxStreamWriter : TextWriter
        {
            TextBox _output = null;

            public TextBoxStreamWriter(TextBox output)
            {
                _output = output;
            }

            public override void Write(char value)
            {
                //cross thread
                MethodInvoker action = delegate { _output.AppendText(value.ToString()); };
                _output.BeginInvoke(action);
            }

            public override Encoding Encoding
            {
                get { return System.Text.Encoding.UTF8; }
            }
        }

        // That's our custom TextWriter class
        TextBoxStreamWriter _writer = null;


        public class Gamepad_State_SlimDX
        {
            uint lastPacket;

            public Gamepad_State_SlimDX(UserIndex userIndex)
            {
                UserIndex = userIndex;
                Controller = new Controller(userIndex);
            }

            public readonly UserIndex UserIndex;
            public readonly Controller Controller;

            public DPadState DPad { get; private set; }
            public ThumbstickState LeftStick { get; private set; }
            public ThumbstickState RightStick { get; private set; }

            public bool A { get; private set; }
            public bool B { get; private set; }
            public bool X { get; private set; }
            public bool Y { get; private set; }

            public bool RightShoulder { get; private set; }
            public bool LeftShoulder { get; private set; }

            public bool Start { get; private set; }
            public bool Back { get; private set; }

            public float RightTrigger { get; private set; }
            public float LeftTrigger { get; private set; }

            public bool Connected
            {
                get { return Controller.IsConnected; }
            }

            public void Vibrate(float leftMotor, float rightMotor)
            {
                Controller.SetVibration(new Vibration
                {
                    LeftMotorSpeed = (ushort)(MathHelper.Saturate(leftMotor) * ushort.MaxValue),
                    RightMotorSpeed = (ushort)(MathHelper.Saturate(rightMotor) * ushort.MaxValue)
                });
            }

            public void Update()
            {
                // If not connected, nothing to update
                if (!Connected) return;

                // If same packet, nothing to update
                State state = Controller.GetState();
                if (lastPacket == state.PacketNumber) return;
                lastPacket = state.PacketNumber;

                var gamepadState = state.Gamepad;

                // Shoulders
                LeftShoulder = (gamepadState.Buttons & GamepadButtonFlags.LeftShoulder) != 0;
                RightShoulder = (gamepadState.Buttons & GamepadButtonFlags.RightShoulder) != 0;

                // Triggers
                LeftTrigger = gamepadState.LeftTrigger / (float)byte.MaxValue;
                RightTrigger = gamepadState.RightTrigger / (float)byte.MaxValue;

                // Buttons
                Start = (gamepadState.Buttons & GamepadButtonFlags.Start) != 0;
                Back = (gamepadState.Buttons & GamepadButtonFlags.Back) != 0;

                A = (gamepadState.Buttons & GamepadButtonFlags.A) != 0;
                B = (gamepadState.Buttons & GamepadButtonFlags.B) != 0;
                X = (gamepadState.Buttons & GamepadButtonFlags.X) != 0;
                Y = (gamepadState.Buttons & GamepadButtonFlags.Y) != 0;

                // D-Pad
                DPad = new DPadState((gamepadState.Buttons & GamepadButtonFlags.DPadUp) != 0,
                                     (gamepadState.Buttons & GamepadButtonFlags.DPadDown) != 0,
                                     (gamepadState.Buttons & GamepadButtonFlags.DPadLeft) != 0,
                                     (gamepadState.Buttons & GamepadButtonFlags.DPadRight) != 0);

                // Thumbsticks
                LeftStick = new ThumbstickState(
                    Normalize(gamepadState.LeftThumbX, gamepadState.LeftThumbY, Gamepad.GamepadLeftThumbDeadZone),
                    (gamepadState.Buttons & GamepadButtonFlags.LeftThumb) != 0);
                RightStick = new ThumbstickState(
                    Normalize(gamepadState.RightThumbX, gamepadState.RightThumbY, Gamepad.GamepadRightThumbDeadZone),
                    (gamepadState.Buttons & GamepadButtonFlags.RightThumb) != 0);
            }

            static Vector2 Normalize(short rawX, short rawY, short threshold)
            {
                var value = new Vector2(rawX, rawY);
                var magnitude = value.Length();
                var direction = value / (magnitude == 0 ? 1 : magnitude);

                var normalizedMagnitude = 0.0f;
                if (magnitude - threshold > 0)
                    normalizedMagnitude = Math.Min((magnitude - threshold) / (short.MaxValue - threshold), 1);

                return direction * normalizedMagnitude;
            }

            public struct DPadState
            {
                public readonly bool Up, Down, Left, Right;

                public DPadState(bool up, bool down, bool left, bool right)
                {
                    Up = up; Down = down; Left = left; Right = right;
                }
            }

            public struct ThumbstickState
            {
                public readonly Vector2 Position;
                public readonly bool Clicked;

                public ThumbstickState(Vector2 position, bool clicked)
                {
                    Clicked = clicked;
                    Position = position;
                }
            }
        }

        public static class MathHelper
        {
            public static float Saturate(float value)
            {
                return value < 0 ? 0 : value > 1 ? 1 : value;
            }
        }

        public class murix_series : Series
        {

            double circular_count = 0;
            public murix_series(string name)
                : base(name)
            {
                this.ChartType = SeriesChartType.Line;
            }
            public void circular_append_y(double value, int max_items)
            {
                while (this.Points.Count > max_items) this.Points.RemoveAt(0);
                this.Points.AddY(value);
                circular_count++;
            }
        }

        double vbat = 0.0;
        private void Form1_Load(object sender, EventArgs e)
        {
            _writer = new TextBoxStreamWriter(this.textBox1);
            Console.SetOut(_writer);

            backgroundWorker1.WorkerReportsProgress = true;
            backgroundWorker1.ProgressChanged += backgroundWorker1_ProgressChanged;
            backgroundWorker1.RunWorkerAsync();
            chart1.Series.Clear();
            chart1.Series.Add(serie_vbat);
        }

        murix_series serie_vbat = new murix_series("vbat");

        double framerate = 25.0;
        double serie_len_in_seconds = 60;


        void backgroundWorker1_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            serie_vbat.circular_append_y(vbat, (int)(framerate * serie_len_in_seconds));
            //chart1.ChartAreas[0].AxisY.Maximum = ping_timeout;
           // chart1.ChartAreas[0].AxisY.Minimum = 0;
            chart1.ChartAreas[0].AxisY.IsStartedFromZero = false;
            chart1.ChartAreas[0].RecalculateAxesScale();
        }

        private void backgroundWorker1_DoWork(object sender, DoWorkEventArgs e)
        {
          
            //Gamepad_State_SlimDX joy = new Gamepad_State_SlimDX(UserIndex.One);


            int timeslice = (int)((1.0 / framerate) * 1000.0);

            UdpClient udpClient = new UdpClient("192.168.8.50", 32000);
            while (true) {
                try
                {
                    //joy.Update();
                    

                    Byte[] sendBytes = Encoding.ASCII.GetBytes("Is anybody there");
                    udpClient.Send(sendBytes, sendBytes.Length);

                    /*
                    IPEndPoint RemoteIpEndPoint = new IPEndPoint(IPAddress.Any, 0);
                    Byte[] receiveBytes = udpClient.Receive(ref RemoteIpEndPoint);
                    string returnData = Encoding.ASCII.GetString(receiveBytes);
                    */

                    //
                    string returnData = "";

                    DateTime start = DateTime.Now;
                    while (true)
                    {
                        //
                        TimeSpan timeItTook = DateTime.Now - start;

                        //exit by timeout
                        if (timeItTook.Milliseconds > timeslice)
                        {
                            break;
                        }

                        //exit by receive data
                        if (udpClient.Available > 0)
                        {
                            IPEndPoint RemoteIpEndPoint = new IPEndPoint(IPAddress.Any, 0);
                            Byte[] receiveBytes = udpClient.Receive(ref RemoteIpEndPoint);
                            returnData = Encoding.ASCII.GetString(receiveBytes);
                        }
                        Thread.Sleep(1);
                    }


                    //Console.WriteLine(returnData);
                    if (returnData.Length > 0)
                    {
                        JObject json = JObject.Parse(returnData);
                        vbat = (double)json["vbat"];
                    }
                    else {
                        Console.WriteLine(DateTime.Now+" timeout");
                    }
                    //Console.WriteLine(vbat);

                    backgroundWorker1.ReportProgress(1);

                    Thread.Sleep(timeslice);
                    Thread.Yield();

                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.ToString());
                }

            }
        }
    }
}
