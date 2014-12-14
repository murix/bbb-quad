#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

void openadc(void){
  

        system("echo cape-bone-iio > /sys/devices/bone_capemgr.9/slots");
        //system("echo cape-bone-iio > /sys/devices/bone_capemgr.9/slots");



while(1){
        FILE* fd = fopen("/sys/devices/ocp.3/helper.12/AIN0","r");
        if(!fd){
          return ;
        }
        fseek(fd,0,SEEK_SET);
        float val;
        int ret = fscanf(fd,"%f",&val);
        printf("%f\r\n", ((val/1000.0)*(5.0/0.265)) );
	usleep(10*1000);
        fclose(fd);

}
	

    

        
}


int main(int argc, char *argv[])
{
	openadc();
	return 0;
}
