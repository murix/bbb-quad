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
	float acc[3];
	float accN;
	float acc_angles[3];


	float gyro[3];
	float gyro_angles[3];

	float mag[3];
	float magN;
	float mag_heading;
	float mag_angles[3];

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
