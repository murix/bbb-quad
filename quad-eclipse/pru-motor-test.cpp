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

#include <pthread.h>

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



//
#define PWM_HZ              400
#define PWM_FLY_ARM      970000
#define PWM_CALIB_MIN   1000000
#define PWM_FLY_MIN     1070000
#define PWM_FLY_MAX     1900000
#define PWM_CALIB_MAX   1950000
#define PWM_STEP           1000
#define PWM_CHANGE      2500000

typedef struct {
 char cmd;
 uint32_t dutyns[8];

} motor_t;


void *motorserver(void *arg){
     motor_t* pdata=(motor_t*) arg;

       //
       printf("enable PRU overlay\r\n");
       system("echo bone_pru0_out > /sys/devices/bone_capemgr.9/slots");
       printf("wait PRU overlay to be ready\r\n");
       usleep(1000000);

	// Initialise PRU PWM
	PRUPWM *myPWM = new PRUPWM(PWM_HZ);
	// Start the PRU
	myPWM->start();

        for(int ch=0;ch<8;ch++){
	   myPWM->setChannelValue(ch,PWM_FLY_ARM);
        }

	

     int cache[8];
     for(;;){
        for(int ch=0;ch<8;ch++){
           //cache outdated
           if(cache[ch]!=pdata->dutyns[ch]){
              //update cache
              cache[ch]=pdata->dutyns[ch];
              //safety cache
              if(cache[ch]<PWM_FLY_ARM)   cache[ch]=PWM_FLY_ARM;
              if(cache[ch]>PWM_CALIB_MAX) cache[ch]=PWM_CALIB_MAX;
              //
              myPWM->setChannelValue(ch,cache[ch]);
           }
        }
        
        // wait 2,5ms for 400hz pwm complete duty
        usleep(2500);
        //
        if(pdata->cmd=='a'){
          break;
        }
     }

        for(int ch=0;ch<8;ch++){
	   myPWM->setChannelValue(ch,PWM_FLY_ARM);
        }


}


int main() {

        motor_t motors;
        bzero(&motors,sizeof(motor_t));
	pthread_t th1;
	pthread_create(&th1, 0, motorserver, &motors);
      

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
	
	   motors.cmd = getch();
    
	   if (motors.cmd =='z'){ for(int i=0;i<8;i++) motors.dutyns[i]=PWM_FLY_ARM;   }
	   if (motors.cmd =='x'){ for(int i=0;i<8;i++) motors.dutyns[i]=PWM_FLY_MIN;   }
	   if (motors.cmd =='c'){ for(int i=0;i<8;i++) motors.dutyns[i]=PWM_FLY_MAX;   }
              
	   if (motors.cmd =='s'){ for(int i=0;i<8;i++) motors.dutyns[i]=PWM_CALIB_MIN; }
	   if (motors.cmd =='d'){ for(int i=0;i<8;i++) motors.dutyns[i]=PWM_CALIB_MAX; }

	   if (motors.cmd =='+'){ for(int i=0;i<8;i++) motors.dutyns[i]+=PWM_STEP; }
	   if (motors.cmd =='-'){ for(int i=0;i<8;i++) motors.dutyns[i]-=PWM_STEP; }
	   if (motors.cmd =='a') break;
	}
        
        printf("wait motorserver terminate...\r\n");
        pthread_join(th1, NULL);
        printf("motorserver terminated\r\n");
        

}
 
