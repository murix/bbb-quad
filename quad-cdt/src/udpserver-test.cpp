/*
The MIT License (MIT)

Copyright (c) 1981-2014 Murilo Pontes

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */


//////////////////////-----------  C Standard Library header files
//#include <assert.h>	//Conditionally compiled macro that compares its argument to zero
//#include <complex.h> //(since C99)	Complex number arithmetic
//#include <ctype.h>	//Functions to determine the type contained in character data
//#include <errno.h>	//Macros reporting error conditions
//#include <fenv.h> //(since C99)	Floating-point environment
//#include <float.h>	//Limits of float types
//#include <inttypes.h> //(since C99)	Format conversion of integer types
//#include <iso646.h> //(since C95)	Alternative operator spellings
//#include <limits.h>	//Sizes of basic types
//#include <locale.h>	//Localization utilities
#include <math.h>	//Common mathematics functions
//#include <setjmp.h>	//Nonlocal jumps
//#include <signal.h>	//Signal handling
//#include <stdalign.h> //(since C11)	alignas and alignof convenience macros
//#include <stdarg.h>	//Variable arguments
//#include <stdatomic.h> //(since C11)	Atomic types
//#include <stdbool.h> //(since C99)	Boolean type
//#include <stddef.h>	//Common macro definitions
#include <stdint.h> //(since C99)	Fixed-width integer types
#include <stdio.h>	//Input/output
#include <stdlib.h>	//General utilities: memory management, program utilities, string conversions, random numbers
//#include <stdnoreturn.h> //(since C11)	noreturn convenience macros
#include <string.h>	//String handling
//#include <tgmath.h> //(since C99)	Type-generic math (macros wrapping math.h and complex.h)
//#include <threads.h> //(since C11)	Thread library
//#include <time.h> //Time/date utilities
//#include <uchar.h> //(since C11)	UTF-16 and UTF-32 character utilities
//#include <wchar.h> //(since C95)	Extended multibyte and wide character utilities
//#include <wctype.h> //(since C95)	Wide character classification and mapping utilities

/////////////////////-------- SYSTEM LIBRARIES

//
#include <sys/mman.h>

//socket
#include <sys/types.h>
#include <sys/socket.h>
//htonl
#include <arpa/inet.h>
//json
#include <jsoncpp/json/json.h>

///////////////////////---- PROJECT LIBRARIES
#include "bbb_i2c.h"
#include "mpu6050.h"
#include "hmc5883.h"
#include "ms5611.h"
#include "bbb-adc.h"
#include "pru-pwm-cpp.h"
#include "timestamps.h"
#include "freeimu_linux.h"



typedef struct {
	// ARM: float and uint32_t are thread safe


	//gravity
	float acc_x;
	float acc_y;
	float acc_z;
	//angular speed
	float gyro_x;
	float gyro_y;
	float gyro_z;

	//temperature
	float mpu6050_temp;

	//accel normalized
	float acc_n;

	//accel angles
	float acc_pitch;
	float acc_roll;

	//gyro integrate angles
	float gyro_pitch;
	float gyro_roll;
	float gyro_yaw;

	//fusion angles
	float fusion_pitch;
	float fusion_roll;



	float mag_x;
	float mag_y;
	float mag_z;
	float mag_n;
	float mag_heading;

	//
	float baro_p0;
	float baro_p;
	float baro_t;
	float baro_h;
	float baro_hema;


	//
	float gps_lat;
	float gps_long;


	//
	//
	float i2c_hz;
	float pru_hz;
	float adc_hz;
	float pilot_hz;


	//
	float pilot_offset_pitch;
	float pilot_offset_roll;
	float pilot_offset_yaw;
	float pilot_pitch;
	float pilot_roll;
	float pilot_yaw;

	//
	float vbat;


	//
	uint32_t motor_cmd;
	uint32_t motor_dutyns_now[8];
	uint32_t motor_dutyns_target[8];

	//
	bool joy_a;
	bool joy_b;
	bool joy_x;
	bool joy_y;

	bool joy_dpad_up;
	bool joy_dpad_down;
	bool joy_dpad_right;
	bool joy_dpad_left;

	float joy_stick_left_x;
	float joy_stick_left_y;
	float joy_stick_right_x;
	float joy_stick_right_y;


	bool ps3_init_ok;
	bool ps3_dpad_left;
	bool ps3_dpad_right;
	bool ps3_dpad_up;
	bool ps3_dpad_down;
	bool ps3_r3;
	bool ps3_r2;
	bool ps3_r1;
	bool ps3_select;
	bool ps3_power;
	bool ps3_start;
	bool ps3_l3;
	bool ps3_l2;
	bool ps3_l1;
	bool ps3_triangle;
	bool ps3_square;
	bool ps3_x;
	bool ps3_ball;
	float  ps3_lstick_x;
	float  ps3_lstick_y;
	float  ps3_rstick_x;
	float  ps3_rstick_y;
	int    ps3_accel[4];

	double q[4];
    double euler[3];
} drone_t;


