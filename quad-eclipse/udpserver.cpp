/*
 * udpserver.cpp
 *
 *  Created on: 10/08/2014
 *      Author: root
 */



//socket
#include <sys/types.h>
#include <sys/socket.h>

//htonl
#include <arpa/inet.h>

//bzero
#include <string.h>

//json
#include <jsoncpp/json/json.h>

#include "udpserver.h"

#include <stdio.h>
#include <stdlib.h>

//
#include "pruPWM.h"

#include "bbb-adc.h"
#include <math.h>

//
#define PWM_HZ              400
#define PWM_FLY_ARM      970000
#define PWM_CALIB_MIN   1000000
#define PWM_FLY_MIN     1070000
#define PWM_FLY_MAX     1900000
#define PWM_CALIB_MAX   1950000
#define PWM_STEP           100
#define PWM_CHANGE      2500000

typedef struct {
 char cmd;
 uint32_t dutyns[8];

} motor_t;

 int speeds[8];
 adc_monitor adc;
void *motorserver(void *arg){
     motor_t* pdata=(motor_t*) arg;

 adc.init();

       //
       printf("enable PRU overlay\r\n");
       system("echo bone_pru0_out > /sys/devices/bone_capemgr.9/slots");
       printf("wait PRU overlay to be ready...\r\n");
       usleep(1000000);

	// Initialise PRU PWM
	PRUPWM *myPWM = new PRUPWM(PWM_HZ);
	// Start the PRU
	myPWM->start();
        printf("PRU initialized\r\n");


        for(int ch=0;ch<8;ch++){
	   myPWM->setChannelValue(ch,PWM_FLY_ARM);
        }
        printf("PRU Motors frequency=%d Hz ch=all @ %d ns\r\n",PWM_HZ,PWM_FLY_ARM);


     
    

    
     for(int ch=0;ch<8;ch++){
       speeds[ch]=PWM_FLY_ARM;
     }

     float _vbatprev=0;
     float _vbatnow=0;
     float _vbatdiff=0;
     int _pwm_step=1000;
     for(;;){
        adc.update();
        _vbatprev=_vbatnow;
        _vbatnow=adc.vbat;
        _vbatdiff=fabs(fabs(_vbatprev)-fabs(_vbatnow));

        // fuzzy step
        if(               _vbatdiff<0.2)  _pwm_step+=100;
        if(_vbatdiff>0.1&&_vbatdiff<0.4)  _pwm_step-=100;
        if(_vbatdiff>0.2&&_vbatdiff<0.6)  _pwm_step-=200;
        if(_vbatdiff>0.5&&_vbatdiff<0.8)  _pwm_step-=300;
        if(_vbatdiff>0.7&&_vbatdiff<1.0)  _pwm_step-=400;
        if(_vbatdiff>0.9               )  _pwm_step-=500;

        //safe
        if(_pwm_step<100) _pwm_step=100;
        if(_pwm_step>2500) _pwm_step=2500;


        for(int ch=0;ch<8;ch++){
           
           bool need_change=false;
           //
           if(speeds[ch] < pdata->dutyns[ch]){
              speeds[ch] += _pwm_step;
              if(speeds[ch]>PWM_FLY_MAX) speeds[ch]=PWM_FLY_MAX;
              need_change=true;
           }
           if(speeds[ch] > pdata->dutyns[ch]){
              speeds[ch] -= _pwm_step;
              if(speeds[ch]<PWM_FLY_ARM) speeds[ch]=PWM_FLY_ARM;
	      need_change=true;
           }

           if(need_change){
              myPWM->setChannelValue(ch,speeds[ch]);
              //printf("PRU Motor frequency=%d Hz ch=%d @ %d ns\r\n",PWM_HZ,ch,speeds[ch]);
           }
        }
        
        // wait 2,5ms for 400hz pwm complete duty
        usleep(2500);
        //command for terminate this thread
        //if(pdata->cmd=='a'){
        //  break;
        //}
     }


        // apply ARM
        //for(int ch=0;ch<8;ch++){
	//   myPWM->setChannelValue(ch,PWM_FLY_ARM);
        //}
        //printf("PRU Motors frequency=%d Hz ch=all @ %d ns\r\n",PWM_HZ,PWM_FLY_ARM);
        //printf("all motors has stoped\r\n");



}




