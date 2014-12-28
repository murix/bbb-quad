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
#include <stdio.h>
#include <stdlib.h>
//sqrt
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


#include "mpu6050.h"
#include "timestamps.h"

mpu6050::mpu6050(int fd){
	this->fd=fd;
	this->tc=0;

	this->gyro_off[0]=0;
	this->gyro_off[1]=0;
	this->gyro_off[2]=0;

	//
	init();
	//pre calibration
	gyro_calibration(330); 
        // sample frequency is about 330hz
        // 330 samples is about 1 second

	gyro_integrate_reset();
	this->t_back=get_timestamp_in_seconds();
	this->t_now=get_timestamp_in_seconds();
	this->t_diff=0;

	this->fusion_pitch=0;
	this->fusion_roll=0;
}

void mpu6050::gyro_integrate_reset(){
	//
	this->gyro_integrate[0]=0;
	this->gyro_integrate[1]=0;
	this->gyro_integrate[2]=0;
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

	//
	gyro_integrate_reset();
}

void mpu6050::init(){
	//
	if(ioctl(fd,I2C_SLAVE,0x68)<0){
		perror("i2c slave mpu6050 Failed");
		exit(1);
	}

	//4.1 Registers 13 to 16 – Self Test Registers


	//4.2 Register 25 – Sample Rate Divider

	//4.3 Register 26 – Configuration

	// MPU6050_REG_CONFIG = FSYNC DISABLE, configure DLPF
	//i2c_smbus_write_byte_data(fd,MPU6050_REG_CONFIG,MPU6050_DLPF_CFG_ACCEL_260HZ_DELAY_0_US_GYRO_256HZ_DELAY_980US_FS_8KHZ);
	//i2c_smbus_write_byte_data(fd,MPU6050_REG_CONFIG,MPU6050_DLPF_CFG_ACCEL_184HZ_DELAY_2000_US_GYRO_188HZ_DELAY_1900US_FS_1KHZ);
	//i2c_smbus_write_byte_data(fd,MPU6050_REG_CONFIG,MPU6050_DLPF_CFG_ACCEL_94HZ_DELAY_3000_US_GYRO_98HZ_DELAY_2800US_FS_1KHZ);
	//i2c_smbus_write_byte_data(fd,MPU6050_REG_CONFIG,MPU6050_DLPF_CFG_ACCEL_44HZ_DELAY_4900_US_GYRO_42HZ_DELAY_4800US_FS_1KHZ);
	//i2c_smbus_write_byte_data(fd,MPU6050_REG_CONFIG,MPU6050_DLPF_CFG_ACCEL_21HZ_DELAY_8500_US_GYRO_20HZ_DELAY_8300US_FS_1KHZ);
	//i2c_smbus_write_byte_data(fd,MPU6050_REG_CONFIG,MPU6050_DLPF_CFG_ACCEL_10HZ_DELAY_13800_US_GYRO_10HZ_DELAY_13400US_FS_1KHZ);
	i2c_smbus_write_byte_data(fd,MPU6050_REG_CONFIG,MPU6050_DLPF_CFG_ACCEL_5HZ_DELAY_19000_US_GYRO_5HZ_DELAY_18600US_FS_1KHZ);


	//4.4 Register 27 – Gyroscope Configuration

	i2c_smbus_write_byte_data(fd,MPU6050_REG_GYRO_CONFIG, MPU6050_GFS_SEL_2000 << 3);


	//4.5 Register 28 – Accelerometer Configuration

	i2c_smbus_write_byte_data(fd,MPU6050_REG_ACCEL_CONFIG, MPU6050_AFS_SEL_16G << 3);


	//4.6 Register 35 – FIFO Enable

	//4.7 Register 36 – I2C Master Control

	//4.8 Registers 37 to 39 – I2C Slave 0 Control

	//4.9 Registers 40 to 42 – I2C Slave 1 Control

	//4.10 Registers 43 to 45 – I2C Slave 2 Control

	//4.11 Registers 46 to 48 – I2C Slave 3 Control

	//4.12 Registers 49 to 53 – I2C Slave 4 Control

	//4.13 Register 54 – I2C Master Status

	//4.14 Register 55 – INT Pin / Bypass Enable Configuration

	//INT_PIN_CFG = int_level zero, int_open zero, latch_int_en zero,
	// int_rd_clear zero, fsync_int_level zero, fsync_en zero, i2c_by_pass_en enabled
	i2c_smbus_write_byte_data(fd,MPU6050_REG_INT_PIN_CFG,0x02);


	//4.15 Register 56 – Interrupt Enable

	//4.16 Register 58 – Interrupt Status

	//4.17 Registers 59 to 64 – Accelerometer Measurements
	//AFS_SEL Full Scale Range LSB Sensitivity
	//0 +-2g 16384 LSB/g
	//1 +-4g 8192 LSB/g
	//2 +-8g 4096 LSB/g
	//3 +-16g 2048 LSB/g


	//4.18 Registers 65 and 66 – Temperature Measurement
	//Temperature in degrees C = (TEMP_OUT Register Value as a signed quantity)/340 + 36.53


	//4.19 Registers 67 to 72 – Gyroscope Measurements
	//FS_SEL Full Scale Range LSB Sensitivity
	//0 +- 250 °/s 131 LSB/°/s
	//1 +- 500 °/s 65.5 LSB/°/s
	//2 +- 1000 °/s 32.8 LSB/°/s
	//3 +- 2000 °/s 16.4 LSB/°/s


	//4.20 Registers 73 to 96 – External Sensor Data

	//4.21 Register 99 – I2C Slave 0 Data Out

	//4.22 Register 100 – I2C Slave 1 Data Out

	//4.23 Register 101 – I2C Slave 2 Data Out

	//4.24 Register 102 – I2C Slave 3 Data Out

	//4.25 Register 103 – I2C Master Delay Control

	//4.26 Register 104 – Signal Path Reset

	//4.27 Register 106 – User Control

	//MPU6050_REG_USER_CTRL = fifo disabled, i2c master disabled (enabled i2c pass throught),
	//i2c primary enable, not fifo reset, not i2c mst reset, not reset signal path
	i2c_smbus_write_byte_data(fd,MPU6050_REG_USER_CTRL,0x00);

	//4.28 Register 107 – Power Management 1

	//MPU6050_REG_PWR_MGMT_1 = disable sleep, cycle not used, temp enabled, clk=internal 8mhz
	i2c_smbus_write_byte_data(fd,MPU6050_REG_PWR_MGMT_1,0x00);

	//4.29 Register 108 – Power Management 2

	//4.30 Register 114 and 115 – FIFO Count Registers

	//4.31 Register 116 – FIFO Read Write

	//4.32 Register 117 – Who Am I

	while( i2c_smbus_read_byte_data(fd,MPU6050_REG_WHO_AM_I) != MPU6050_REG_WHO_AMI_I_REPLY ){
         printf("mpu6050 who am i error\r\n");
	}


}

