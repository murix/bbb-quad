/*
 * mpu6050.cpp
 *
 *  Created on: 10/08/2014
 *      Author: root
 */

//i2c defines
#include "i2c-dev.h"
//ioctl
#include <sys/ioctl.h>
//swap
#include <endian.h>
//printf,perror
#include <stdio.h>
// int types
#include <stdint.h>


#include "mpu6050.h"

mpu6050::mpu6050(int fd){
	this->fd=fd;
	this->tc=0;

	this->gyro_off[0]=0;
	this->gyro_off[1]=0;
	this->gyro_off[2]=0;
	//
	init();
}

void mpu6050::gyro_calibration(int samples){
	//zero sum
	this->gyro_off[0]=0;
	this->gyro_off[1]=0;
	this->gyro_off[2]=0;
	//sum
	for(int i=0;i<samples;i++){
		update();
		this->gyro_off[0]+=gyro_raw[0];
		this->gyro_off[1]+=gyro_raw[1];
		this->gyro_off[2]+=gyro_raw[2];
	}
	//avg
	this->gyro_off[0] /= (float) samples;
	this->gyro_off[1] /= (float) samples;
	this->gyro_off[2] /= (float) samples;
}

void mpu6050::init(){
	//
	if(ioctl(fd,I2C_SLAVE,0x68)<0){
		perror("i2c slave mpu6050 Failed");
	}
	//master enable
	i2c_smbus_write_byte_data(fd,0x6a,0x00);
	//i2c bypass
	i2c_smbus_write_byte_data(fd,0x37,0x02);
	//wake up from sleep
	i2c_smbus_write_byte_data(fd,0x6b,0x00);
}

void mpu6050::update(){
	//
	if(ioctl(fd,I2C_SLAVE,0x68)<0){
		perror("i2c slave mpu6050 Failed");
	}
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
	gyro_raw[0]=(vs[4]/131.0);
	gyro_raw[1]=(vs[5]/131.0);
	gyro_raw[2]=(vs[6]/131.0);
	gyro[0]=gyro_raw[0]-gyro_off[0];
	gyro[1]=gyro_raw[1]-gyro_off[1];
	gyro[2]=gyro_raw[2]-gyro_off[2];
}


