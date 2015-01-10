/*
 * mpu6050.h
 *
 *  Created on: 10/08/2014
 *      Author: root
 */

#ifndef MPU6050_H_
#define MPU6050_H_


class mpu6050 {
public:
	int fd;

	float acc_g_x;
	float acc_g_y;
	float acc_g_z;

	float gyro_degrees_x;
	float gyro_degrees_y;
	float gyro_degrees_z;

	float temperate_celcius;

	mpu6050(int fd);
	void init();
	void update();

};



#endif /* MPU6050_H_ */
