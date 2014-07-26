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

int main(int argc,char** argv){

	printf("enable ttyO4 overlay\r\n");
	system("echo BB-UART4 > /sys/devices/bone_capemgr.9/slots");
	printf("wait ttyO4 overlay to be ready\r\n");
	usleep(1000000);
	//
	system("gpsdctl add /dev/ttyO4");

	struct gps_data_t gps_data;
	int ret = gps_open("localhost", "2947", &gps_data);
	gps_stream(&gps_data, WATCH_ENABLE | WATCH_JSON, NULL);
	while(1){
		/* Put this in a loop with a call to a high resolution sleep () in it. */
		if (gps_waiting (&gps_data, 500)) {
			int errno = 0;
			if (gps_read (&gps_data) == -1) {
				perror("gps-read-error\r\n");
			} else {
				fprintf(stderr,"Longitude: %lf\nLatitude: %lf\nAltitude: %lf\nAccuracy: %lf\n\n",
						gps_data.fix.latitude, gps_data.fix.longitude, gps_data.fix.altitude,
						(gps_data.fix.epx>gps_data.fix.epy)?gps_data.fix.epx:gps_data.fix.epy);

			}
		}
	}
	gps_stream(&gps_data, WATCH_DISABLE, NULL);
	gps_close (&gps_data);
	return 0;
}
