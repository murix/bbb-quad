/*
 * hmc5883.cpp
 *
 *  Created on: 10/08/2014
 *      Author: root
 */

#ifndef HMC5883_CPP_
#define HMC5883_CPP_

#include "i2c_linux.h"

class hmc5883 {
public:
	double mag_x;
	double mag_y;
	double mag_z;

	hmc5883();
	void configure(i2c_linux i2c);
	void update(i2c_linux i2c);

private:
	void set_addr(i2c_linux i2c);
	bool is_ready(i2c_linux i2c);
	double read16(i2c_linux i2c,int reg_msb,int reg_lsb);


};


#endif /* HMC5883_CPP_ */
