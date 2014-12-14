#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>

#include "bbb-adc.h"

void adc_monitor::init(void){
        system("echo cape-bone-iio > /sys/devices/bone_capemgr.9/slots");
        //system("echo cape-bone-iio > /sys/devices/bone_capemgr.9/slots");
}

void adc_monitor::update(void){

        FILE* fd = fopen("/sys/devices/ocp.3/helper.12/AIN0","r");

        if(!fd){
          return ;
        }

        fseek(fd,0,SEEK_SET);

        int ret = fscanf(fd,"%f",&vbat_raw);

        vbat=((vbat_raw/1000.0)*(5.0/0.265));

        fclose(fd);

}
	
