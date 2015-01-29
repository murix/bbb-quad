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

// C library
#include <math.h>	//Common mathematics functions
#include <stdint.h> //(since C99)	Fixed-width integer types
#include <stdio.h>	//Input/output
#include <stdlib.h>	//General utilities: memory management, program utilities, string conversions, random numbers
#include <string.h>	//String handling
#include <time.h> //Time/date utilities
//
#include <sys/mman.h>
#include <signal.h>
//socket
#include <sys/types.h>
#include <sys/socket.h>
//htonl
#include <arpa/inet.h>
//json
#include <jsoncpp/json/json.h>
//
#include <gps.h>
// xenomai
#include <xenomai/native/task.h>
#include <xenomai/native/timer.h>

///////////////////////---- PROJECT LIBRARIES
#include "i2c_linux.h"
#include "mpu6050.h"
#include "hmc5883.h"
#include "ms5611.h"
#include "bbb-adc.h"
#include "pru-pwm-cpp.h"
#include "timestamps.h"
#include "freeimu_linux.h"
#include "cc1101_linux.h"
#include "murix_pid.h"


#define VBAT_STABLE 9.0
#define SLEEP_1_SECOND (1000*1000)
#define TASK_RT_HZ   250

class xbox_joy_t {
public:
	xbox_joy_t(){
		reset();
	}
	void reset(){
		button_a=false;
		button_b=false;
		button_x=false;
		button_y=false;

		dpad_up=false;
		dpad_down=false;
		dpad_right=false;
		dpad_left=false;

		stick_left_x=0;
		stick_left_y=0;
		stick_right_x=0;
		stick_right_y=0;

	}

	bool button_a;
	bool button_b;
	bool button_x;
	bool button_y;

	bool dpad_up;
	bool dpad_down;
	bool dpad_right;
	bool dpad_left;

	float stick_left_x;
	float stick_left_y;
	float stick_right_x;
	float stick_right_y;
} ;

class ps3_joy_t {
public:

	bool dpad_left;
	bool dpad_right;
	bool dpad_up;
	bool dpad_down;

	float  lstick_x;
	float  lstick_y;
	float  rstick_x;
	float  rstick_y;

	bool button_r3;
	bool button_r2;
	bool button_r1;
	bool button_select;
	bool button_power;
	bool button_start;
	bool button_l3;
	bool button_l2;
	bool button_l1;
	bool button_triangle;
	bool button_square;
	bool button_x;
	bool button_ball;

	int    accel[4];

	ps3_joy_t(){
		reset();
	}

	void reset(){
		dpad_left=false;
		dpad_right=false;
		dpad_up=false;
		dpad_down=false;

		lstick_x=0;
		lstick_y=0;
		rstick_x=0;
		rstick_y=0;


		button_r3=false;
		button_r2=false;
		button_r1=false;
		button_select=false;
		button_power=false;
		button_start=false;
		button_l3=false;
		button_l2=false;
		button_l1=false;
		button_triangle=false;
		button_square=false;
		button_x=false;
		button_ball=false;

		accel[0]=0;
		accel[1]=0;
		accel[2]=0;
		accel[3]=0;

	}

};





/////////////////////////////////////////////
#define PWM_HZ                  400
#define PWM_FAILSAFE         500000
/////////////////////////////////////////////
#define PWM_FLY_ARM          900000
/////////////////////////////////////////////
#define PWM_CALIB_MIN       1000000
#define PWM_FLY_MIN         1070000
#define PWM_FLY_MAX         1900000
#define PWM_CALIB_MAX       1950000
//////////////////////////////////////////////
#define PWM_FLY_INTERVAL    (PWM_FLY_MAX-PWM_FLY_MIN)
#define PWM_STEP_PER_CYCLE    (PWM_FLY_INTERVAL/1000)
/////////////////////////////////////////////
#define PWM_NS_PER_SEC     (1000*1000*1000)
#define PWM_CYCLE_IN_NS    (PWM_NS_PER_SEC/PWM_HZ)
#define PWM_CYCLE_IN_US    (PWM_CYCLE_IN_NS/1000)
/////////////////////////////////////////////

typedef enum {
	MOTOR_CMD_NORMAL,
	MOTOR_CMD_CALIB_MIN,
	MOTOR_CMD_CALIB_MAX,
} motor_cmd_t;

class motor_t {
private:
	PRUPWM *myPWM;
	uint32_t pwm_step;




public:
	//
	motor_cmd_t cmd;
	uint32_t dutyns_target[8];
	uint32_t dutyns_now[8];

