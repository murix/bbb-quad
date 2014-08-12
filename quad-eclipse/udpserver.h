/*
 * udpserver.h
 *
 *  Created on: 10/08/2014
 *      Author: root
 */

#ifndef UDPSERVER_H_
#define UDPSERVER_H_

typedef struct {
	// ARM: float and uint32_t are thread safe
	//
	float acc_x;
	float acc_y;
	float acc_z;
	float acc_n;
	float acc_pitch;
	float acc_roll;

	float gyro_x;
	float gyro_y;
	float gyro_z;
	float gyro_pitch;
	float gyro_roll;
	float gyro_yaw;

	float fusion_pitch;
	float fusion_roll;

	float mag_x;
	float mag_y;
	float mag_z;
	float mag_n;
	float mag_heading;
	//
	//
	float dt;
	float hz;
	//
	float baro_p0;
	float baro_p;
	float baro_t;
	float baro_h;

} imu_data_t;

void udpstart_start(imu_data_t* imu_data);

#endif /* UDPSERVER_H_ */
