#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

static int openadc(void){
  int fd;

        system("echo cape-bone-iio > /sys/devices/bone_capemgr.9/slots");
        //system("echo cape-bone-iio > /sys/devices/bone_capemgr.9/slots");

    

        return fd;
}


int main(int argc, char *argv[])
{
	openadc();
	return 0;
}