	motor_t(){
		myPWM=NULL;
		cmd=MOTOR_CMD_NORMAL;
		pwm_step=PWM_STEP_PER_CYCLE;
		for(int i=0;i<8;i++){
			dutyns_now[i]=0;
			dutyns_target[i]=0;
		}
	}

	void configure(){
		// Initialise PRU PWM
		myPWM = new PRUPWM(PWM_HZ);
		// Start the PRU
		myPWM->start();
		// initialize task variables
		cmd=MOTOR_CMD_NORMAL;
		for(int ch=0;ch<8;ch++){
			dutyns_target[ch]=PWM_FLY_ARM;
			dutyns_now[ch]=PWM_FLY_ARM;
			myPWM->setChannelValue(ch,PWM_FLY_ARM);
			myPWM->setFailsafeValue(ch,PWM_FAILSAFE);
		}
		//
		myPWM->setFailsafeTimeout(0);
		//myPWM->setFailsafeTimeout(2000);
	}

	void update(double vbat){

		//incremental PWM
		if(cmd==MOTOR_CMD_NORMAL){
			for(int ch=0;ch<8;ch++){
				bool need_change=false;
				//
				if(dutyns_now[ch] < dutyns_target[ch] && vbat > VBAT_STABLE){
					dutyns_now[ch] += pwm_step;
					if(dutyns_now[ch]>PWM_FLY_MAX) dutyns_now[ch]=PWM_FLY_MAX;
					need_change=true;
				}
				if(dutyns_now[ch] > dutyns_target[ch] ){
					dutyns_now[ch] -= PWM_STEP_PER_CYCLE;
					if(dutyns_now[ch]<PWM_FLY_ARM) dutyns_now[ch]=PWM_FLY_ARM;
					need_change=true;
				}
				if(need_change){
					myPWM->setChannelValue(ch,dutyns_now[ch]);
				}
			}
		}
		//calibration MIN
		if(cmd==MOTOR_CMD_CALIB_MIN){
			for(int ch=0;ch<8;ch++){
				bool need_change=false;
				if(dutyns_now[ch]!=PWM_CALIB_MIN){
					dutyns_now[ch]=PWM_CALIB_MIN;
					need_change=true;
				}
				if(need_change){
					myPWM->setChannelValue(ch,dutyns_now[ch]);
				}
			}

		}
		//calibration MAX
		if(cmd==MOTOR_CMD_CALIB_MAX){
			for(int ch=0;ch<8;ch++){
				bool need_change=false;
				if(dutyns_now[ch]!=PWM_CALIB_MAX){
					dutyns_now[ch]=PWM_CALIB_MAX;
					need_change=true;
				}
				if(need_change){
					myPWM->setChannelValue(ch,dutyns_now[ch]);
				}
			}
		}
		// wait pwm complete duty
		usleep(PWM_CYCLE_IN_US);

	}


};

class drone_t {
public:

	drone_t(){
		euler_degree[0]=0;
		euler_degree[1]=0;
		euler_degree[2]=0;

		gps_lat=0;
		gps_long=0;

		i2c_hz=0;
		adc_hz=0;
		pru_hz=0;
		pilot_hz=0;

		vbat=0;
	}

	//
	i2c_linux i2c;
	mpu6050 acc_gyro;
	hmc5883 mag;
	ms5611 baro;
	FreeIMU freeimu;

	double euler_degree[3];
	//
	float gps_lat;
	float gps_long;
	//
	float vbat;
	//
	float i2c_hz;
	float pru_hz;
	float adc_hz;
	float pilot_hz;


	murix_controller pids[6];

	motor_t motors;


	//
	xbox_joy_t xbox_joy;
	ps3_joy_t ps3_joy;
};


#define PID_YAW_RATE    1
#define PID_YAW_ANGLE   2
#define PID_ROLL_RATE   3
#define PID_ROLL_ANGLE  4
#define PID_PITCH_RATE  5
#define PID_PITCH_ANGLE 6





void task_rt_adc(void *arg){

	drone_t* drone=(drone_t*) arg;
	adc_monitor adc;
	adc.init();

	//---------
	double t_back=get_timestamp_in_seconds();;
	double t_now=get_timestamp_in_seconds();;
	double t_diff=0;

	rt_task_set_periodic(NULL, TM_NOW, 1000000000 / TASK_RT_HZ);
	while(1){
		rt_task_wait_period(NULL);

		//-------------
		t_back=t_now;
		t_now=get_timestamp_in_seconds();
		t_diff=t_now-t_back;

		///////////////////////////////////

		drone->adc_hz = 1.0 / t_diff;

		////////////////////////////////////


		adc.update();
		drone->vbat=adc.vbat;



	}


}



