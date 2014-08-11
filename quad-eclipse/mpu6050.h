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
	float acc[3];
	float gyro_raw[3];
	float gyro[3];
	float gyro_off[3];
	float tc;


	void gyro_calibration(int samples);

	void init();
	mpu6050(int fd);
	void update();

	double to_degrees(double radians);
	double to_radian(double degree);

	double t_back;
	double t_now;
	double t_diff;
	float gyro_step[3];
	float gyro_integrate[3];
	void gyro_integrate_reset();
};



#endif /* MPU6050_H_ */
