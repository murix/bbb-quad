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




typedef struct {
	// ARM: float and uint32_t are thread safe
	//
	float acc_x;
	float acc_y;
	float acc_z;
	float acc_n;
	float acc_pitch;
	float acc_roll;

	float gyro_x;
	float gyro_y;
	float gyro_z;
	float gyro_pitch;
	float gyro_roll;
	float gyro_yaw;

	float fusion_pitch;
	float fusion_roll;

	float mag_x;
	float mag_y;
	float mag_z;
	float mag_n;
	float mag_heading;
	//
	//
	float i2c_hz;
	float pru_hz;
	float adc_hz;
	float pilot_hz;

	//
	float baro_p0;
	float baro_p;
	float baro_t;
	float baro_h;
	float baro_hema;

	float pilot_offset_pitch;
	float pilot_offset_roll;
	float pilot_pitch;
	float pilot_roll;

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

} drone_t;





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
#define PWM_FLY_ARM          900000
#define PWM_CALIB_MIN       1000000
#define PWM_FLY_MIN         1070000
#define PWM_FLY_MAX         1900000
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
	printf("PRU initialized\r\n");

	// initialize task variables
	drone->motor_cmd=MOTOR_CMD_NORMAL;
	for(int ch=0;ch<8;ch++){
		drone->motor_dutyns_target[ch]=PWM_FLY_ARM;
		drone->motor_dutyns_now[ch]=PWM_FLY_ARM;
		myPWM->setChannelValue(ch,PWM_FLY_ARM);
	}

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

		drone->pru_hz = 1.0 / t_diff;

		////////////////////////////////////


		//incremental PWM
		if(drone->motor_cmd==MOTOR_CMD_NORMAL){
			for(int ch=0;ch<8;ch++){
				bool need_change=false;
				//
				if(drone->motor_dutyns_now[ch] < drone->motor_dutyns_target[ch] && drone->vbat > 9){
					drone->motor_dutyns_now[ch] += PWM_STEP_PER_CYCLE;
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
		//usleep(5000);
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
	while(1){
		//usleep(1000);

		mpu.update();
		mag.update();
		baro.update();

		////////////////////////////////////////////////

		drone->i2c_hz = 1.0/mpu.t_diff;

		////////////////////////////////////////////////
		drone->acc_x=mpu.acc[0];
		drone->acc_y=mpu.acc[1];
		drone->acc_z=mpu.acc[2];
		drone->acc_n=mpu.accn;
		drone->acc_pitch=mpu.acc_pitch;
		drone->acc_roll=mpu.acc_roll;

		drone->gyro_x=mpu.gyro[0];
		drone->gyro_y=mpu.gyro[1];
		drone->gyro_z=mpu.gyro[2];
		drone->gyro_pitch=mpu.gyro_integrate[0];
		drone->gyro_roll=mpu.gyro_integrate[1];
		drone->gyro_yaw=mpu.gyro_integrate[2];

		drone->fusion_pitch=mpu.fusion_pitch;
		drone->fusion_roll=mpu.fusion_roll;

		///////////////////////////////////////////////

		drone->mag_x=mag.mag[0];
		drone->mag_y=mag.mag[1];
		drone->mag_z=mag.mag[2];
		drone->mag_n=mag.magn;
		drone->mag_heading=mag.heading;

		///////////////////////////////////////////////
		drone->baro_p0=baro.P0;
		drone->baro_p=baro.P;
		drone->baro_t=baro.T;
		drone->baro_h=baro.H;
		drone->baro_hema=baro.H_EMA;

		//////////////////////////////////////////////////
	}
}

#define MOTOR_FR 0
#define MOTOR_FL 1
#define MOTOR_RR 5
#define MOTOR_RL 7

float motor_clamp(float a,float b){
	if (a > b) return a;
	else return b;
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
		if(drone->joy_a){
			takeoff=true;
		}
		if(drone->joy_b){
			takeoff=false;
		}
		///////////////////////////////////////////////////////////////////////////
		if(drone->joy_y){
			drone->pilot_offset_pitch=drone->fusion_pitch;
			drone->pilot_offset_roll=drone->fusion_roll;
		}
		drone->pilot_pitch = drone->fusion_pitch - drone->pilot_offset_pitch;
		drone->pilot_roll  = drone->fusion_roll  - drone->pilot_offset_roll;
		////////////////////////////////////////////////////////////////////////////

		float throttle,pitch,roll,yaw;

		//
		yaw      = drone->joy_stick_left_x  * PWM_FLY_MAX;
		throttle = drone->joy_stick_left_y  * PWM_FLY_MAX;
		pitch    = drone->joy_stick_right_y * PWM_FLY_MAX;
		roll     = drone->joy_stick_right_x * PWM_FLY_MAX;

		//mix table
		if(takeoff){
			drone->motor_dutyns_target[MOTOR_FL] = motor_clamp(throttle - pitch + roll + yaw,PWM_FLY_MIN);
			drone->motor_dutyns_target[MOTOR_RL] = motor_clamp(throttle + pitch + roll - yaw,PWM_FLY_MIN);
			drone->motor_dutyns_target[MOTOR_FR] = motor_clamp(throttle - pitch - roll - yaw,PWM_FLY_MIN);
			drone->motor_dutyns_target[MOTOR_RR] = motor_clamp(throttle + pitch - roll + yaw,PWM_FLY_MIN);
		} else {
			drone->motor_dutyns_target[MOTOR_FL] = 0;
			drone->motor_dutyns_target[MOTOR_RL] = 0;
			drone->motor_dutyns_target[MOTOR_FR] = 0;
			drone->motor_dutyns_target[MOTOR_RR] = 0;
		}
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


	int rxcount=0;
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
		printf("packet[%d] from %s:%d len=%d\r\n",rxcount++,inet_ntoa(cliaddr.sin_addr),ntohs(cliaddr.sin_port),rxlen);

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

		//
		telemetric_json["acc_x"]=drone->acc_x;
		telemetric_json["acc_y"]=drone->acc_y;
		telemetric_json["acc_z"]=drone->acc_z;
		telemetric_json["acc_n"]=drone->acc_n;
		telemetric_json["pitch_acc"]=drone->acc_pitch;
		telemetric_json["roll_acc"]=drone->acc_roll;

		//
		telemetric_json["gyro_x"]=drone->gyro_x;
		telemetric_json["gyro_y"]=drone->gyro_y;
		telemetric_json["gyro_z"]=drone->gyro_z;
		telemetric_json["pitch_gyro"]=drone->gyro_pitch;
		telemetric_json["roll_gyro"]=drone->gyro_roll;
		telemetric_json["yaw_gyro"]=drone->gyro_yaw;

		//
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

		telemetric_json["pilot_pitch"]=drone->pilot_pitch;
		telemetric_json["pilot_roll"]=drone->pilot_roll;

		//
		telemetric_json["motor_fl"]=drone->motor_dutyns_now[MOTOR_FL];
		telemetric_json["motor_fr"]=drone->motor_dutyns_now[MOTOR_FR];
		telemetric_json["motor_rl"]=drone->motor_dutyns_now[MOTOR_RL];
		telemetric_json["motor_rr"]=drone->motor_dutyns_now[MOTOR_RR];



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
	for (;;)
	{
		usleep(10000);
	}
}

void* task_gps(void *arg){
	for (;;)
	{
		usleep(10000);
	}
}


int main(int argc,char** argv){

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
		usleep(1000*1000);
	}
#endif


	//load spi
	if(system("cat /sys/devices/bone_capemgr.9/slots | grep ADAFRUIT-SPI0")){
		printf("load SPI\r\n");
		system("echo ADAFRUIT-SPI0 > /sys/devices/bone_capemgr.9/slots");
		//system("echo BB-SPIDEV0 > /sys/devices/bone_capemgr.9/slots");
		usleep(1000*1000);
	}

	//load pru
	if(system("cat /sys/devices/bone_capemgr.9/slots | grep bone_pru0_out")){
		printf("load PRU\r\n");
		system("echo bone_pru0_out > /sys/devices/bone_capemgr.9/slots");
		usleep(1000*1000);
	}

	//load adc
	if(system("cat /sys/devices/bone_capemgr.9/slots | grep cape-bone-iio")){
		printf("load ADC\r\n");
		system("echo cape-bone-iio > /sys/devices/bone_capemgr.9/slots");
		usleep(1000*1000);
	}

	//load usart4
	if(system("cat /sys/devices/bone_capemgr.9/slots | grep BB-UART4")){
		printf("load UART4\r\n");
		system("echo BB-UART4 > /sys/devices/bone_capemgr.9/slots");
		usleep(1000*1000);
	}

	//
	system("gpsdctl add /dev/ttyO4");
	usleep(1000*1000);


	//
	drone_t drone_data;
	memset (&drone_data,0,sizeof(drone_data));

	//
	pthread_t id_adc,id_imu,id_motors,id_rx_joystick_and_tx_telemetric,id_pilot,id_ps3,id_gps,id_spi;

	//
	pthread_create(&id_adc                          , 0, task_adc                          , &drone_data);
	pthread_create(&id_imu                          , 0, task_imu                          , &drone_data);
	pthread_create(&id_motors                       , 0, task_motors                       , &drone_data);
	pthread_create(&id_rx_joystick_and_tx_telemetric, 0, task_rx_joystick_and_tx_telemetric, &drone_data);
	pthread_create(&id_pilot                        , 0, task_pilot                        , &drone_data);
	pthread_create(&id_ps3                          , 0, task_bluetooth_ps3                , &drone_data);
	pthread_create(&id_gps                          , 0, task_gps                          , &drone_data);
	pthread_create(&id_spi                          , 0, task_spi_cc1101                   , &drone_data);

	/////////////////////////////////////////////////////123456789012345
	pthread_setname_np(id_adc                          ,"adc-vbat       ");
	pthread_setname_np(id_imu                          ,"i2c-sensors    ");
	pthread_setname_np(id_motors                       ,"pru-pwm-motors ");
	pthread_setname_np(id_rx_joystick_and_tx_telemetric,"joy-telemetric ");
	pthread_setname_np(id_pilot                        ,"pilot-pid      ");
	pthread_setname_np(id_ps3                          ,"bluetooth-ps3  ");
	pthread_setname_np(id_gps                          ,"usart-gps      ");
	pthread_setname_np(id_spi                          ,"spi-cc1101-tap ");

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

