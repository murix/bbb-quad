
#include "memGPIO.hpp"


int main () {

  easyBlack::memGPIO myExample;

  //easyBlack::memGPIO::gpioPin usr0 = myExample.getPin ("USR0");
 // easyBlack::memGPIO::gpioPin p8_7 = myExample.getPin ("P8_7");
  //easyBlack::memGPIO::gpioPin p8_8 = myExample.getPin ("P8_8");

  //myExample.pinMode (p8_7, myExample.OUTPUT,7,myExample.DISABLED,myExample.FAST);
 // myExample.pinMode (p8_8, myExample.OUTPUT,7,myExample.DISABLED,myExample.FAST);


  printf("build %s %s\r\n",__DATE__,__TIME__);
  for (;;) {
    myExample.digitalWrite ("P8_7", "HIGH");
    usleep(100);
    myExample.digitalWrite ("P8_7", "LOW");
    usleep(100);
    myExample.digitalWrite ("P8_8", "HIGH");
    usleep(100);
    myExample.digitalWrite ("P8_8", "LOW");
    usleep(100);
  }

  return 0;
}
