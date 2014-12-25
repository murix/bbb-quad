/*
 * gps.cpp
 *
 *  Created on: 26/07/2014
 *      Author: root
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <gps.h>
#include <time.h>

int main(int argc,char** argv){

	printf("enable ttyO4 overlay\r\n");
	system("echo BB-UART4 > /sys/devices/bone_capemgr.9/slots");
	printf("wait ttyO4 overlay to be ready\r\n");
	usleep(1000*1000);
	//
	system("gpsdctl add /dev/ttyO4");

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
			}
		}
	}
	gps_stream(&gps_data, WATCH_DISABLE, NULL);
	gps_close (&gps_data);
	return 0;
}
