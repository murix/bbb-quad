/*
 * hmc5883-test.cpp
 *
 *  Created on: 10/08/2014
 *      Author: root
 */



#include "bbb_i2c.h"
#include "mpu6050.h"
#include "hmc5883.h"

//printf
#include <stdio.h>


int main(int argc,char** argv){

	bbb_i2c i2c;
	i2c.open();

	//configure mpu6050 to access hmc5883
	mpu6050 mpu(i2c.fd);

	//
	hmc5883 mag(i2c.fd);


	while(1){
		mag.update();
		printf("mag=%f|%f|%f\r\n",mag.mag[0],mag.mag[1],mag.mag[2]);

	}

	return 0;
}




