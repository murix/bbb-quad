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

	for (;;)
	{
		len = sizeof(cliaddr);
		n = recvfrom(sockfd,mesg,1000,0,(struct sockaddr *)&cliaddr,&len);
		mesg[n] = 0;

		Json::Value fromScratch;

		//
		fromScratch["acc_x"]=pdata->acc[0];
		fromScratch["acc_y"]=pdata->acc[1];
		fromScratch["acc_z"]=pdata->acc[2];
		fromScratch["acc_n"]=pdata->accN;

		//
		fromScratch["gyro_x"]=pdata->gyro[0];
		fromScratch["gyro_y"]=pdata->gyro[1];
		fromScratch["gyro_z"]=pdata->gyro[2];

		//
		fromScratch["mag_x"]=pdata->mag[0];
		fromScratch["mag_y"]=pdata->mag[1];
		fromScratch["mag_z"]=pdata->mag[2];
		fromScratch["mag_n"]=pdata->magN;
		fromScratch["mag_head"]=pdata->mag_heading;


		//
		fromScratch["time_hz"]=pdata->hz;
		fromScratch["time_dt"]=pdata->dt;

		//
		fromScratch["baro_p0"]=pdata->baro_p0;
		fromScratch["baro_p"]=pdata->baro_p;
		fromScratch["baro_t"]=pdata->baro_t;
		fromScratch["baro_h"]=pdata->baro_h;

		fromScratch["pitch_gyro"]=pdata->gyro_angles[0];
		fromScratch["roll_gyro"]=pdata->gyro_angles[1];
		fromScratch["yaw_gyro"]=pdata->gyro_angles[2];

		fromScratch["pitch_acc"]=pdata->acc_angles[0];
		fromScratch["roll_acc"]=pdata->acc_angles[1];
		fromScratch["yaw_acc"]=pdata->acc_angles[2];

		fromScratch["pitch_mag"]=pdata->mag_angles[0];
		fromScratch["roll_mag"]=pdata->mag_angles[1];
		fromScratch["yaw_mag"]=pdata->mag_angles[2];


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


