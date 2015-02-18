/*
 * mpu6050.cpp
 *
 *  Created on: 10/08/2014
 *      Author: root
 */

//apt-get install libi2c-dev
#include <linux/i2c-dev.h>


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



//http://www.invensense.com/mems/gyro/mpu6050.html
//http://www.invensense.com/mems/gyro/documents/RM-MPU-6000A-00v4.2.pdf


//
#define MPU6050_REG_SELF_TEST_X   0x0d
#define MPU6050_REG_SELF_TEST_Y   0x0e
#define MPU6050_REG_SELF_TEST_Z   0x0f
#define MPU6050_REG_SELF_TEST_A   0x10

#define MPU6050_REG_SMPLRT_DIV     0x19
#define MPU6050_REG_CONFIG         0x1a
#define MPU6050_REG_GYRO_CONFIG    0x1b
#define MPU6050_REG_ACCEL_CONFIG   0x1c
#define MPU6050_REG_FIFO_EN        0x23

#define MPU6050_REG_I2C_MST_CTRL   0x24

#define MPU6050_REG_I2C_SLV0_ADDR  0x25
#define MPU6050_REG_I2C_SLV0_REG   0x26
#define MPU6050_REG_I2C_SLV0_CTRL  0x27

#define MPU6050_REG_I2C_SLV1_ADDR  0x28
#define MPU6050_REG_I2C_SLV1_REG   0x29
#define MPU6050_REG_I2C_SLV1_CTRL  0x2a

#define MPU6050_REG_I2C_SLV2_ADDR  0x2b
#define MPU6050_REG_I2C_SLV2_REG   0x2c
#define MPU6050_REG_I2C_SLV2_CTRL  0x2d

#define MPU6050_REG_I2C_SLV3_ADDR  0x2e
#define MPU6050_REG_I2C_SLV3_REG   0x2f
#define MPU6050_REG_I2C_SLV3_CTRL  0x30

#define MPU6050_REG_I2C_SLV4_ADDR  0x31
#define MPU6050_REG_I2C_SLV4_REG   0x32
#define MPU6050_REG_I2C_SLV4_DO    0x33
#define MPU6050_REG_I2C_SLV4_CTRL  0x34
#define MPU6050_REG_I2C_SLV4_DI    0x35

#define MPU6050_REG_I2C_MST_STATUS 0x36

#define MPU6050_REG_INT_PIN_CFG    0x37
#define MPU6050_REG_INT_ENABLE     0x38
#define MPU6050_REG_INT_STATUS     0x3A

#define MPU6050_REG_ACCEL_XOUT_H   0x3b
#define MPU6050_REG_ACCEL_XOUT_L   0x3c
#define MPU6050_REG_ACCEL_YOUT_H   0x3d
#define MPU6050_REG_ACCEL_YOUT_L   0x3e
#define MPU6050_REG_ACCEL_ZOUT_H   0x3f
#define MPU6050_REG_ACCEL_ZOUT_L   0x40

#define MPU6050_REG_TEMP_OUT_H     0x41
#define MPU6050_REG_TEMP_OUT_L     0x42

#define MPU6050_REG_GYRO_XOUT_H    0x43
#define MPU6050_REG_GYRO_XOUT_L    0x44
#define MPU6050_REG_GYRO_YOUT_H    0x45
#define MPU6050_REG_GYRO_YOUT_L    0x46
#define MPU6050_REG_GYRO_ZOUT_H    0x47
#define MPU6050_REG_GYRO_ZOUT_L    0x48

#define MPU6050_REG_EXT_SENS_DATA_00 0x49
#define MPU6050_REG_EXT_SENS_DATA_01 0x4a
#define MPU6050_REG_EXT_SENS_DATA_02 0x4b
#define MPU6050_REG_EXT_SENS_DATA_03 0x4c
#define MPU6050_REG_EXT_SENS_DATA_04 0x4d
#define MPU6050_REG_EXT_SENS_DATA_05 0x4e
#define MPU6050_REG_EXT_SENS_DATA_06 0x4f
#define MPU6050_REG_EXT_SENS_DATA_07 0x50
#define MPU6050_REG_EXT_SENS_DATA_08 0x51
#define MPU6050_REG_EXT_SENS_DATA_09 0x52
#define MPU6050_REG_EXT_SENS_DATA_10 0x53
#define MPU6050_REG_EXT_SENS_DATA_11 0x54
#define MPU6050_REG_EXT_SENS_DATA_12 0x55
#define MPU6050_REG_EXT_SENS_DATA_13 0x56
#define MPU6050_REG_EXT_SENS_DATA_14 0x57
#define MPU6050_REG_EXT_SENS_DATA_15 0x58
#define MPU6050_REG_EXT_SENS_DATA_16 0x59
#define MPU6050_REG_EXT_SENS_DATA_17 0x5a
#define MPU6050_REG_EXT_SENS_DATA_18 0x5b
#define MPU6050_REG_EXT_SENS_DATA_19 0x5c
#define MPU6050_REG_EXT_SENS_DATA_20 0x5d
#define MPU6050_REG_EXT_SENS_DATA_21 0x5e
#define MPU6050_REG_EXT_SENS_DATA_22 0x5f
#define MPU6050_REG_EXT_SENS_DATA_23 0x60