#define VBAT_STABLE 8.0
#define SLEEP_1_SECOND (1000*1000)


double altimeter(double p0,double p,double t){
	//R=gas constant=8.31432
	//T=air temperature in measured kelvin
	//g=earth gravity=9.80665
	//M=molar mass of the gas=0.0289644
	double R=8.31432;
	double TK=t+273.15;
	double g=9.80665;
	double M=0.0289644;
	return ((R*TK)/(g*M))*log(p0/p);
}



void *task_adc(void *arg){

	drone_t* drone=(drone_t*) arg;
	adc_monitor adc;
	adc.init();

	//---------
	double t_back=get_timestamp_in_seconds();;
	double t_now=get_timestamp_in_seconds();;
	double t_diff=0;

	while(1){

		//-------------
		t_back=t_now;
		t_now=get_timestamp_in_seconds();
		t_diff=t_now-t_back;

		///////////////////////////////////

		drone->adc_hz = 1.0 / t_diff;

		////////////////////////////////////


		adc.update();
		drone->vbat=adc.vbat;

		usleep(1500);

	}
}


//
#define PWM_HZ                  400
#define PWM_FAILSAFE         500000
#define PWM_FLY_ARM          900000
#define PWM_CALIB_MIN       1000000
//#define PWM_FLY_MIN       1070000
#define PWM_FLY_MIN         1130000
//#define PWM_FLY_MAX         1430000
#define PWM_FLY_MAX         1900000
#define PWM_FLY_INTERVAL    (PWM_FLY_MAX-PWM_FLY_MIN)
#define PWM_CALIB_MAX       1950000
#define PWM_STEP_PER_CYCLE    10000
#define PWM_NS_PER_SEC     (1000*1000*1000)
#define PWM_CYCLE_IN_NS    (PWM_NS_PER_SEC/PWM_HZ)
#define PWM_CYCLE_IN_US    (PWM_CYCLE_IN_NS/1000)
#define MOTOR_CMD_NORMAL    0
#define MOTOR_CMD_CALIB_MIN 1
#define MOTOR_CMD_CALIB_MAX 2

