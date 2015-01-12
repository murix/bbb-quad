/*
 * freeimu_linux.h
 *
 *  Created on: 04/01/2015
 *      Author: murix
 */

#ifndef FREEIMU_LINUX_H_
#define FREEIMU_LINUX_H_


class FreeIMU
{
private:
	// quaternion of sensor frame relative to auxiliary frame
	// integration magnetic field
	double integralFBx,  integralFBy, integralFBz;
	// sample period expressed in seconds
	double lastUpdate, now;
	// half the sample period expressed in seconds
	double sampleFreq;


	double gravity[3];
    double linear_acc[3];
    double linear_speed[3];
    double linear_position[3];

public:
	//
	FreeIMU();
	double q0, q1, q2, q3;



	// accelerometer - any unit
	// gyroscope - must be in rad/s
	// Magnetometer - any unit
	void getQ(double * q,double ax,double ay,double az, double gx,double gy,double gz, double mx,double my,double mz);
	void getEulerRad(double * angles,double ax,double ay,double az, double gx,double gy,double gz, double mx,double my,double mz);
	void getEulerDegree(double * angles,double ax,double ay,double az, double gx,double gy,double gz, double mx,double my,double mz);
	void getYawPitchRollDegree(double * ypr,double ax,double ay,double az, double gx2,double gy2,double gz2, double mx,double my,double mz);
	void getYawPitchRollRad(double * ypr,double ax,double ay,double az, double gx2,double gy2,double gz2, double mx,double my,double mz);

	//
	void AHRSupdate(double gx, double gy, double gz, double ax, double ay, double az, double mx, double my, double mz);

	//
	double getBaroAlt(double sea_press,double temp,double press);

	//
	void gravityCompensateAcc(double * acc, double * q);
	//
	void arr3_rad_to_deg(double * arr);

	void quaternation_to_euler_rad(double* q,double* angles);
};




#endif /* FREEIMU_LINUX_H_ */
