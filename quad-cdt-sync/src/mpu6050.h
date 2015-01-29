/*
 * mpu6050.h
 *
 *  Created on: 10/08/2014
 *      Author: root
 */

#ifndef MPU6050_H_
#define MPU6050_H_

#include "i2c_linux.h"


class mpu6050 {
private:


public:


	float acc_g_x;
	float acc_g_y;
	float acc_g_z;

	float gyro_degrees_x;
	float gyro_degrees_y;
	float gyro_degrees_z;

	float gyro_radians_x;
	float gyro_radians_y;
	float gyro_radians_z;

	float temperate_celcius;

	mpu6050();
	void configure(i2c_linux i2c);
	void update(i2c_linux i2c);

};



#endif /* MPU6050_H_ */
