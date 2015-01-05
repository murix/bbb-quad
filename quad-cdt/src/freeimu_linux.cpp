/*
 * freeimu_linux.cpp
 *
 *  Created on: 04/01/2015
 *      Author: murix
 */

#include "freeimu_linux.h"

#include <math.h>
#include "timestamps.h"


#define twoKpDef  (2.0 * 0.5) // 2 * proportional gain
#define twoKiDef  (2.0 * 0.1) // 2 * integral gain


FreeIMU::FreeIMU() {

	// initialize quaternion
	q0 = 1.0;
	q1 = 0.0;
	q2 = 0.0;
	q3 = 0.0;

	//
	integralFBx = 0.0;
	integralFBy = 0.0;
	integralFBz = 0.0;

	//
	sampleFreq=1.0;
	lastUpdate = get_timestamp_in_seconds();
	now = get_timestamp_in_seconds();

}




/**
 * Quaternion implementation of the 'DCM filter' [Mayhony et al].
 *
 * Incorporates the magnetic distortion compensation algorithms
 * from Sebastian Madgwick's filter which eliminates
 * the need for a reference direction of flux (bx bz)
 * to be predefined and limits the effect of magnetic distortions to yaw axis only.
 *
 * @see: http://www.x-io.co.uk/node/8#open_source_ahrs_and_imu_algorithms
 */

void  FreeIMU::AHRSupdate(double gx, double gy, double gz, double ax, double ay, double az, double mx, double my, double mz) {
	double recipNorm;
	double q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;
	double halfex = 0.0f, halfey = 0.0f, halfez = 0.0f;
	double qa, qb, qc;

	// Auxiliary variables to avoid repeated arithmetic
	q0q0 = q0 * q0;
	q0q1 = q0 * q1;
	q0q2 = q0 * q2;
	q0q3 = q0 * q3;
	q1q1 = q1 * q1;
	q1q2 = q1 * q2;
	q1q3 = q1 * q3;
	q2q2 = q2 * q2;
	q2q3 = q2 * q3;
	q3q3 = q3 * q3;

	// Use magnetometer measurement only when valid (avoids NaN in magnetometer normalisation)
	if((mx != 0.0f) && (my != 0.0f) && (mz != 0.0f)) {
		double hx, hy, bx, bz;
		double halfwx, halfwy, halfwz;

		// Normalise magnetometer measurement
		recipNorm = 1.0/sqrt(mx * mx + my * my + mz * mz);
		mx *= recipNorm;
		my *= recipNorm;
		mz *= recipNorm;

		// Reference direction of Earth's magnetic field
		hx = 2.0f * (mx * (0.5f - q2q2 - q3q3) + my * (q1q2 - q0q3) + mz * (q1q3 + q0q2));
		hy = 2.0f * (mx * (q1q2 + q0q3) + my * (0.5f - q1q1 - q3q3) + mz * (q2q3 - q0q1));
		bx = sqrt(hx * hx + hy * hy);
		bz = 2.0f * (mx * (q1q3 - q0q2) + my * (q2q3 + q0q1) + mz * (0.5f - q1q1 - q2q2));

		// Estimated direction of magnetic field
		halfwx = bx * (0.5f - q2q2 - q3q3) + bz * (q1q3 - q0q2);
		halfwy = bx * (q1q2 - q0q3) + bz * (q0q1 + q2q3);
		halfwz = bx * (q0q2 + q1q3) + bz * (0.5f - q1q1 - q2q2);

		// Error is sum of cross product between estimated direction and measured direction of field vectors
		halfex = (my * halfwz - mz * halfwy);
		halfey = (mz * halfwx - mx * halfwz);
		halfez = (mx * halfwy - my * halfwx);
	}

	// Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
	if((ax != 0.0f) && (ay != 0.0f) && (az != 0.0f)) {
		double halfvx, halfvy, halfvz;

		// Normalise accelerometer measurement
		recipNorm = 1.0/sqrt(ax * ax + ay * ay + az * az);
		ax *= recipNorm;
		ay *= recipNorm;
		az *= recipNorm;

		// Estimated direction of gravity
		halfvx = q1q3 - q0q2;
		halfvy = q0q1 + q2q3;
		halfvz = q0q0 - 0.5f + q3q3;

		// Error is sum of cross product between estimated direction and measured direction of field vectors
		halfex += (ay * halfvz - az * halfvy);
		halfey += (az * halfvx - ax * halfvz);
		halfez += (ax * halfvy - ay * halfvx);
	}

	// Apply feedback only when valid data has been gathered from the accelerometer or magnetometer
	if(halfex != 0.0f && halfey != 0.0f && halfez != 0.0f) {

		// Compute and apply integral feedback if enabled
		if(twoKiDef > 0.0f) {
			// integral error scaled by Ki
			integralFBx += twoKiDef * halfex * (1.0f / sampleFreq);
			integralFBy += twoKiDef * halfey * (1.0f / sampleFreq);
			integralFBz += twoKiDef * halfez * (1.0f / sampleFreq);
			// apply integral feedback
			gx += integralFBx;
			gy += integralFBy;
			gz += integralFBz;
		}
		else {
			// prevent integral windup (dead code?)
			integralFBx = 0.0f;
			integralFBy = 0.0f;
			integralFBz = 0.0f;
		}

		// Apply proportional feedback
		gx += twoKpDef * halfex;
		gy += twoKpDef * halfey;
		gz += twoKpDef * halfez;
	}

	// Integrate rate of change of quaternion
	gx *= (0.5f * (1.0f / sampleFreq));   // pre-multiply common factors
	gy *= (0.5f * (1.0f / sampleFreq));
	gz *= (0.5f * (1.0f / sampleFreq));
	qa = q0;
	qb = q1;
	qc = q2;
	q0 += (-qb * gx - qc * gy - q3 * gz);
	q1 += (qa * gx + qc * gz - q3 * gy);
	q2 += (qa * gy - qb * gz + q3 * gx);
	q3 += (qa * gz + qb * gy - qc * gx);

	// Normalise quaternion
	recipNorm = 1.0/sqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
	q0 *= recipNorm;
	q1 *= recipNorm;
	q2 *= recipNorm;
	q3 *= recipNorm;
}


