using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace beaglebone_quadcopter
{
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
            try
            {
                MethodInvoker action = delegate { _output.AppendText(value.ToString()); };
                _output.BeginInvoke(action);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }

        public override Encoding Encoding
        {
            get { return System.Text.Encoding.UTF8; }
        }
    }

}
