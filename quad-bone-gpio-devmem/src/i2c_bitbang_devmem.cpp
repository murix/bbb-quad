
#include "i2c_bitbang.h"
#include "devmem_gpio.h"
#include <unistd.h>

void I2C_delay(void){
	usleep(5);
}



int scl_state=2;
int sda_state=2;

// Set SCL as input and return current level of line, 0 or 1
bool read_SCL(void){
	if(scl_state!=INPUT){
		pinMode2("P8_10", INPUT,7,DISABLED);
		scl_state=INPUT;
	}
	return digitalRead(P8_10);
}
// Set SDA as input and return current level of line, 0 or 1
bool read_SDA(void){
	if(sda_state!=INPUT){
		pinMode2("P8_9", INPUT,7,DISABLED);
		sda_state=INPUT;
	}
	return digitalRead(P8_9);
}

// Actively drive SCL signal low
void clear_SCL(void){
	if(scl_state!=OUTPUT){
		pinMode2("P8_10", OUTPUT,7,DISABLED);
		scl_state=OUTPUT;
	}
	digitalWrite(P8_10, 0);

}
// Actively drive SDA signal low
void clear_SDA(void){
	if(sda_state!=OUTPUT){
		pinMode2("P8_9", OUTPUT,7,DISABLED);
		sda_state=OUTPUT;
	}
	digitalWrite(P8_9, 0);
}


void arbitration_lost(void){
   printf("arbitration_lost\r\n");
}