/**
 * Populates array q with a quaternion representing the IMU orientation with respect to the Earth
 *
 * @param q the quaternion to populate
 */
void FreeIMU::getQ(double * q,double ax,double ay,double az, double gx,double gy,double gz, double mx,double my,double mz) {


	//
	lastUpdate = now;
	now = get_timestamp_in_seconds();
	double period = (now - lastUpdate);

	// frequency = 1 / period
	sampleFreq = 1.0 / period;

	// gyro values are expressed in deg/sec, the * M_PI/180 will convert it to radians/sec
	AHRSupdate(gx, gy, gz, ax, ay, az, mx, my, mz);

	//
	q[0] = q0;
	q[1] = q1;
	q[2] = q2;
	q[3] = q3;
}




/**
 * Returns an altitude estimate from baromether readings only using sea_press as current sea level pressure
 */
double FreeIMU::getBaroAlt(double sea_press,double temp,double press) {
	return ((pow((sea_press / press), 1/5.257) - 1.0) * (temp + 273.15)) / 0.0065;
}

/**
 * Compensates the accelerometer readings in the 3D vector acc expressed in the sensor frame for gravity
 * @param acc the accelerometer readings to compensate for gravity
 * @param q the quaternion orientation of the sensor board with respect to the world
 */
void FreeIMU::gravityCompensateAcc(double * acc, double * q) {
	double g[3];

	// get expected direction of gravity in the sensor frame
	g[0] = 2 * (q[1] * q[3] - q[0] * q[2]);
	g[1] = 2 * (q[0] * q[1] + q[2] * q[3]);
	g[2] = q[0] * q[0] - q[1] * q[1] - q[2] * q[2] + q[3] * q[3];

	// compensate accelerometer readings with the expected direction of gravity
	acc[0] = acc[0] - g[0];
	acc[1] = acc[1] - g[1];
	acc[2] = acc[2] - g[2];
}


/**
 * Returns the Euler angles in radians defined in the Aerospace sequence.
 * See Sebastian O.H. Madwick report "An efficient orientation filter for
 * inertial and intertial/magnetic sensor arrays" Chapter 2 Quaternion representation
 *
 * @param angles three doubles array which will be populated by the Euler angles in radians
 */