#define MPU6050_REG_SLV_DO 0x63
#define MPU6050_REG_SLV_D1 0x64
#define MPU6050_REG_SLV_D2 0x65
#define MPU6050_REG_SLV_D3 0x66

#define MPU6050_REG_I2C_MST_DELAY_CTRL 0x67
#define MPU6050_REG_SIGNAL_PATH_RESET  0x68
#define MPU6050_REG_USER_CTRL          0x6a
#define MPU6050_REG_PWR_MGMT_1         0x6b
#define MPU6050_REG_PWR_MGMT_2         0x6c
#define MPU6050_REG_FIFO_COUNTH        0x72
#define MPU6050_REG_FIFOLCOUTNL        0x73
#define MPU6050_REG_FIFO_R_W           0x74
#define MPU6050_REG_WHO_AM_I           0x75

#define MPU6050_REG_WHO_AMI_I_REPLY    0x68
#define MPU6050_REG_PWR_MGMT_1_DEFAULT 0x40

#define MPU6050_DLPF_CFG_ACCEL_260HZ_DELAY_0_US_GYRO_256HZ_DELAY_980US_FS_8KHZ 0
#define MPU6050_DLPF_CFG_ACCEL_184HZ_DELAY_2000_US_GYRO_188HZ_DELAY_1900US_FS_1KHZ 1
#define MPU6050_DLPF_CFG_ACCEL_94HZ_DELAY_3000_US_GYRO_98HZ_DELAY_2800US_FS_1KHZ 2
#define MPU6050_DLPF_CFG_ACCEL_44HZ_DELAY_4900_US_GYRO_42HZ_DELAY_4800US_FS_1KHZ 3
#define MPU6050_DLPF_CFG_ACCEL_21HZ_DELAY_8500_US_GYRO_20HZ_DELAY_8300US_FS_1KHZ 4
#define MPU6050_DLPF_CFG_ACCEL_10HZ_DELAY_13800_US_GYRO_10HZ_DELAY_13400US_FS_1KHZ 5
#define MPU6050_DLPF_CFG_ACCEL_5HZ_DELAY_19000_US_GYRO_5HZ_DELAY_18600US_FS_1KHZ 6
#define MPU6050_DLPF_RESERVED                                                  7


#define MPU6050_AFS_SEL_2G         0
#define MPU6050_AFS_SEL_4G         1
#define MPU6050_AFS_SEL_8G         2
#define MPU6050_AFS_SEL_16G        3

#define MPU6050_AFS_DIV_2G   16384.0
#define MPU6050_AFS_DIV_4G    8192.0
#define MPU6050_AFS_DIV_8G    4096.0
#define MPU6050_AFS_DIV_16G   2048.0

#define MPU6050_GFS_SEL_250        0
#define MPU6050_GFS_SEL_500        1
#define MPU6050_GFS_SEL_100        2
#define MPU6050_GFS_SEL_2000       3

#define MPU6050_GFS_DIV_250    131.0
#define MPU6050_GFS_DIV_500     65.5
#define MPU6050_GFS_DIV_100     32.8
#define MPU6050_GFS_DIV_2000    16.4




#include "mpu6050.h"
#include "timestamps.h"

mpu6050::mpu6050(){
	this->temperate_celcius=0;
	this->acc_g_x=0;
	this->acc_g_y=0;
	this->acc_g_z=0;
	this->gyro_degrees_x=0;
	this->gyro_degrees_y=0;
	this->gyro_degrees_z=0;

}