void task_rt_motor(void *arg){
	drone_t* drone=(drone_t*) arg;


	//---------
	double t_back=get_timestamp_in_seconds();
	double t_now=get_timestamp_in_seconds();
	double t_diff=0;

	drone->motors.configure();

	rt_task_set_periodic(NULL, TM_NOW, 1000000000 / TASK_RT_HZ);
	while(1){
		rt_task_wait_period(NULL);

		//-------------
		t_back=t_now;
		t_now=get_timestamp_in_seconds();
		t_diff=t_now-t_back;

		///////////////////////////////////

		drone->pru_hz = 1.0 / t_diff;

		////////////////////////////////////


		drone->motors.update(drone->vbat);


	}

}

void task_rt_i2c(void *arg){


	drone_t* drone=(drone_t*) arg;

	//
	drone->i2c.open();
	//
	drone->acc_gyro.configure(drone->i2c);
	drone->mag.configure(drone->i2c);
	drone->baro.configure(drone->i2c);
	//

	double t_back=get_timestamp_in_seconds();;
	double t_now=get_timestamp_in_seconds();;
	double t_diff=0;



	rt_task_set_periodic(NULL, TM_NOW, 1000000000 / TASK_RT_HZ);
	while(1){
		rt_task_wait_period(NULL);


		///////////////////////////////////////
		t_back=t_now;
		t_now=get_timestamp_in_seconds();
		t_diff=t_now-t_back;
		drone->i2c_hz = 1.0 / t_diff;
		///////////////////////////////////////

		// update sensors
		drone->acc_gyro.update(drone->i2c);
		drone->mag.update(drone->i2c);
		drone->baro.update(drone->i2c);

		// update quaternion
		drone->freeimu.getEulerDegree(drone->euler_degree,
				(double)drone->acc_gyro.acc_g_x,
				(double)drone->acc_gyro.acc_g_y,
				(double)drone->acc_gyro.acc_g_z,
				(double)drone->acc_gyro.gyro_radians_x,
				(double)drone->acc_gyro.gyro_radians_y,
				(double)drone->acc_gyro.gyro_radians_z,
				drone->mag.mag_x,
				drone->mag.mag_y,
				drone->mag.mag_z);


		//////////////////////////////////////////////////
	}
}

#define MOTOR_FR 0
#define MOTOR_FL 1
#define MOTOR_RR 5
#define MOTOR_RL 7


float mixer_clamp(float a,float b){
	if (a > b){
		return a;
	}
	else {
		return b;
	}
}