void *task_motors(void *arg){
	drone_t* drone=(drone_t*) arg;




	// Initialise PRU PWM
	PRUPWM *myPWM = new PRUPWM(PWM_HZ);

	// Start the PRU
	myPWM->start();


	// initialize task variables
	drone->motor_cmd=MOTOR_CMD_NORMAL;
	for(int ch=0;ch<8;ch++){
		drone->motor_dutyns_target[ch]=PWM_FLY_ARM;
		drone->motor_dutyns_now[ch]=PWM_FLY_ARM;
		myPWM->setChannelValue(ch,PWM_FLY_ARM);
		myPWM->setFailsafeValue(ch,PWM_FAILSAFE);
	}
	//
	myPWM->setFailsafeTimeout(0);
	//myPWM->setFailsafeTimeout(2000);

	//---------
	double t_back=get_timestamp_in_seconds();;
	double t_now=get_timestamp_in_seconds();;
	double t_diff=0;

	double vbat_prev=0;
	double vbat_now=0;
	double vbat_diff=0;
    uint32_t pwm_step=PWM_STEP_PER_CYCLE;

	while(1){

		//-------------
		t_back=t_now;
		t_now=get_timestamp_in_seconds();
		t_diff=t_now-t_back;

		///////////////////////////////////

		drone->pru_hz = 1.0 / t_diff;

		////////////////////////////////////

		vbat_prev=vbat_now;
		vbat_now=drone->vbat;
		vbat_diff= vbat_now-vbat_prev;


		//incremental PWM
		if(drone->motor_cmd==MOTOR_CMD_NORMAL){
			for(int ch=0;ch<8;ch++){
				bool need_change=false;
				//
				if(drone->motor_dutyns_now[ch] < drone->motor_dutyns_target[ch] && drone->vbat > VBAT_STABLE){

					//printf("vbat_diff=%f\r\n",vbat_diff);
					//queda maior 0.1V
					if(vbat_diff<-0.1){
						pwm_step--; //-1us
						if(pwm_step<1) pwm_step=1;
						printf("queda maior 0.1v -> pwm_step=%d\r\n",pwm_step);
					}

					drone->motor_dutyns_now[ch] += pwm_step;
					if(drone->motor_dutyns_now[ch]>PWM_FLY_MAX) drone->motor_dutyns_now[ch]=PWM_FLY_MAX;
					need_change=true;
				}
				if(drone->motor_dutyns_now[ch] > drone->motor_dutyns_target[ch] ){
					drone->motor_dutyns_now[ch] -= PWM_STEP_PER_CYCLE;
					if(drone->motor_dutyns_now[ch]<PWM_FLY_ARM) drone->motor_dutyns_now[ch]=PWM_FLY_ARM;
					need_change=true;
				}

				if(need_change){
					myPWM->setChannelValue(ch,drone->motor_dutyns_now[ch]);
				}
			}
		}
		//calibration MIN
		if(drone->motor_cmd==MOTOR_CMD_CALIB_MIN){
			for(int ch=0;ch<8;ch++){
				bool need_change=false;
				if(drone->motor_dutyns_now[ch]!=PWM_CALIB_MIN){
					drone->motor_dutyns_now[ch]=PWM_CALIB_MIN;
					need_change=true;
				}
				if(need_change){
					myPWM->setChannelValue(ch,drone->motor_dutyns_now[ch]);
				}
			}

		}
		//calibration MAX
		if(drone->motor_cmd==MOTOR_CMD_CALIB_MAX){
			for(int ch=0;ch<8;ch++){
				bool need_change=false;
				if(drone->motor_dutyns_now[ch]!=PWM_CALIB_MAX){
					drone->motor_dutyns_now[ch]=PWM_CALIB_MAX;
					need_change=true;
				}
				if(need_change){
					myPWM->setChannelValue(ch,drone->motor_dutyns_now[ch]);
				}
			}
		}

		// wait pwm complete duty
		usleep(PWM_CYCLE_IN_US);

	}

}

void *task_imu(void *arg){
	drone_t* drone=(drone_t*) arg;

	//
	bbb_i2c i2c;
	i2c.open();
	//
	mpu6050 mpu(i2c.fd);
	//
	hmc5883 mag(i2c.fd);
	//
	ms5611 baro(i2c.fd);
	//

	double t_back=get_timestamp_in_seconds();;
	double t_now=get_timestamp_in_seconds();;
	double t_diff=0;

	drone->gyro_pitch=0;
	drone->gyro_roll=0;
	drone->gyro_yaw=0;
	drone->baro_hema=0;


	FreeIMU freeimu;
	double to_radian_per_second = M_PI/180.0;

	while(1){

		///////////////////////////////////////
		t_back=t_now;
		t_now=get_timestamp_in_seconds();
		t_diff=t_now-t_back;
		drone->i2c_hz = 1.0 / t_diff;
		///////////////////////////////////////

		mpu.update();
		mag.update();
		baro.update();

		//
		freeimu.getQ(drone->q,
				mpu.acc_g_x,mpu.acc_g_y,mpu.acc_g_z,
				mpu.gyro_degrees_x*to_radian_per_second,mpu.gyro_degrees_y*to_radian_per_second,mpu.gyro_degrees_z*to_radian_per_second,
				mag.mag_x,mag.mag_y,mag.mag_z);

		//
		freeimu.quaternation_to_euler_rad(drone->q,drone->euler);

		////////////////////////////////////////////////
		drone->acc_x=mpu.acc_g_x;
		drone->acc_y=mpu.acc_g_y;
		drone->acc_z=mpu.acc_g_z;
		drone->gyro_x=mpu.gyro_degrees_x*to_radian_per_second;
		drone->gyro_y=mpu.gyro_degrees_y*to_radian_per_second;
		drone->gyro_z=mpu.gyro_degrees_z*to_radian_per_second;
		drone->mpu6050_temp=mpu.temperate_celcius;
 		drone->mag_x=mag.mag_x;
		drone->mag_y=mag.mag_y;
		drone->mag_z=mag.mag_z;
		drone->mag_n=0;
		drone->mag_heading=0;


		////////////////////////////////////////////////////
		drone->gyro_roll =drone->euler[2];
		drone->gyro_pitch=drone->euler[1];
		drone->gyro_yaw  =drone->euler[0];
		drone->acc_pitch   =0;
		drone->acc_roll    =0;
		drone->fusion_pitch=0;
		drone->fusion_roll =0;
		///////////////////////////////////////////////


		///////////////////////////////////////////////
		drone->baro_p0=0;
		drone->baro_p=baro.P;
		drone->baro_t=baro.T;
		drone->baro_h=altimeter(1100,drone->baro_p,drone->baro_t);
		drone->baro_hema=drone->baro_h;//(drone->baro_hema*0.99)+(0.01*drone->baro_h);

		//////////////////////////////////////////////////
	}
}

