
#include "devmem_gpio.h"
#include "i2c_bitbang.h"


int main() {

	//pinMode2("P8_8", OUTPUT,7,DISABLED);
	//pinMode2("P8_9", OUTPUT,7,DISABLED);
	//pinMode2("P8_10", OUTPUT,7,DISABLED);
	//pinMode2("P9_19", OUTPUT,7,DISABLED);
	//pinMode2("P9_20", OUTPUT,7,DISABLED);

	while(1) {

		//digitalWrite(P8_8, 1);
		//digitalWrite(P8_8, 0);

		//digitalWrite(P8_9, 1);
		//digitalWrite(P8_9, 0);

		//digitalWrite(P8_10, 1);
		//digitalWrite(P8_10, 0);


		//digitalWrite(P9_19, 1);
		//digitalWrite(P9_19, 0);

		//digitalWrite(P9_20, 1);
		//digitalWrite(P9_20, 0);

		i2c_write_byte(1,0,0x68);
		i2c_write_byte(0,0,0x75);
		printf("resposta=%02x\r\n",i2c_read_byte(0,1));
	}	
	return 0;
}