void task_rt_pid(void *arg)
{
	drone_t* drone=(drone_t*) arg;

	double t_back=get_timestamp_in_seconds();;
	double t_now=get_timestamp_in_seconds();;
	double t_diff=0;



	bool takeoff=false;
	bool mode_stable=false;
	bool mode_acrobatic=false;
	drone->roll_rate.setpoint=0;

	rt_task_set_periodic(NULL, TM_NOW, 1000000000 / TASK_RT_HZ);
	while(1){
		rt_task_wait_period(NULL);

		///////////////////////////////////

		t_back=t_now;
		t_now=get_timestamp_in_seconds();
		t_diff=t_now-t_back;

		///////////////////////////////////

		drone->pilot_hz = 1.0 / t_diff;

		/////////////////////////////////////////////////////////////////////////////
		if(drone->ps3_joy.button_start){
			takeoff=true;
		}
		if(drone->ps3_joy.button_select){
			takeoff=false;
		}
		///////////////////////////////////////////////////////////////////////////
		if(drone->ps3_joy.button_x){
			mode_stable=true;
			mode_acrobatic=false;
		}
		if(drone->ps3_joy.button_ball){
			mode_stable=false;
			mode_acrobatic=true;
		}
		////////////////////////////////////////////////////////////////////////////

		float throttle=0;
		float pitch=0;
		float roll=0;
		float yaw=0;

		throttle =  (drone->ps3_joy.lstick_y  * PWM_FLY_INTERVAL);
		pitch    =  (drone->ps3_joy.rstick_y  * PWM_FLY_INTERVAL);
		roll     =  (drone->ps3_joy.rstick_x  * PWM_FLY_INTERVAL);
		yaw      =  (drone->ps3_joy.lstick_x  * PWM_FLY_INTERVAL);

		if(mode_acrobatic){
			//set desired rate in degree/s
			drone->pids[PID_PITCH_RATE].setpoint=drone->ps3_joy.rstick_y  * 200;
			drone->pids[PID_ROLL_RATE].setpoint=drone->ps3_joy.rstick_x  * 200;
			drone->pids[PID_YAW_RATE].setpoint=drone->ps3_joy.lstick_x  * 200;

		}

		if(mode_stable){


		}


		//mix table
		if(takeoff){
			drone->motors.dutyns_target[MOTOR_FL] = mixer_clamp(PWM_FLY_MIN+ throttle - pitch + roll + yaw,PWM_FLY_MIN);
			drone->motors.dutyns_target[MOTOR_RL] = mixer_clamp(PWM_FLY_MIN+ throttle + pitch + roll - yaw,PWM_FLY_MIN);
			drone->motors.dutyns_target[MOTOR_FR] = mixer_clamp(PWM_FLY_MIN+ throttle - pitch - roll - yaw,PWM_FLY_MIN);
			drone->motors.dutyns_target[MOTOR_RR] = mixer_clamp(PWM_FLY_MIN+ throttle + pitch - roll + yaw,PWM_FLY_MIN);
		} else {
			// motors off
			drone->motors.dutyns_target[MOTOR_FL] = 0;
			drone->motors.dutyns_target[MOTOR_RL] = 0;
			drone->motors.dutyns_target[MOTOR_FR] = 0;
			drone->motors.dutyns_target[MOTOR_RR] = 0;
			// set target yaw to sensor yaw - on takeoff

			// reset PID integrals while on the ground

		}



	}



}





void task_nonrt_udp(void *arg)
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

			drone->xbox_joy.button_y = root.get("joy_y",false).asBool();
			drone->xbox_joy.button_a = root.get("joy_a",false).asBool();
			drone->xbox_joy.button_x = root.get("joy_x",false).asBool();
			drone->xbox_joy.button_b = root.get("joy_b",false).asBool();

			drone->xbox_joy.dpad_up    = root.get("joy_dpad_up",false).asBool();
			drone->xbox_joy.dpad_down  = root.get("joy_dpad_down",false).asBool();
			drone->xbox_joy.dpad_right = root.get("joy_dpad_right",false).asBool();
			drone->xbox_joy.dpad_left  = root.get("joy_dpad_left",false).asBool();

			drone->xbox_joy.stick_left_x= root.get("joy_left_x",0).asDouble();
			drone->xbox_joy.stick_left_y= root.get("joy_left_y",0).asDouble();
			drone->xbox_joy.stick_right_x= root.get("joy_right_x",0).asDouble();
			drone->xbox_joy.stick_right_y= root.get("joy_right_y",0).asDouble();
		}
		else {
			rxerror++;
		}


		//
		Json::Value telemetric_json;


		//gravity
		telemetric_json["acc_x"]=drone->acc_gyro.acc_g_x;
		telemetric_json["acc_y"]=drone->acc_gyro.acc_g_y;
		telemetric_json["acc_z"]=drone->acc_gyro.acc_g_z;
		//angular speed
		telemetric_json["gyro_x"]=drone->acc_gyro.gyro_degrees_x;
		telemetric_json["gyro_y"]=drone->acc_gyro.gyro_degrees_y;
		telemetric_json["gyro_z"]=drone->acc_gyro.gyro_degrees_z;
		//temperature
		telemetric_json["mpu6050_temp"]=drone->acc_gyro.temperate_celcius;

		//
		telemetric_json["mag_x"]=drone->mag.mag_x;
		telemetric_json["mag_y"]=drone->mag.mag_y;
		telemetric_json["mag_z"]=drone->mag.mag_z;

		//
		telemetric_json["i2c_hz"]=drone->i2c_hz;
		telemetric_json["adc_hz"]=drone->adc_hz;
		telemetric_json["pru_hz"]=drone->pru_hz;
		telemetric_json["pilot_hz"]=drone->pilot_hz;

		//
		telemetric_json["baro_p0"]=drone->baro.P0;
		telemetric_json["baro_p"]=drone->baro.P;
		telemetric_json["baro_t"]=drone->baro.T;
		telemetric_json["baro_h"]=drone->baro.H;

		//
		telemetric_json["vbat"]=drone->vbat;

		telemetric_json["gps_lat"]=drone->gps_lat;
		telemetric_json["gps_long"]=drone->gps_long;

		//
		telemetric_json["motor_fl"]=drone->motors.dutyns_now[MOTOR_FL];
		telemetric_json["motor_fr"]=drone->motors.dutyns_now[MOTOR_FR];
		telemetric_json["motor_rl"]=drone->motors.dutyns_now[MOTOR_RL];
		telemetric_json["motor_rr"]=drone->motors.dutyns_now[MOTOR_RR];

		telemetric_json["q0"]=drone->freeimu.q0;
		telemetric_json["q1"]=drone->freeimu.q1;
		telemetric_json["q2"]=drone->freeimu.q2;
		telemetric_json["q3"]=drone->freeimu.q3;

		telemetric_json["e0"]=drone->euler_degree[0];
		telemetric_json["e1"]=drone->euler_degree[1];
		telemetric_json["e2"]=drone->euler_degree[2];

		//
		std::string txt = telemetric_json.toStyledString();

		//
		sendto(sockfd,txt.c_str(),txt.length(),0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));

	}


}