#define MOTOR_FR 0
#define MOTOR_FL 1
#define MOTOR_RR 5
#define MOTOR_RL 7

float motor_clamp(float a,float b){
	if (a > b){
		return a;
	}
	else {
		return b;
	}
}

void *task_pilot(void *arg)
{
	drone_t* drone=(drone_t*) arg;


	drone->pilot_offset_pitch=0;
	drone->pilot_offset_roll=0;

	//---------
	double t_back=get_timestamp_in_seconds();;
	double t_now=get_timestamp_in_seconds();;
	double t_diff=0;

	bool takeoff=false;


	float ga_error=0;
	float ga_error_bak=0;
	float pid_rate_kp = 30000;

	while(1){

		//-------------
		t_back=t_now;
		t_now=get_timestamp_in_seconds();
		t_diff=t_now-t_back;

		///////////////////////////////////

		drone->pilot_hz = 1.0 / t_diff;

		///////////////////////////////////

		usleep(2500);



		/////////////////////////////////////////////////////////////////////////////
		if(drone->ps3_start){
			//printf("ps3 takeoff\r\n");
			takeoff=true;
		}
		if(drone->ps3_select){
			//printf("ps3 landing\r\n");
			takeoff=false;
		}
		///////////////////////////////////////////////////////////////////////////
		if(drone->ps3_triangle){
			//printf("ps3 trim\r\n");
			drone->pilot_offset_pitch=drone->gyro_pitch;
			drone->pilot_offset_roll=drone->gyro_roll;
			drone->pilot_offset_yaw=drone->gyro_yaw;
		}
		//
		drone->pilot_pitch = drone->gyro_pitch - drone->pilot_offset_pitch;
		drone->pilot_roll  = drone->gyro_roll  - drone->pilot_offset_roll;
		drone->pilot_yaw   = drone->gyro_yaw   - drone->pilot_offset_yaw;
		////////////////////////////////////////////////////////////////////////////

		float throttle=0;
		float pitch=0;
		float roll=0;
		float yaw=0;

		throttle =  (drone->ps3_lstick_y  * PWM_FLY_INTERVAL);
		pitch    = 0;// (drone->ps3_rstick_y * PWM_FLY_INTERVAL);
		roll     = (drone->ps3_rstick_x * PWM_FLY_INTERVAL);
		yaw      = 0;// (drone->ps3_lstick_x  * PWM_FLY_INTERVAL);

		//1500=pouco
		//2000=pouco
		//3000=pouco

		//y = pitch
		//x = roll

		float pid_rate_target=0;
		float pid_rate_input= drone->gyro_x;
		float pid_rate_error= pid_rate_target-pid_rate_input;
		//
		pid_rate_kp = 1000;
		float pid_rate_output = pid_rate_error*pid_rate_kp;
		//roll += pid_rate_output;

		ga_error_bak=ga_error;
		ga_error = pow(pid_rate_error,2);

		float ga_diff=ga_error-ga_error_bak;

        if(ga_diff > 0.002 ){
        	//pid_rate_kp+=10;
        }
        if(ga_diff < -0.002 ){
        	//pid_rate_kp-=10;
        }

       // printf("kp=%f error=%f ga_error=%f ga_diff=%f\r\n",pid_rate_kp,pid_rate_error,ga_error,ga_diff);


		roll += pid_rate_output;
		//pitch += pid_rate_output;

		//mix table
		if(takeoff){
			drone->motor_dutyns_target[MOTOR_FL] = motor_clamp(PWM_FLY_MIN+ throttle - pitch + roll + yaw,PWM_FLY_MIN);
			drone->motor_dutyns_target[MOTOR_RL] = motor_clamp(PWM_FLY_MIN+ throttle + pitch + roll - yaw,PWM_FLY_MIN);
			drone->motor_dutyns_target[MOTOR_FR] = motor_clamp(PWM_FLY_MIN+ throttle - pitch - roll - yaw,PWM_FLY_MIN);
			drone->motor_dutyns_target[MOTOR_RR] = motor_clamp(PWM_FLY_MIN+ throttle + pitch - roll + yaw,PWM_FLY_MIN);
		} else {
			drone->motor_dutyns_target[MOTOR_FL] = 0;
			drone->motor_dutyns_target[MOTOR_RL] = 0;
			drone->motor_dutyns_target[MOTOR_FR] = 0;
			drone->motor_dutyns_target[MOTOR_RR] = 0;
		}

		//printf("x=%f  roll=%f pid_rate_roll=%f m=%d|%d|%d|%d\r\n",drone->gyro_x,roll,pid_rate_roll,
		//		drone->motor_dutyns_target[MOTOR_FL],
		//		drone->motor_dutyns_target[MOTOR_RL],
		//		drone->motor_dutyns_target[MOTOR_FR],
		//		drone->motor_dutyns_target[MOTOR_RR]
		//);



	}
}


