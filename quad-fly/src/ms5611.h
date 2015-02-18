/*
 * ms5611.h
 *
 *  Created on: 10/08/2014
 *      Author: root
 */

#ifndef QUAD_FLY_SRC_MS5611_H_
#define QUAD_FLY_SRC_MS5611_H_

#include "i2c_linux.h"

enum ms5611_states {
	START_D1,
	WAIT_D1,
	READ_D1,

	START_D2,
	WAIT_D2,
	READ_D2,

	COMPUTE_ALL
};

class ms5611 {
public:
	ms5611();
	void configure(i2c_linux i2c);
	void update(i2c_linux i2c);
	double P; // compensated pressure value
	double T; // compensated temperature value
	double P0;
	double H;

private:
	double altimeter(double p0,double p,double t);
	void update_start();
	double read_started;
	ms5611_states state;
	void i2c_start(i2c_linux i2c);
	void i2c_send(i2c_linux i2c,char cmd);
	void i2c_recv(i2c_linux i2c,unsigned char* buffer,int buffer_len);
	void cmd_reset(i2c_linux i2c);
	unsigned long cmd_adc(i2c_linux i2c,char cmd);
	unsigned int cmd_prom(i2c_linux i2c,char coef_num);
	unsigned char crc4(unsigned int n_prom[]);

	unsigned int C[8]; // calibration coefficients
	unsigned long D1; // ADC value of the pressure conversion
	unsigned long D2; // ADC value of the temperature conversion
	double dT; // difference between actual and measured temperature
	double OFF; // offset at actual temperature
	double SENS; // sensitivity at actual temperature

};



#endif /* QUAD_FLY_SRC_MS5611_H_ */