void task_nonrt_spi(void* arg){

	spidev_cc1101_worker((char*)"172.31.200.1");


}



void task_nonrt_ps3(void* arg){

	drone_t* drone=(drone_t*) arg;



	drone->ps3_joy.reset();



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
			//printf("ps3 try connect...\r\n");
			fd=open("/dev/input/js0", O_RDONLY);
			if(fd==-1){
				drone->ps3_joy.reset();
				//printf("ps3 joy not connected\r\n");
				usleep(SLEEP_1_SECOND);
				continue;
			}
			else {
				printf("ps3 joy connected ok\r\n");
			}
		}



		/////////////////////////////////////////
		struct js_event e;
		int ret;
		ret = read (fd, &e, sizeof(e));
		if( ret == -1 ){
			close(fd);
			fd=-1;
			drone->ps3_joy.reset();
		}

		/////////////////////////////////////////
		// ps3 accelerometer quaternion
		if(e.number==23 && e.type==2) drone->ps3_joy.accel[0]=e.value;
		if(e.number==24 && e.type==2) drone->ps3_joy.accel[1]=e.value;
		if(e.number==25 && e.type==2) drone->ps3_joy.accel[2]=e.value;
		if(e.number==26 && e.type==2) drone->ps3_joy.accel[3]=e.value;

		if(e.number==23 && e.type==2) drone->ps3_joy.accel[0]=e.value;
		if(e.number==24 && e.type==2) drone->ps3_joy.accel[1]=e.value;
		if(e.number==25 && e.type==2) drone->ps3_joy.accel[2]=e.value;
		if(e.number==26 && e.type==2) drone->ps3_joy.accel[3]=e.value;

		//buttons
		if(e.number==15 && e.type==1) drone->ps3_joy.button_square=e.value;
		if(e.number==12 && e.type==1) drone->ps3_joy.button_triangle=e.value;
		if(e.number==14 && e.type==1) drone->ps3_joy.button_x=e.value;
		if(e.number==13 && e.type==1) drone->ps3_joy.button_ball=e.value;
		//dpad
		if(e.number==4 && e.type==1) drone->ps3_joy.dpad_up=e.value;
		if(e.number==6 && e.type==1) drone->ps3_joy.dpad_down=e.value;
		if(e.number==5 && e.type==1) drone->ps3_joy.dpad_right=e.value;
		if(e.number==7 && e.type==1) drone->ps3_joy.dpad_left=e.value;
		//left
		if(e.number==10 && e.type==1) drone->ps3_joy.button_l1=e.value;
		if(e.number==8  && e.type==1) drone->ps3_joy.button_l2=e.value;
		if(e.number==1  && e.type==1) drone->ps3_joy.button_l3=e.value;
		//right
		if(e.number==11 && e.type==1) drone->ps3_joy.button_r1=e.value;
		if(e.number==9  && e.type==1) drone->ps3_joy.button_r2=e.value;
		if(e.number==2  && e.type==1) drone->ps3_joy.button_r3=e.value;
		//
		if(e.number==0  && e.type==1) drone->ps3_joy.button_select=e.value;
		if(e.number==16 && e.type==1) drone->ps3_joy.button_power=e.value;
		if(e.number==3  && e.type==1) drone->ps3_joy.button_start=e.value;
		//sticks
		if(e.number==0  && e.type==2) drone->ps3_joy.lstick_x=(float)e.value/32768.0;
		if(e.number==1  && e.type==2) drone->ps3_joy.lstick_y=(float)e.value/-32768.0;
		if(e.number==2  && e.type==2) drone->ps3_joy.rstick_x=(float)e.value/32768.0;
		if(e.number==3  && e.type==2) drone->ps3_joy.rstick_y=(float)e.value/-32768.0;

	}


}