void mpu6050::update(){
	//
	t_back=t_now;
	t_now=get_timestamp_in_seconds();
	t_diff=t_now-t_back;
	//printf("%f\r\n",t_diff);

	//
	if(ioctl(fd,I2C_SLAVE,0x68)<0){
		perror("i2c slave mpu6050 Failed");
	}
	//read all
	uint16_t vu[7];
	int16_t vs[7];

	//read all - accel, temp, gyro
	i2c_smbus_read_i2c_block_data(fd,MPU6050_REG_ACCEL_XOUT_H,14,(uint8_t*)vu);

	for(int i=0;i<7;i++){
		vs[i]=(int16_t) __bswap_16(vu[i]);
	}
	//
	acc[0] =vs[0]/MPU6050_AFS_DIV_16G;
	acc[1] =vs[1]/MPU6050_AFS_DIV_16G;
	acc[2] =vs[2]/MPU6050_AFS_DIV_16G;

	tc     =vs[3]/340.0 + 36.53;

	gyro_raw[0]=vs[4]/MPU6050_GFS_DIV_2000;
	gyro_raw[1]=vs[5]/MPU6050_GFS_DIV_2000;
	gyro_raw[2]=vs[6]/MPU6050_GFS_DIV_2000;


	gyro[0]=gyro_raw[0]-gyro_off[0];
	gyro[1]=gyro_raw[1]-gyro_off[1];
	gyro[2]=gyro_raw[2]-gyro_off[2];

	//radian = speed * time
	gyro_step[0]=to_radian(gyro[0])*t_diff;
	gyro_step[1]=to_radian(gyro[1])*t_diff;
	gyro_step[2]=to_radian(gyro[2])*t_diff;

	//radian
	gyro_integrate[0] += gyro_step[0];
	gyro_integrate[1] += gyro_step[1];
	gyro_integrate[2] += gyro_step[2];


	//accelerometer
	accn=sqrt(pow(acc[0],2)+pow(acc[1],2)+pow(acc[2],2));


	//
	acc_pitch=atan2(-acc[1],sqrt(pow(acc[0],2)+pow(acc[2],2)));
	acc_roll =atan2(-acc[0],sqrt(pow(acc[1],2)+pow(acc[2],2)));

	//
	double alpha=0.98;
	if(acc[2]<0) alpha=1;
	fusion_pitch = alpha*(fusion_pitch + gyro_step[0] ) + (1-alpha)*acc_pitch;
	fusion_roll  = alpha*(fusion_roll  + gyro_step[1] ) + (1-alpha)*acc_roll;


}

double mpu6050::to_degrees(double radians){
	return radians*(180.0/M_PI);
}
double mpu6050::to_radian(double degree){
	return degree * M_PI/180;
}

