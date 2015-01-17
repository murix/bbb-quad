using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace beaglebone_quadcopter
{
    public class drone_data
    {

        public double euler_degree0 = 0;
        public double euler_degree1 = 0;
        public double euler_degree2 = 0;
        //
        public murix_series serie_euler0 = new murix_series("euler[0]");
        public murix_series serie_euler1 = new murix_series("euler[1]");
        public murix_series serie_euler2 = new murix_series("euler[2]");




        public double gps_lat = 0;
        public double gps_long = 0;
        public murix_series serie_gps_lat = new murix_series("gps_lat");
        public murix_series serie_gps_long = new murix_series("gps_long");


        public double mpu6050_temp = 0.0;
        public murix_series serie_mpu6050_temp = new murix_series("mpu6050 (celcius)");

        public double magx = 0.0;
        public double magy = 0.0;
        public double magz = 0.0;
        public murix_series serie_magx = new murix_series("magx (gauss)");
        public murix_series serie_magy = new murix_series("magy (gauss)");
        public murix_series serie_magz = new murix_series("magz (gauss)");


        public double accx = 0.0;
        public double accy = 0.0;
        public double accz = 0.0;
        public murix_series serie_accx = new murix_series("accx (g-force)");
        public murix_series serie_accy = new murix_series("accy (g-force)");
        public murix_series serie_accz = new murix_series("accz (g-force)");


        public double gyrox = 0.0;
        public double gyroy = 0.0;
        public double gyroz = 0.0;
        public murix_series serie_gyrox = new murix_series("gyrox (degree/s) - roll");
        public murix_series serie_gyroy = new murix_series("gyroy (degree/s) - pitch");
        public murix_series serie_gyroz = new murix_series("gyroz (degree/s) - yaw");


        public double vbat = 0.0;
        public murix_series serie_vbat = new murix_series("battery (volt)");

        public double rtt = 0.0;
        public murix_series serie_rtt = new murix_series("net rtt (ms)");


        public double motor_fl = 0.0;
        public double motor_fr = 0.0;
        public double motor_rl = 0.0;
        public double motor_rr = 0.0;
        public murix_series serie_motor_fl = new murix_series("motor_fl (us)");
        public murix_series serie_motor_fr = new murix_series("motor_fr (us)");
        public murix_series serie_motor_rl = new murix_series("motor_rl (us)");
        public murix_series serie_motor_rr = new murix_series("motor_rr (us)");


        public double baro_p = 0.0;
        public double baro_h = 0.0;
        public double baro_t = 0.0;
        public murix_series serie_baro_p = new murix_series("baro_p");
        public murix_series serie_baro_h = new murix_series("baro_h");
        public murix_series serie_baro_t = new murix_series("baro_t");

        //
        public double i2c_hz = 0;
        public double pru_hz = 0;
        public double adc_hz = 0;
        public double pilot_hz = 0;
        public murix_series serie_i2c_hz = new murix_series("i2c sensors (Hz)");
        public murix_series serie_pru_hz = new murix_series("pru motors (Hz)");
        public murix_series serie_adc_hz = new murix_series("adc battery (Hz)");
        public murix_series serie_pilot_hz = new murix_series("pilot PID (Hz)");
    }
}
