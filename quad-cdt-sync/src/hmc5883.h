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
	double mag_x;
	double mag_y;
	double mag_z;

	int fd;


	hmc5883(int fd);
	void update();
	void set_addr();
	void configure();
	bool is_ready();
	double read16(int reg_msb,int reg_lsb);




};


#endif /* HMC5883_CPP_ */
