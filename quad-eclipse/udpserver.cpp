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

        int count=0;
	for (;;)
	{
                printf("udpserver count=%d\r\n",count);
		count++;
                    
		len = sizeof(cliaddr);
		n = recvfrom(sockfd,mesg,1000,0,(struct sockaddr *)&cliaddr,&len);
		mesg[n] = 0;

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


                fromScratch["vbat"]=pdata->vbat;


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


