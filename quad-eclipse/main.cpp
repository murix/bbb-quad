/* Copyright (c) 2014 Murilo Pontes
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "pruPWM.h"
#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

int getch()
{
  int            ch;
  struct termios old;
  struct termios tmp;

  if (tcgetattr(STDIN_FILENO, &old))
  {
    return -1;
  }

  memcpy(&tmp, &old, sizeof(old));

  tmp.c_lflag &= ~ICANON & ~ECHO;

  if (tcsetattr(STDIN_FILENO, TCSANOW, (const struct termios*) &tmp))
  {
    return -1;
  }

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, (const struct termios*) &old);

  return ch;
}

void set_all(PRUPWM* myPWM,uint32_t dutyns){
	myPWM->setChannelValue(0,dutyns);
	myPWM->setChannelValue(1,dutyns);
	myPWM->setChannelValue(2,dutyns);
	myPWM->setChannelValue(3,dutyns);
	myPWM->setChannelValue(4,dutyns);
	myPWM->setChannelValue(5,dutyns);
	myPWM->setChannelValue(6,dutyns);
	myPWM->setChannelValue(7,dutyns);
}

#define PWM_HZ          50
#define PWM_ARM     900000
#define PWM_MIN    1000000 
#define PWM_MAX    2000000
#define PWM_STEP      1000

int main() {

       printf("enable PRU overlay\r\n");
       system("echo bone_pru0_out > /sys/devices/bone_capemgr.9/slots");
       printf("wait PRU overlay to be ready\r\n");
       usleep(1000000);


	// Initialise PRU PWM
	PRUPWM *myPWM = new PRUPWM(PWM_HZ);

	// Start the PRU
	myPWM->start();

        // ARM Motors
	uint32_t dutyns=PWM_ARM;
	set_all(myPWM,dutyns);
	while(1){
	   printf("PWM frequency %d Hz\r\n",PWM_HZ);
	   printf("cmd +: increase      duty by %8d ns\r\n",PWM_STEP);
	   printf("cmd -: decrease      duty by %8d ns\r\n",PWM_STEP);
	   printf("cmd d: motor max     -> duty=%8d ns\r\n",PWM_MAX);
	   printf("cmd c: motor min     -> duty=%8d ns\r\n",PWM_MIN);
	   printf("cmd space: motor arm -> duty=%8d ns\r\n",PWM_ARM);
	   printf("cmd q: quit\r\n");
	
	   char c = getch();
	   if (c =='+') dutyns+=PWM_STEP;
	   if (c =='-') dutyns-=PWM_STEP;
	   if (c =='d') dutyns=PWM_MAX;
	   if (c =='c') dutyns=PWM_MIN;
	   if (c ==' ') dutyns=PWM_ARM;
	   if (c =='q') break;
	   printf("dutyns=%d\r\n",dutyns);
	   set_all(myPWM,dutyns);
	
	}

        //Leave motors ARMED
	dutyns=PWM_ARM;
	set_all(myPWM,dutyns);
}
 