void task_nonrt_gps(void *arg){

	drone_t* drone=(drone_t*) arg;


	for (;;)
	{

		struct gps_data_t gps_data;
		gps_open("localhost", "2947", &gps_data);
		gps_stream(&gps_data, WATCH_ENABLE | WATCH_JSON, NULL);
		while(1){
			/* Put this in a loop with a call to a high resolution sleep () in it. */
			if (gps_waiting (&gps_data, 500)) {
				if (gps_read (&gps_data) == -1) {
					perror("gps-read-error\r\n");
				} else {

					printf("mode=%d ",gps_data.fix.mode);
					printf("alt=%.2lfm(+-%.2lfm) ",gps_data.fix.altitude,gps_data.fix.epv);
					printf("vspeed=%.2lfm/s(+-%.2lfm/s) ",gps_data.fix.climb,gps_data.fix.epc);
					printf("gspeed=%.2lfm/s(+-%.2lfm/s) ",gps_data.fix.speed,gps_data.fix.eps);
					printf("north=%.2lfdeg(+-%.2lfdeg) ",gps_data.fix.track,gps_data.fix.epd);
					printf("lat=%.2lfdeg(+-%.2lfm) ",gps_data.fix.latitude,gps_data.fix.epy);
					printf("long=%.2lfdeg(+-%.2lfm) ",gps_data.fix.longitude,gps_data.fix.epx);
					char buff[30];
					char* txt=unix_to_iso8601(gps_data.fix.time,buff,sizeof(buff));
					printf("%s(+-%.2lf)\r\n",txt,gps_data.fix.ept);

					if(isfinite(gps_data.fix.latitude)) drone->gps_lat=gps_data.fix.latitude;
					if(isfinite(gps_data.fix.longitude)) drone->gps_long=gps_data.fix.longitude;
				}
			}
		}
		gps_stream(&gps_data, WATCH_DISABLE, NULL);
		gps_close (&gps_data);


	}

}

void catch_signal(int sig)
{
	printf("catch_signal=%d\r\n",sig);
}


int main(int argc,char** argv){

	//signal(SIGTERM, catch_signal);
	//signal(SIGINT, catch_signal);

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

	RT_TASK adc_task;
	RT_TASK gps_task;
	RT_TASK i2c_task;
	RT_TASK motor_task;
	RT_TASK pid_task;
	RT_TASK ps3_task;
	RT_TASK spi_task;
	RT_TASK udp_task;

	rt_task_create(&adc_task  , "adc_task"  , 0, 95, 0);
	rt_task_create(&i2c_task  , "i2c_task"  , 0, 99, 0);
	rt_task_create(&motor_task, "motor_task", 0, 97, 0);
	rt_task_create(&pid_task  , "pid_task"  , 0, 96, 0);

	rt_task_create(&gps_task  , "gps_task"  , 0, 85, 0);
	rt_task_create(&ps3_task  , "ps3_task"  , 0, 85, 0);
	rt_task_create(&spi_task  , "spi_task"  , 0, 85, 0);
	rt_task_create(&udp_task  , "udp_task"  , 0, 85, 0);

	rt_task_start(&adc_task  , &task_rt_adc  , &drone_data);
	rt_task_start(&i2c_task  , &task_rt_i2c  , &drone_data);
	rt_task_start(&motor_task, &task_rt_motor, &drone_data);
	rt_task_start(&pid_task  , &task_rt_pid  , &drone_data);

	rt_task_start(&gps_task  , &task_nonrt_gps  , &drone_data);
	rt_task_start(&ps3_task  , &task_nonrt_ps3  , &drone_data);
	rt_task_start(&spi_task  , &task_nonrt_spi  , &drone_data);
	rt_task_start(&udp_task  , &task_nonrt_udp  , &drone_data);


	printf("Wait for all threads\r\n");
	while(1){
		usleep(SLEEP_1_SECOND);
	}


	return 0;
}