void mpu6050::configure(i2c_linux i2c){
	//
	if(ioctl(i2c.fd,I2C_SLAVE,0x68)<0){
		perror("i2c slave mpu6050 Failed");
		exit(1);
	}

	//4.1 Registers 13 to 16 – Self Test Registers


	//4.2 Register 25 – Sample Rate Divider

	//4.3 Register 26 – Configuration

	// MPU6050_REG_CONFIG = FSYNC DISABLE, configure DLPF
	i2c_smbus_write_byte_data(i2c.fd,MPU6050_REG_CONFIG,MPU6050_DLPF_CFG_ACCEL_260HZ_DELAY_0_US_GYRO_256HZ_DELAY_980US_FS_8KHZ);
	//i2c_smbus_write_byte_data(i2c.fd,MPU6050_REG_CONFIG,MPU6050_DLPF_CFG_ACCEL_184HZ_DELAY_2000_US_GYRO_188HZ_DELAY_1900US_FS_1KHZ);
	//i2c_smbus_write_byte_data(i2c.fd,MPU6050_REG_CONFIG,MPU6050_DLPF_CFG_ACCEL_94HZ_DELAY_3000_US_GYRO_98HZ_DELAY_2800US_FS_1KHZ);
	//i2c_smbus_write_byte_data(i2c.fd,MPU6050_REG_CONFIG,MPU6050_DLPF_CFG_ACCEL_44HZ_DELAY_4900_US_GYRO_42HZ_DELAY_4800US_FS_1KHZ);
	//i2c_smbus_write_byte_data(i2c.fd,MPU6050_REG_CONFIG,MPU6050_DLPF_CFG_ACCEL_21HZ_DELAY_8500_US_GYRO_20HZ_DELAY_8300US_FS_1KHZ);
	//i2c_smbus_write_byte_data(i2c.fd,MPU6050_REG_CONFIG,MPU6050_DLPF_CFG_ACCEL_10HZ_DELAY_13800_US_GYRO_10HZ_DELAY_13400US_FS_1KHZ);
	//i2c_smbus_write_byte_data(i2c.fd,MPU6050_REG_CONFIG,MPU6050_DLPF_CFG_ACCEL_5HZ_DELAY_19000_US_GYRO_5HZ_DELAY_18600US_FS_1KHZ);


	//4.4 Register 27 – Gyroscope Configuration

	i2c_smbus_write_byte_data(i2c.fd,MPU6050_REG_GYRO_CONFIG, MPU6050_GFS_SEL_2000 << 3);


	//4.5 Register 28 – Accelerometer Configuration

	i2c_smbus_write_byte_data(i2c.fd,MPU6050_REG_ACCEL_CONFIG, MPU6050_AFS_SEL_16G << 3);


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
	i2c_smbus_write_byte_data(i2c.fd,MPU6050_REG_INT_PIN_CFG,0x02);


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
	i2c_smbus_write_byte_data(i2c.fd,MPU6050_REG_USER_CTRL,0x00);

	//4.28 Register 107 – Power Management 1

	//MPU6050_REG_PWR_MGMT_1 = disable sleep, cycle not used, temp enabled, clk=internal 8mhz
	i2c_smbus_write_byte_data(i2c.fd,MPU6050_REG_PWR_MGMT_1,0x00);

	//4.29 Register 108 – Power Management 2

	//4.30 Register 114 and 115 – FIFO Count Registers

	//4.31 Register 116 – FIFO Read Write

	//4.32 Register 117 – Who Am I

	while( i2c_smbus_read_byte_data(i2c.fd,MPU6050_REG_WHO_AM_I) != MPU6050_REG_WHO_AMI_I_REPLY ){
		printf("mpu6050 who am i error\r\n");
	}


}

void mpu6050::update(i2c_linux i2c){
	//
	if(ioctl(i2c.fd,I2C_SLAVE,0x68)<0){
		perror("i2c slave mpu6050 Failed");
	}

	//read all
	uint16_t vu[7];
	int16_t vs[7];

	//read all - accel, temp, gyro
	i2c_smbus_read_i2c_block_data(i2c.fd,MPU6050_REG_ACCEL_XOUT_H,14,(uint8_t*)vu);

	for(int i=0;i<7;i++){
		vs[i]=(int16_t) __bswap_16(vu[i]);
	}

	//
	acc_g_x           =vs[0]/MPU6050_AFS_DIV_16G;
	acc_g_y           =vs[1]/MPU6050_AFS_DIV_16G;
	acc_g_z           =vs[2]/MPU6050_AFS_DIV_16G;
	temperate_celcius =vs[3]/340.0 + 36.53;
	gyro_degrees_x    =vs[4]/MPU6050_GFS_DIV_2000;
	gyro_degrees_y    =vs[5]/MPU6050_GFS_DIV_2000;
	gyro_degrees_z    =vs[6]/MPU6050_GFS_DIV_2000;

	float to_radian_per_second = M_PI/180.0;
	gyro_radians_x = gyro_degrees_x*to_radian_per_second;
	gyro_radians_y = gyro_degrees_y*to_radian_per_second;
	gyro_radians_z = gyro_degrees_z*to_radian_per_second;

}


