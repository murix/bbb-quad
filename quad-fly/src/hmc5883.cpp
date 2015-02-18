/*
 * hmc5883.cpp
 *
 *  Created on: 10/08/2014
 *      Author: root
 */

//ioctl
#include <sys/ioctl.h>


//apt-get install libi2c-dev
#include <linux/i2c-dev.h>


// int types
#include <stdint.h>
//printf,perror
#include <stdio.h>
//read,write
#include <unistd.h>
//
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "hmc5883.h"


hmc5883::hmc5883(){
	mag_x=0;
	mag_y=0;
	mag_z=0;
}

#define REG_CONFIG_A 0 //Configuration Register A Read/Write
#define REG_CONFIG_B 1 //Configuration Register B Read/Write
#define REG_MODE     2 //Mode Register Read/Write
#define REG_X_MSB    3 //Data Output X MSB Register Read
#define REG_X_LSB    4 //Data Output X LSB Register Read
#define REG_Z_MSB    5 //Data Output Z MSB Register Read
#define REG_Z_LSB    6 //Data Output Z LSB Register Read
#define REG_Y_MSB    7 //Data Output Y MSB Register Read
#define REG_Y_LSB    8 //Data Output Y LSB Register Read
#define REG_STATUS   9 //Status Register Read
#define REG_ID_A    10 //Identification Register A Read
#define REG_ID_B    11 //Identification Register B Read
#define REG_ID_C    12 //Identification Register C Read


void hmc5883::update(i2c_linux i2c){
	if(is_ready(i2c)){
		mag_x= read16(i2c,REG_X_MSB,REG_X_LSB)/1090.0;
		mag_y= read16(i2c,REG_Y_MSB,REG_Y_LSB)/1090.0;
		mag_z= read16(i2c,REG_Z_MSB,REG_Z_LSB)/1090.0;

	}
}

#define ADDR_HMC5883 0x1e

void hmc5883::set_addr(i2c_linux i2c){
	if (ioctl(i2c.fd,I2C_SLAVE,ADDR_HMC5883) < 0) {
		perror("i2c slave hmc5883 failed");
	}
}

void hmc5883::configure(i2c_linux i2c){
	set_addr(i2c);
	uint8_t buffer[2];

	buffer[0]=REG_CONFIG_A;
	buffer[1]=0x78; // 8 average, 75hz , normal flow
	write(i2c.fd,buffer,2);

	buffer[0]=REG_CONFIG_B;
	buffer[1]=0x20; // 1090 lsb/gauss
	write(i2c.fd,buffer,2);

	buffer[0]=REG_MODE;
	buffer[1]=0x00; // continuous mode
	write(i2c.fd,buffer,2);

}

bool hmc5883::is_ready(i2c_linux i2c){
	set_addr(i2c);

	//wait to be ready
	uint8_t buffer[2];
	buffer[0]=REG_STATUS;
	write(i2c.fd,buffer,1);
	read(i2c.fd,buffer,1);
	if( (buffer[0] & 0x01) == 1){
		return true;
	} else {
		return false;
	}
}

double hmc5883::read16(i2c_linux i2c,int reg_msb,int reg_lsb){
	set_addr(i2c);

	uint8_t buffer[2];
	uint8_t msb;
	uint8_t lsb;
	uint16_t *p16;
	p16=(uint16_t*)buffer;
	buffer[0]=reg_msb;
	write(i2c.fd,buffer,1);
	read(i2c.fd,&msb,1);
	buffer[0]=reg_lsb;
	write(i2c.fd,buffer,1);
	read(i2c.fd,&lsb,1);
	buffer[0]=lsb;
	buffer[1]=msb;
	int16_t value = p16[0];
	double fvalue = value;
	return fvalue;
}





