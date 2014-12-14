
#include <stdio.h>
#include <stdlib.h>

#include "bbb-adc.h"

int main(int argc,char** argv){
   adc_monitor adc;
   adc.init();
   while(1){
      adc.update();
      printf("vbat_raw=%f vbat=%f\r\n",adc.vbat_raw,adc.vbat);
   }

   return 0;
}