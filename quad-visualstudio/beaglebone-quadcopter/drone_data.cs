using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace beaglebone_quadcopter
{
    public class drone_data
    {
        //
        public float pitch_gyro = 0;
        public float roll_gyro = 0;
        public float yaw_gyro = 0;
        //
        public float fusion_pitch = 0;
        public float fusion_roll = 0;
        //
        public float pitch_acc = 0;
        public float roll_acc = 0;
        //
        public double pilot_pitch = 0;
        public double pilot_roll = 0;

        public double accx = 0.0;
        public double accy = 0.0;
        public double accz = 0.0;
        public murix_series serie_accx = new murix_series("accx (g-force)");
        public murix_series serie_accy = new murix_series("accy (g-force)");
        public murix_series serie_accz = new murix_series("accz (g-force)");


        public double gyrox = 0.0;
        public double gyroy = 0.0;
        public double gyroz = 0.0;
        public murix_series serie_gyrox = new murix_series("gyrox (rad/s) - roll");
        public murix_series serie_gyroy = new murix_series("gyroy (rad/s) - pitch");
        public murix_series serie_gyroz = new murix_series("gyroz (rad/s) - yaw");


        public double vbat = 0.0;
        public murix_series serie_vbat = new murix_series("battery (volt)");

        public double rtt = 0.0;
        public murix_series serie_rtt = new murix_series("rtt (ms)");


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
        public double baro_hema = 0.0;
        public double baro_t = 0.0;
        public murix_series serie_baro_p = new murix_series("baro_p (hpa)");
        public murix_series serie_baro_h = new murix_series("baro_h (meters)");
        public murix_series serie_baro_hema = new murix_series("baro_hema (meters)");
        public murix_series serie_baro_t = new murix_series("baro_t (celcius)");



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
