/*
 * mpu6050-test.cpp
 *
 *  Created on: 03/08/2014
 *      Author: root
 */

//printf
#include <stdio.h>


#include "bbb_i2c.h"
#include "mpu6050.h"

int main(int argc,char** argv){

	bbb_i2c i2c;
	i2c.open();

	mpu6050 mpu(i2c.fd);
	mpu.gyro_calibration(10);

	while(1){
		mpu.update();
		printf("acc=%f|%f|%f tc=%f gyro=%f|%f|%f gyro_raw=%f|%f|%f gyro_off=%f|%f|%f\r\n",
				mpu.acc[0],
				mpu.acc[1],
				mpu.acc[2],
				mpu.tc,
				mpu.gyro[0],
				mpu.gyro[1],
				mpu.gyro[2],
				mpu.gyro_raw[0],
				mpu.gyro_raw[1],
				mpu.gyro_raw[2],
				mpu.gyro_off[0],
				mpu.gyro_off[1],
				mpu.gyro_off[2]
		);
	}

	return 0;
}




