/*
 * hmc5883.cpp
 *
 *  Created on: 10/08/2014
 *      Author: root
 */

#ifndef HMC5883_CPP_
#define HMC5883_CPP_


class hmc5883 {
public:
	double mag[3];
	double magn;
	int fd;
	double heading;

	hmc5883(int fd);
	void update();
	void set_addr();
	void configure();
	bool is_ready();
	double read16(int reg_msb,int reg_lsb);

	void calculate_heading();

	double to_degrees(double radians);
	double to_radian(double degree);


};


#endif /* HMC5883_CPP_ */
