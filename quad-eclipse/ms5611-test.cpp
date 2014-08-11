/*
 * baro.cpp
 *
 *  Created on: 26/07/2014
 *      Author: root
 */


//printf
#include <stdio.h>

#include "bbb_i2c.h"
#include "ms5611.h"


int main (void)
{

	bbb_i2c i2c;
	i2c.open();

	ms5611 baro(i2c.fd);
	while(1){
		baro.update();
		printf("baro t=%f p0=%f p=%f altimeter=%f\r\n",baro.T,baro.P0,baro.P,baro.H);
	}

	return 0;
}


