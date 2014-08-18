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

		pdata.acc_x=mpu.acc[0];
		pdata.acc_y=mpu.acc[1];
		pdata.acc_z=mpu.acc[2];
		pdata.acc_n=mpu.accn;
		pdata.acc_pitch=mpu.acc_pitch;
		pdata.acc_roll=mpu.acc_roll;

		pdata.gyro_x=mpu.gyro[0];
		pdata.gyro_y=mpu.gyro[1];
		pdata.gyro_z=mpu.gyro[2];
		pdata.gyro_pitch=mpu.gyro_integrate[0];
		pdata.gyro_roll=mpu.gyro_integrate[1];
		pdata.gyro_yaw=mpu.gyro_integrate[2];

		pdata.fusion_pitch=mpu.fusion_pitch;
		pdata.fusion_roll=mpu.fusion_roll;

		pdata.mag_x=mag.mag[0];
		pdata.mag_y=mag.mag[1];
		pdata.mag_z=mag.mag[2];
		pdata.mag_n=mag.magn;
		pdata.mag_heading=mag.heading;


		pdata.baro_p0=baro.P0;
		pdata.baro_p=baro.P;
		pdata.baro_t=baro.T;
		pdata.baro_h=baro.H;
	}

	return 0;
}