void *udpserver(void *arg)
{
	imu_data_t* pdata=(imu_data_t*) arg;

	int sockfd,n;
	struct sockaddr_in servaddr,cliaddr;
	socklen_t len;
	char mesg[1000];

	sockfd=socket(AF_INET,SOCK_DGRAM,0);

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons(32000);
	bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));


        motor_t motors;
        bzero(&motors,sizeof(motor_t));
	pthread_t th1;
	pthread_create(&th1, 0, motorserver, &motors);
      


        int count=0;
	for (;;)
	{
            
                    
		len = sizeof(cliaddr);
		n = recvfrom(sockfd,mesg,1000,0,(struct sockaddr *)&cliaddr,&len);
		mesg[n] = 0;

                Json::Value root;
                Json::Reader reader;
                if(reader.parse(mesg,root,false)){
                     bool joy_y = root.get("joy_y",false).asBool(); //up
                     bool joy_a = root.get("joy_a",false).asBool(); //down
                     bool joy_x = root.get("joy_x",false).asBool(); //right
                     bool joy_b = root.get("joy_b",false).asBool(); //left

                     bool joy_dpad_up    = root.get("joy_dpad_up",false).asBool(); //up
                     bool joy_dpad_down  = root.get("joy_dpad_down",false).asBool(); //down
                     bool joy_dpad_right = root.get("joy_dpad_right",false).asBool(); //right
                     bool joy_dpad_left  = root.get("joy_dpad_left",false).asBool(); //left

                     if(joy_y){
                        for(int i=0;i<8;i++) motors.dutyns[i]=PWM_FLY_MAX;
                     }
                     if(joy_x){
                        for(int i=0;i<8;i++) motors.dutyns[i]=PWM_FLY_MIN;
                     }
                     if(joy_a){
                         for(int i=0;i<8;i++) motors.dutyns[i]=PWM_FLY_ARM; 
                     }



                } 
                else {
                   std::cout << reader.getFormatedErrorMessages() << "\r\n" ;
                }


                printf("udpserver count=%d\r\n",count);
		count++;                

		Json::Value fromScratch;

		//
		fromScratch["acc_x"]=pdata->acc_x;
		fromScratch["acc_y"]=pdata->acc_y;
		fromScratch["acc_z"]=pdata->acc_z;
		fromScratch["acc_n"]=pdata->acc_n;
		fromScratch["pitch_acc"]=pdata->acc_pitch;
		fromScratch["roll_acc"]=pdata->acc_roll;

		//
		fromScratch["gyro_x"]=pdata->gyro_x;
		fromScratch["gyro_y"]=pdata->gyro_y;
		fromScratch["gyro_z"]=pdata->gyro_z;
		fromScratch["pitch_gyro"]=pdata->gyro_pitch;
		fromScratch["roll_gyro"]=pdata->gyro_roll;
		fromScratch["yaw_gyro"]=pdata->gyro_yaw;

		//
		fromScratch["fusion_pitch"]=pdata->fusion_pitch;
		fromScratch["fusion_roll"]=pdata->fusion_roll;


		//
		fromScratch["mag_x"]=pdata->mag_x;
		fromScratch["mag_y"]=pdata->mag_y;
		fromScratch["mag_z"]=pdata->mag_z;
		fromScratch["mag_n"]=pdata->mag_n;
		fromScratch["mag_head"]=pdata->mag_heading;


		//
		fromScratch["time_hz"]=pdata->hz;
		fromScratch["time_dt"]=pdata->dt;

		//
		fromScratch["baro_p0"]=pdata->baro_p0;
		fromScratch["baro_p"]=pdata->baro_p;
		fromScratch["baro_t"]=pdata->baro_t;
		fromScratch["baro_h"]=pdata->baro_h;


                fromScratch["vbat"]=adc.vbat;

                fromScratch["motor1"]=speeds[0];
                fromScratch["motor2"]=speeds[1];
                fromScratch["motor3"]=speeds[5];
                fromScratch["motor4"]=speeds[7];


		std::string txt = fromScratch.toStyledString();

		sendto(sockfd,txt.c_str(),txt.length(),0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));

	}

	return NULL;
}

void udpstart_start(imu_data_t* imu_data){

	bzero(imu_data,sizeof(imu_data_t));

	pthread_t id;
	pthread_create(&id, 0, udpserver, imu_data);
}


