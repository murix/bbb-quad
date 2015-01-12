using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms.DataVisualization.Charting;

namespace beaglebone_quadcopter
{
    public class murix_series : Series
    {

        double circular_count = 0;
        public murix_series(string name)
            : base(name)
        {
            this.ChartType = SeriesChartType.StepLine;
            // this.XValueType = ChartValueType.Time;
           // this.XValueType = ChartValueType.DateTime;
        }
        public void circular_append_y(double value, int max_items)
        {
            //max_items = 100;
            try
            {
                while (this.Points.Count > max_items) this.Points.RemoveAt(0);
                this.Points.AddY(value);
                circular_count++;
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }
    }
}
