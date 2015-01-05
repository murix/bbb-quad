/*
 * timestamps.cpp
 *
 *  Created on: 11/08/2014
 *      Author: root
 */


#include <stdio.h>
#include <sys/time.h>


#include "timestamps.h"


static double start_time = 0;
//
unsigned long micros(void){
	struct timeval start;
	gettimeofday(&start, NULL);
	double seconds = start.tv_sec + start.tv_usec/1000000.0;
	if(start_time==0){
		start_time=seconds;
		gettimeofday(&start, NULL);
		seconds = start.tv_sec + start.tv_usec/1000000.0;
	}
	double micros = (seconds-start_time)*1000000.0;
	unsigned long ret = micros;
	//printf("micros %lu\r\n",ret);
	return micros;
}

double get_timestamp_in_seconds(void){
	struct timeval start;
	gettimeofday(&start, NULL);
	double seconds = start.tv_sec + start.tv_usec/1000000.0;
	return seconds;
}


