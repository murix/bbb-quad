/*
 * udpserver-test.cpp
 *
 *  Created on: 10/08/2014
 *      Author: root
 */


//
#include "udpserver.h"

//
#include "bbb_i2c.h"
#include "mpu6050.h"
#include "hmc5883.h"
#include "ms5611.h"

//printf
#include <stdio.h>


int main(int argc,char** argv){

	bbb_i2c i2c;
	i2c.open();

	//
	mpu6050 mpu(i2c.fd);

	//
	hmc5883 mag(i2c.fd);

	//
	ms5611 baro(i2c.fd);

	//
	imu_data_t pdata;
	udpstart_start(&pdata);

	while(1){
		mpu.update();
		mag.update();
		baro.update();

		pdata.acc[0]=mpu.acc[0];
		pdata.acc[1]=mpu.acc[1];
		pdata.acc[2]=mpu.acc[2];

		pdata.gyro[0]=mpu.gyro[0];
		pdata.gyro[1]=mpu.gyro[1];
		pdata.gyro[2]=mpu.gyro[2];

		pdata.gyro_angles[0]=mpu.gyro_integrate[0];
		pdata.gyro_angles[1]=mpu.gyro_integrate[1];
		pdata.gyro_angles[2]=mpu.gyro_integrate[2];

		pdata.mag[0]=mag.mag[0];
		pdata.mag[1]=mag.mag[1];
		pdata.mag[2]=mag.mag[2];
		pdata.mag_heading=mag.heading;

		pdata.baro_p0=baro.P0;
		pdata.baro_p=baro.P;
		pdata.baro_t=baro.T;
		pdata.baro_h=baro.H;
	}

	return 0;
}