void *task_rx_joystick_and_tx_telemetric(void *arg)
{
	drone_t* drone=(drone_t*) arg;

	//configure server address
	struct sockaddr_in servaddr;
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons(32000);
	//open socket and bind in server address
	int sockfd=socket(AF_INET,SOCK_DGRAM,0);
	bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));


	int rxerror=0;
	for (;;)
	{

		// receive a packet from socket
		struct sockaddr_in cliaddr;
		socklen_t len = sizeof(cliaddr);
		char mesg[1500];
		int rxlen = recvfrom(sockfd,mesg,sizeof(mesg),0,(struct sockaddr *)&cliaddr,&len);
		if(rxlen==-1){
			printf("recvfrom error\r\n");
			continue;
		}
		mesg[rxlen] = 0;
		//printf("packet[%d] from %s:%d len=%d\r\n",rxcount++,inet_ntoa(cliaddr.sin_addr),ntohs(cliaddr.sin_port),rxlen);

		// packet must be a valid json -> contain joystick data
		Json::Value root;
		Json::Reader reader;
		if(reader.parse(mesg,root,false)){

			drone->joy_y = root.get("joy_y",false).asBool();
			drone->joy_a = root.get("joy_a",false).asBool();
			drone->joy_x = root.get("joy_x",false).asBool();
			drone->joy_b = root.get("joy_b",false).asBool();

			drone->joy_dpad_up    = root.get("joy_dpad_up",false).asBool();
			drone->joy_dpad_down  = root.get("joy_dpad_down",false).asBool();
			drone->joy_dpad_right = root.get("joy_dpad_right",false).asBool();
			drone->joy_dpad_left  = root.get("joy_dpad_left",false).asBool();

			drone->joy_stick_left_x= root.get("joy_left_x",0).asDouble();
			drone->joy_stick_left_y= root.get("joy_left_y",0).asDouble();
			drone->joy_stick_right_x= root.get("joy_right_x",0).asDouble();
			drone->joy_stick_right_y= root.get("joy_right_y",0).asDouble();
		}
		else {
			rxerror++;
		}


		//
		Json::Value telemetric_json;


		//gravity
		telemetric_json["acc_x"]=drone->acc_x;
		telemetric_json["acc_y"]=drone->acc_y;
		telemetric_json["acc_z"]=drone->acc_z;

		//angular speed
		telemetric_json["gyro_x"]=drone->gyro_x;
		telemetric_json["gyro_y"]=drone->gyro_y;
		telemetric_json["gyro_z"]=drone->gyro_z;

		//accel normalized
		telemetric_json["acc_n"]=drone->acc_n;

		telemetric_json["mpu6050_temp"]=drone->mpu6050_temp;

		//angles accel
		telemetric_json["pitch_acc"]=drone->acc_pitch;
		telemetric_json["roll_acc"]=drone->acc_roll;

		//angles gyro
		telemetric_json["pitch_gyro"]=drone->gyro_pitch;
		telemetric_json["roll_gyro"]=drone->gyro_roll;
		telemetric_json["yaw_gyro"]=drone->gyro_yaw;

		//angles fusion
		telemetric_json["fusion_pitch"]=drone->fusion_pitch;
		telemetric_json["fusion_roll"]=drone->fusion_roll;





		//
		telemetric_json["mag_x"]=drone->mag_x;
		telemetric_json["mag_y"]=drone->mag_y;
		telemetric_json["mag_z"]=drone->mag_z;
		telemetric_json["mag_n"]=drone->mag_n;
		telemetric_json["mag_head"]=drone->mag_heading;

		//
		telemetric_json["i2c_hz"]=drone->i2c_hz;
		telemetric_json["adc_hz"]=drone->adc_hz;
		telemetric_json["pru_hz"]=drone->pru_hz;
		telemetric_json["pilot_hz"]=drone->pilot_hz;

		//
		telemetric_json["baro_p0"]=drone->baro_p0;
		telemetric_json["baro_p"]=drone->baro_p;
		telemetric_json["baro_t"]=drone->baro_t;
		telemetric_json["baro_h"]=drone->baro_h;
		telemetric_json["baro_hema"]=drone->baro_hema;

		//
		telemetric_json["vbat"]=drone->vbat;

		telemetric_json["gps_lat"]=drone->gps_lat;
		telemetric_json["gps_long"]=drone->gps_long;


		telemetric_json["pilot_pitch"]=drone->pilot_pitch;
		telemetric_json["pilot_roll"]=drone->pilot_roll;
		telemetric_json["pilot_yaw"]=drone->pilot_yaw;

		//
		telemetric_json["motor_fl"]=drone->motor_dutyns_now[MOTOR_FL];
		telemetric_json["motor_fr"]=drone->motor_dutyns_now[MOTOR_FR];
		telemetric_json["motor_rl"]=drone->motor_dutyns_now[MOTOR_RL];
		telemetric_json["motor_rr"]=drone->motor_dutyns_now[MOTOR_RR];

		telemetric_json["q0"]=drone->q[0];
		telemetric_json["q1"]=drone->q[1];
		telemetric_json["q2"]=drone->q[2];
		telemetric_json["q3"]=drone->q[3];

		telemetric_json["e0"]=drone->euler[0];
		telemetric_json["e1"]=drone->euler[1];
		telemetric_json["e2"]=drone->euler[2];

		//
		std::string txt = telemetric_json.toStyledString();

		//
		sendto(sockfd,txt.c_str(),txt.length(),0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));

	}

	return NULL;
}

