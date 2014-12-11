/*
 * bbb_i2c.cpp
 *
 *  Created on: 10/08/2014
 *      Author: root
 */

//printf
#include <stdio.h>

//system
#include <stdlib.h>

//open
#include <unistd.h>
#include <fcntl.h>

#include "bbb_i2c.h"

void bbb_i2c::open(){

	//system("echo BB-I2C1 > /sys/devices/bone_capemgr.9/slots");
	//usleep(1*1000*1000);

	if ((this->fd = ::open("/dev/i2c-1",O_RDWR|O_LARGEFILE)) < 0) {
		printf("Failed to open the bus.");
		exit(1);
	}

}


