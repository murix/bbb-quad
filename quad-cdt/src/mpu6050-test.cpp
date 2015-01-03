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
	while(1){
		mpu.update();
		printf("acc=%+.2f|%+.2f|%+.2f tc=%+.2f gyro=%+.2f|%+.2f|%+.2f gyro_angles(degree)=%+.2f|%+.2f|%+.2f\r\n",
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
				mpu.to_degrees(mpu.gyro_integrate[0]),
				mpu.to_degrees(mpu.gyro_integrate[1]),
				mpu.to_degrees(mpu.gyro_integrate[2])
		);
	}

	return 0;
}




