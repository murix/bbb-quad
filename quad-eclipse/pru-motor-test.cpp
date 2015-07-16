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
#define PWM_HZ              400
#define PWM_FLY_ARM      970000
#define PWM_CALIB_MIN   1000000
#define PWM_FLY_MIN     1070000
#define PWM_FLY_MAX     1900000
#define PWM_CALIB_MAX   1950000
#define PWM_STEP           1000

uint32_t dutyns[8];
uint32_t targetns[8];
float errorns[8];


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
	for(int i=0;i<8;i++){
           dutyns[i]=PWM_FLY_ARM;
           targetns[i]=PWM_FLY_ARM;
           
        }
	//
	set_all(myPWM,dutyns);

 
        

	while(1){
	   printf("PWM frequency %d Hz\r\n",PWM_HZ);

	   printf("cmd z: motor fly arm\r\n");
	   printf("cmd x: motor fly min\r\n");
	   printf("cmd c: motor fly max\r\n");

	   printf("cmd s: esc calibration min\r\n");
	   printf("cmd d: esc calibration max\r\n");

	   printf("cmd +: increase\r\n");
	   printf("cmd -: decrease\r\n");
	   printf("cmd a: quit\r\n");
	
	   char c = getch();
            
    
	   if (c =='z'){ for(int i=0;i<8;i++) targetns[i]=PWM_FLY_ARM;   }
	   if (c =='x'){ for(int i=0;i<8;i++) targetns[i]=PWM_FLY_MIN;   }
	   if (c =='c'){ for(int i=0;i<8;i++) targetns[i]=PWM_FLY_MAX;   }
              
	   if (c =='s'){ for(int i=0;i<8;i++) targetns[i]=PWM_CALIB_MIN; }
	   if (c =='d'){ for(int i=0;i<8;i++) targetns[i]=PWM_CALIB_MAX; }

	   if (c =='+'){ for(int i=0;i<8;i++) targetns[i]+=PWM_STEP; }
	   if (c =='-'){ for(int i=0;i<8;i++) targetns[i]-=PWM_STEP; }
	   if (c =='a') break;

	   //
	   if (c =='1') targetns[0]+=PWM_STEP;
	   if (c =='2') targetns[1]+=PWM_STEP;
	   if (c =='3') targetns[2]+=PWM_STEP;
	   if (c =='4') targetns[3]+=PWM_STEP;
	   if (c =='5') targetns[4]+=PWM_STEP;
	   if (c =='6') targetns[5]+=PWM_STEP;
	   if (c =='7') targetns[6]+=PWM_STEP;
	   if (c =='8') targetns[7]+=PWM_STEP;
	   //
	   if (c =='q') targetns[0]-=PWM_STEP;
	   if (c =='w') targetns[1]-=PWM_STEP;
	   if (c =='e') targetns[2]-=PWM_STEP;
	   if (c =='r') targetns[3]-=PWM_STEP;
	   if (c =='t') targetns[4]-=PWM_STEP;
	   if (c =='y') targetns[5]-=PWM_STEP;
	   if (c =='u') targetns[6]-=PWM_STEP;
	   if (c =='i') targetns[7]-=PWM_STEP;


             //           
	     for(int i=0;i<8;i++){
                errorns[i]=targetns[i]-dutyns[i];
             }

             float steps=100.0;
             int step_delay_us=1;

           //safety check
           for(int k=0;k<steps;k++){

	     for(int i=0;i<8;i++){
                //limit max pwm step to prevent overload fets, battery and power supply
                  dutyns[i]+=errorns[i]/steps;
                  usleep(step_delay_us);
                // minimum
                if(dutyns[i]<PWM_FLY_ARM  || targetns[i]<PWM_FLY_ARM){
                   dutyns[i]=PWM_FLY_ARM; 
                   targetns[i]=PWM_FLY_ARM; 
                }
                // maximum
                if(dutyns[i]>PWM_CALIB_MAX || targetns[i]>PWM_CALIB_MAX){
                   dutyns[i]=PWM_CALIB_MAX;
                   targetns[i]=PWM_CALIB_MAX;
                }
              }

           printf("dutyns=|");
	   for(int i=0;i<8;i++){
             printf("|%d",dutyns[i]);
           }
	   printf("|\r\n");
           printf("targetns=|");
	   for(int i=0;i<8;i++){
             printf("|%d",targetns[i]);
           }
	   printf("|\r\n");


	      set_all(myPWM,dutyns);
 
            }

	
	}

        //Leave motors ARMED
	for(int i=0;i<8;i++){
             dutyns[i]=PWM_FLY_ARM;
             targetns[i]=PWM_FLY_ARM;

        }
	//
	set_all(myPWM,dutyns);
}
 