void* task_spi_cc1101(void* arg){
	for (;;)
	{
		usleep(10000);
	}
}

void* task_bluetooth_ps3(void* arg){

	drone_t* drone=(drone_t*) arg;

	drone->ps3_init_ok=false;
	drone->ps3_dpad_left=false;
	drone->ps3_dpad_right=false;
	drone->ps3_dpad_up=false;
	drone->ps3_dpad_down=false;
	drone->ps3_r3=false;
	drone->ps3_r2=false;
	drone->ps3_r1=false;
	drone->ps3_select=false;
	drone->ps3_power=false;
	drone->ps3_start=false;
	drone->ps3_l3=false;
	drone->ps3_l2=false;
	drone->ps3_l1=false;
	drone->ps3_triangle=false;
	drone->ps3_square=false;
	drone->ps3_x=false;
	drone->ps3_ball=false;
	drone->ps3_lstick_x=0;
	drone->ps3_lstick_y=0;
	drone->ps3_rstick_x=0;
	drone->ps3_rstick_y=0;


#define JS_EVENT_BUTTON         0x01    /* button pressed/released */
#define JS_EVENT_AXIS           0x02    /* joystick moved */
#define JS_EVENT_INIT           0x80    /* initial state of device */

	struct js_event {
		uint32_t time;     /* event timestamp in milliseconds */
		int16_t value;    /* value */
		uint8_t type;      /* event type */
		uint8_t number;    /* axis/button number */
	};



	int fd=-1; 
	for (;;)
	{

		///////////////////////////////////////
		if(fd==-1){
			drone->ps3_init_ok=false;
			printf("ps3 try connect...\r\n");
			fd=open("/dev/input/js0", O_RDONLY);
			if(fd==-1){
				printf("ps3 joy not connected\r\n");
				drone->ps3_init_ok=false;
				usleep(1000*1000);
				continue;
			}
			else {
				printf("ps3 joy connected ok\r\n");
				drone->ps3_init_ok=true;
			}
		}

		drone->ps3_init_ok=true;

		/////////////////////////////////////////
		struct js_event e;
		read (fd, &e, sizeof(e));

		/////////////////////////////////////////
		// ps3 accelerometer quaternion
		if(e.number==23 && e.type==2) drone->ps3_accel[0]=e.value;
		if(e.number==24 && e.type==2) drone->ps3_accel[1]=e.value;
		if(e.number==25 && e.type==2) drone->ps3_accel[2]=e.value;
		if(e.number==26 && e.type==2) drone->ps3_accel[3]=e.value;

		if(e.number==23 && e.type==2) continue; //drone->ps3_accel[0]=e.value;
		if(e.number==24 && e.type==2) continue; //drone->ps3_accel[1]=e.value;
		if(e.number==25 && e.type==2) continue; //drone->ps3_accel[2]=e.value;
		if(e.number==26 && e.type==2) continue; //drone->ps3_accel[3]=e.value;

		//buttons
		if(e.number==15 && e.type==1) drone->ps3_square=e.value;
		if(e.number==12 && e.type==1) drone->ps3_triangle=e.value;
		if(e.number==14 && e.type==1) drone->ps3_x=e.value;
		if(e.number==13 && e.type==1) drone->ps3_ball=e.value;
		//dpad
		if(e.number==4 && e.type==1) drone->ps3_dpad_up=e.value;
		if(e.number==6 && e.type==1) drone->ps3_dpad_down=e.value;
		if(e.number==5 && e.type==1) drone->ps3_dpad_right=e.value;
		if(e.number==7 && e.type==1) drone->ps3_dpad_left=e.value;
		//left
		if(e.number==10 && e.type==1) drone->ps3_l1=e.value;
		if(e.number==8  && e.type==1) drone->ps3_l2=e.value;
		if(e.number==1  && e.type==1) drone->ps3_l3=e.value;
		//right
		if(e.number==11 && e.type==1) drone->ps3_r1=e.value;
		if(e.number==9  && e.type==1) drone->ps3_r2=e.value;
		if(e.number==2  && e.type==1) drone->ps3_r3=e.value;
		//
		if(e.number==0  && e.type==1) drone->ps3_select=e.value;
		if(e.number==16 && e.type==1) drone->ps3_power=e.value;
		if(e.number==3  && e.type==1) drone->ps3_start=e.value;
		//sticks
		if(e.number==0  && e.type==2) drone->ps3_lstick_x=(float)e.value/32768.0;
		if(e.number==1  && e.type==2) drone->ps3_lstick_y=(float)e.value/-32768.0;
		if(e.number==2  && e.type==2) drone->ps3_rstick_x=(float)e.value/32768.0;
		if(e.number==3  && e.type==2) drone->ps3_rstick_y=(float)e.value/-32768.0;

		//if(e.type==2) continue;

		//printf("ps3 type=%d number=%d value=%d time=%u\r\n",e.type,e.number,e.value,e.time);



	}
}

