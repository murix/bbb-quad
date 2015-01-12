/*
The MIT License (MIT)

Copyright (c) 1981-2015 Murilo Pontes

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

#include "murix_pid.h"
#include "timestamps.h"

#define PID_IDX_NOW  0
#define PID_IDX_PREV 1

murix_pid::murix_pid(){

	feedback=0;
	target=0;
	actuator=0;

	kp=0;
	ki=0;
	kd=0;


	perror[PID_IDX_NOW]=0;
	perror[PID_IDX_PREV]=0;
	ierror=0;
	derror=0;


	error_min=0;
	error_max=0;

	time[PID_IDX_NOW]=0;
	time[PID_IDX_PREV]=0;
}

void murix_pid::update(){


    // time stuff
	time[PID_IDX_PREV] = time[PID_IDX_NOW];
	time[PID_IDX_NOW]= get_timestamp_in_seconds();
	dt = time[PID_IDX_NOW]-time[PID_IDX_PREV];

	// proportional term
	perror[PID_IDX_PREV] = perror[PID_IDX_NOW];
	perror[PID_IDX_NOW] = target - feedback;

	// integral term
	ierror += perror[PID_IDX_NOW] * dt;
	if(ierror>error_max) ierror=error_max;
	if(ierror<error_min) ierror=error_min;

	// derivation term
	derror =  (perror[PID_IDX_NOW]-perror[PID_IDX_PREV])/dt;

	// system action
    actuator = kp*perror[PID_IDX_NOW]+ki*ierror+kd*derror;

}