void FreeIMU::getEulerRad(double * angles,double ax,double ay,double az, double gx,double gy,double gz, double mx,double my,double mz) {

	double q[4]; // quaternion

	getQ(q,ax,ay,az,gx,gy,gz,mx,my,mz);

	angles[0] = atan2(2 * q[1] * q[2] - 2 * q[0] * q[3], 2 * q[0]*q[0] + 2 * q[1] * q[1] - 1);   // psi -
	angles[1] = -asin(2 * q[1] * q[3] + 2 * q[0] * q[2]);                                        // theta -
	angles[2] = atan2(2 * q[2] * q[3] - 2 * q[0] * q[1], 2 * q[0] * q[0] + 2 * q[3] * q[3] - 1); // phi -
}


/**
 * Returns the Euler angles in degrees defined with the Aerospace sequence.
 * See Sebastian O.H. Madwick report "An efficient orientation filter for
 * inertial and intertial/magnetic sensor arrays" Chapter 2 Quaternion representation
 *
 * @param angles three doubles array which will be populated by the Euler angles in degrees
 */
void FreeIMU::getEuler(double * angles,double ax,double ay,double az, double gx,double gy,double gz, double mx,double my,double mz) {
	getEulerRad(angles,ax,ay,az,gx,gy,gz,mx,my,mz);
	arr3_rad_to_deg(angles);
}


/**
 * Returns the yaw pitch and roll angles, respectively defined as the angles in radians between
 * the Earth North and the IMU X axis (yaw), the Earth ground plane and the IMU X axis (pitch)
 * and the Earth ground plane and the IMU Y axis.
 *
 * @note This is not an Euler representation: the rotations aren't consecutive rotations but only
 * angles from Earth and the IMU. For Euler representation Yaw, Pitch and Roll see FreeIMU::getEuler
 *
 * @param ypr three doubles array which will be populated by Yaw, Pitch and Roll angles in radians
 */
void FreeIMU::getYawPitchRollRad(double * ypr,double ax,double ay,double az, double gx2,double gy2,double gz2, double mx,double my,double mz) {

	double q[4]; // quaternion
	double gx, gy, gz; // estimated gravity direction

	getQ(q,ax,ay,az,gx2,gy2,gz2,mx,my,mz);

	gx = 2 * (q[1]*q[3] - q[0]*q[2]);
	gy = 2 * (q[0]*q[1] + q[2]*q[3]);
	gz = q[0]*q[0] - q[1]*q[1] - q[2]*q[2] + q[3]*q[3];

	ypr[0] = atan2(2 * q[1] * q[2] - 2 * q[0] * q[3], 2 * q[0]*q[0] + 2 * q[1] * q[1] - 1);
	ypr[1] = atan(gx / sqrt(gy*gy + gz*gz));
	ypr[2] = atan(gy / sqrt(gx*gx + gz*gz));
}


/**
 * Returns the yaw pitch and roll angles, respectively defined as the angles in degrees between
 * the Earth North and the IMU X axis (yaw), the Earth ground plane and the IMU X axis (pitch)
 * and the Earth ground plane and the IMU Y axis.
 *
 * @note This is not an Euler representation: the rotations aren't consecutive rotations but only
 * angles from Earth and the IMU. For Euler representation Yaw, Pitch and Roll see FreeIMU::getEuler
 *
 * @param ypr three doubles array which will be populated by Yaw, Pitch and Roll angles in degrees
 */
void FreeIMU::getYawPitchRoll(double * ypr,double ax,double ay,double az, double gx2,double gy2,double gz2, double mx,double my,double mz) {
	getYawPitchRollRad(ypr,ax,ay,az,gx2,gy2,gz2,mx,my,mz);
	arr3_rad_to_deg(ypr);
}


/**
 * Converts a 3 elements array arr of angles expressed in radians into degrees
 */
void FreeIMU::arr3_rad_to_deg(double * arr) {
	arr[0] *= 180/M_PI;
	arr[1] *= 180/M_PI;
	arr[2] *= 180/M_PI;
}



