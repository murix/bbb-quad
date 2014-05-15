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

void set_all(PRUPWM* myPWM,uint32_t* dutyns){
	myPWM->setChannelValue(0,dutyns[0]);
	myPWM->setChannelValue(1,dutyns[1]);
	myPWM->setChannelValue(2,dutyns[2]);
	myPWM->setChannelValue(3,dutyns[3]);
	myPWM->setChannelValue(4,dutyns[4]);
	myPWM->setChannelValue(5,dutyns[5]);
	myPWM->setChannelValue(6,dutyns[6]);
	myPWM->setChannelValue(7,dutyns[7]);
}

//
#define PWM_HZ          50

// deve ser menos de que o minimo da calibracao
#define PWM_NORMAL_STOP   900000
#define PWM_NORMAL_MIN   1050000
#define PWM_NORMAL_MAX   1950000
#define PWM_NORMAL_STEP     1000

// calibration only
#define PWM_CALIB_MIN    1000000 
#define PWM_CALIB_MAX    2000000


uint32_t dutyns[8];

int main() {

       printf("enable PRU overlay\r\n");
       system("echo bone_pru0_out > /sys/devices/bone_capemgr.9/slots");
       printf("wait PRU overlay to be ready\r\n");
       usleep(1000000);


	// Initialise PRU PWM
	PRUPWM *myPWM = new PRUPWM(PWM_HZ);

	// Start the PRU
	myPWM->start();

	//
	for(int i=0;i<8;i++) 
           dutyns[i]=PWM_NORMAL_ARM;
	//
	set_all(myPWM,dutyns);

	while(1){
	   printf("PWM frequency %d Hz\r\n",PWM_HZ);
	   printf("cmd +: increase      duty by %8d ns, max=%d ns\r\n",PWM_NORMAL_STEP,PWM_NORMAL_MAX);
	   printf("cmd -: decrease      duty by %8d ns, min=%d ns\r\n",PWM_NORMAL_STEP,PWM_NORMAL_MIN);
	   printf("cmd d: esc calibration max -> duty=%8d ns\r\n",PWM_MAX);
	   printf("cmd c: esc calibration min -> duty=%8d ns\r\n",PWM_MIN);
	   printf("cmd s: motor arm -> duty=%8d ns\r\n",PWM_NORMAL_STOP);
	   printf("cmd x: quit\r\n");
	
	   char c = getch();
	   if (c =='+'){ for(int i=0;i<8;i++) dutyns[i]+=PWM_STEP; }
	   if (c =='-'){ for(int i=0;i<8;i++) dutyns[i]-=PWM_STEP; }
	   if (c =='d'){ for(int i=0;i<8;i++) dutyns[i]=PWM_MAX; }
	   if (c =='c'){ for(int i=0;i<8;i++) dutyns[i]=PWM_MIN; }
	   if (c ==' '){ for(int i=0;i<8;i++) dutyns[i]=PWM_ARM; }
	   if (c =='x') break;
	   //
	   if (c =='1') dutyns[0]+=PWM_STEP;//FL H
	   if (c =='2') dutyns[1]+=PWM_STEP;//FR L
	   if (c =='3') dutyns[2]+=PWM_STEP;
	   if (c =='4') dutyns[3]+=PWM_STEP;//RL I
	   if (c =='5') dutyns[4]+=PWM_STEP;//RR K
	   if (c =='6') dutyns[5]+=PWM_STEP;
	   if (c =='7') dutyns[6]+=PWM_STEP;
	   if (c =='8') dutyns[7]+=PWM_STEP;
	   //
	   if (c =='q') dutyns[0]-=PWM_STEP;
	   if (c =='w') dutyns[1]-=PWM_STEP;
	   if (c =='e') dutyns[2]-=PWM_STEP;
	   if (c =='r') dutyns[3]-=PWM_STEP;
	   if (c =='t') dutyns[4]-=PWM_STEP;
	   if (c =='y') dutyns[5]-=PWM_STEP;
	   if (c =='u') dutyns[6]-=PWM_STEP;
	   if (c =='i') dutyns[7]-=PWM_STEP;



	   for(int i=0;i<8;i++) printf("|%d",dutyns[i]);
	   printf("|\r\n");

	   set_all(myPWM,dutyns);
	
	}

    //Leave motors ARMED
	for(int i=0;i<8;i++) dutyns[i]=PWM_ARM;
	//
	set_all(myPWM,dutyns);
}
 