void* task_gps(void *arg){
	for (;;)
	{
		usleep(10000);
	}
}


int main(int argc,char** argv){

        /* Avoids memory swapping for this program */
        mlockall(MCL_CURRENT|MCL_FUTURE);
	
	// gera panic se kernel travar por 10 segundos
	system("sysctl -w kernel.hung_task_timeout_secs=10");
	system("sysctl -w kernel.hung_task_panic=1");
	// gera panic se kernel pegar simbolo doido
	system("sysctl -w kernel.panic_on_oops=1");
	// gera panic se kernel sofrer de lockup
	system("sysctl -w kernel.softlockup_panic=1");
	// gera panic se acabar a RAM
	system("sysctl -w vm.panic_on_oom=1");
	// reboot automatico em 2 segundos apos um kernel panic
	system("sysctl -w kernel.panic=2");



#if 0
	//
	if(system("cat /sys/devices/bone_capemgr.9/slots | grep BB-I2C1")){
		system("echo BB-I2C1 > /sys/devices/bone_capemgr.9/slots");
		usleep(SLEEP_1_SECOND);
	}
#endif


	//load spi
	if(system("cat /sys/devices/bone_capemgr.9/slots | grep ADAFRUIT-SPI0")){
		printf("load SPI\r\n");
		system("echo ADAFRUIT-SPI0 > /sys/devices/bone_capemgr.9/slots");
		//system("echo BB-SPIDEV0 > /sys/devices/bone_capemgr.9/slots");
		usleep(SLEEP_1_SECOND);
	}

	//load pru
	if(system("cat /sys/devices/bone_capemgr.9/slots | grep bone_pru0_out")){
		printf("load PRU\r\n");
		system("echo bone_pru0_out > /sys/devices/bone_capemgr.9/slots");
		usleep(SLEEP_1_SECOND);
	}

	//load adc
	if(system("cat /sys/devices/bone_capemgr.9/slots | grep cape-bone-iio")){
		printf("load ADC\r\n");
		system("echo cape-bone-iio > /sys/devices/bone_capemgr.9/slots");
		usleep(SLEEP_1_SECOND);
	}

	//load usart4
	if(system("cat /sys/devices/bone_capemgr.9/slots | grep BB-UART4")){
		printf("load UART4\r\n");
		system("echo BB-UART4 > /sys/devices/bone_capemgr.9/slots");
		usleep(SLEEP_1_SECOND);
	}

	//
	system("/etc/init.d/gpsd restart");
	system("gpsdctl add /dev/ttyO4");


	//
	drone_t drone_data;
	memset (&drone_data,0,sizeof(drone_data));

	//
	pthread_t id_adc,id_imu,id_motors,id_rx_joystick_and_tx_telemetric,id_pilot,id_ps3,id_gps,id_spi;

	pthread_attr_t attr;


	pthread_attr_init(&attr);

	// EXPLICIT=usar do atributo, INHERIT=usar da thread criadora
	pthread_attr_setinheritsched(&attr,PTHREAD_EXPLICIT_SCHED);

	//
#if 1
	pthread_attr_setschedpolicy(&attr,SCHED_FIFO);
	struct sched_param schedParam;
	schedParam.sched_priority=sched_get_priority_max(SCHED_FIFO);
	pthread_attr_setschedparam(&attr,&schedParam);
#else
	pthread_attr_setschedpolicy(&attr,SCHED_RR);
	struct sched_param schedParam;
	schedParam.sched_priority=sched_get_priority_max(SCHED_RR);
	pthread_attr_setschedparam(&attr,&schedParam);
#endif

	//
	pthread_create(&id_adc                          , &attr, task_adc                          , &drone_data);
	pthread_create(&id_imu                          , &attr, task_imu                          , &drone_data);
	pthread_create(&id_motors                       , &attr, task_motors                       , &drone_data);
	pthread_create(&id_rx_joystick_and_tx_telemetric, &attr, task_rx_joystick_and_tx_telemetric, &drone_data);
	pthread_create(&id_pilot                        , &attr, task_pilot                        , &drone_data);
	pthread_create(&id_ps3                          , &attr, task_bluetooth_ps3                , &drone_data);
	pthread_create(&id_gps                          , &attr, task_gps                          , &drone_data);
	pthread_create(&id_spi                          , &attr, task_spi_cc1101                   , &drone_data);

	/////////////////////////////////////////////////////123456789012345
	pthread_setname_np(id_adc                          ,"adc-vbat       ");
	pthread_setname_np(id_imu                          ,"i2c-sensors    ");
	pthread_setname_np(id_motors                       ,"pru-pwm-motors ");
	pthread_setname_np(id_rx_joystick_and_tx_telemetric,"joy-telemetric ");
	pthread_setname_np(id_pilot                        ,"pilot-pid      ");
	pthread_setname_np(id_ps3                          ,"bluetooth-ps3  ");
	pthread_setname_np(id_gps                          ,"usart-gps      ");
	pthread_setname_np(id_spi                          ,"spi-cc1101-tap ");

	printf("Wait for all threads\r\n");
	//
	pthread_join(id_adc,NULL);
	pthread_join(id_imu,NULL);
	pthread_join(id_motors,NULL);
	pthread_join(id_rx_joystick_and_tx_telemetric,NULL);
	pthread_join(id_pilot,NULL);
	pthread_join(id_ps3,NULL);
	pthread_join(id_gps,NULL);
	pthread_join(id_spi,NULL);


	return 0;
}

