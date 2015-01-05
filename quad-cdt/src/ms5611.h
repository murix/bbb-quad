/*
 * ms5611.h
 *
 *  Created on: 10/08/2014
 *      Author: root
 */

#ifndef MS5611_H_
#define MS5611_H_

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
	double read_started;
	ms5611_states state;

	int fd;
	ms5611(int fd);
	void i2c_start();
	void i2c_send(char cmd);
	void i2c_recv(unsigned char* buffer,int buffer_len);
	void cmd_reset(void);
	unsigned long cmd_adc(char cmd);
	unsigned int cmd_prom(char coef_num);
	unsigned char crc4(unsigned int n_prom[]);
	void init();
	void update();
	void update_start();

	double altimeter(double p0,double p,double t);

	unsigned int C[8]; // calibration coefficients

	unsigned long D1; // ADC value of the pressure conversion
	unsigned long D2; // ADC value of the temperature conversion
	double P; // compensated pressure value
	double T; // compensated temperature value
	double dT; // difference between actual and measured temperature
	double OFF; // offset at actual temperature
	double SENS; // sensitivity at actual temperature
};



#endif /* MS5611_H_ */
