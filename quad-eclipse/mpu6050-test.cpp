/*
 * mpu6050-test.cpp
 *
 *  Created on: 03/08/2014
 *      Author: root
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

//open
#include <unistd.h>
#include <fcntl.h>

//i2c defines
#include "i2c-dev.h"

//ioctl
#include <sys/ioctl.h>







#include <endian.h>


class mpu6050 {
public:
	int fd;
	float acc[3];
	float gyro[3];
	float tc;

	mpu6050(int fd){
		this->fd=fd;
	}
	void update(){
		ioctl(fd,I2C_SLAVE,0x68);
		//master enable
		i2c_smbus_write_byte_data(fd,0x6a,0x00);
		//i2c bypass
		i2c_smbus_write_byte_data(fd,0x37,0x02);
		//wake up from sleep
		i2c_smbus_write_byte_data(fd,0x6b,0x00);
		//read all
		uint16_t vu[7];
		int16_t vs[7];
		i2c_smbus_read_i2c_block_data(fd,0x3b,14,(uint8_t*)vu);
		for(int i=0;i<7;i++){
			vs[i]=(int16_t) __bswap_16(vu[i]);
		}
		//
		acc[0] =vs[0]/16384.0;
		acc[1] =vs[1]/16384.0;
		acc[2] =vs[2]/16384.0;
		tc     =vs[3]/340.0 + 36.53;
		gyro[0]=vs[4]/131.0;
		gyro[1]=vs[5]/131.0;
		gyro[2]=vs[6]/131.0;
	}
};


int main(uint argc,char** argv){

	//system("echo BB-I2C1 > /sys/devices/bone_capemgr.9/slots");
	//usleep(1*1000*1000);


	int fd;
	if ((fd = open("/dev/i2c-2",O_RDWR|O_LARGEFILE)) < 0) {
		printf("Failed to open the bus.");
		exit(1);
	}

	mpu6050 mpu(fd);

	while(1){
		mpu.update();
		printf("|%f|%f|%f|%f|%f|%f|%f|\r\n",
				mpu.acc[0],
				mpu.acc[1],
				mpu.acc[2],
				mpu.tc,
				mpu.gyro[0],
				mpu.gyro[1],
				mpu.gyro[2]);
	}

	return 0;

}




