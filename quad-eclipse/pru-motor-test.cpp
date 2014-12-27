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

#include "pru-pwm-cpp.h"
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

#if 0
       //
       printf("enable PRU overlay\r\n");
       system("echo bone_pru0_out > /sys/devices/bone_capemgr.9/slots");
       printf("wait PRU overlay to be ready...\r\n");
       usleep(1000*1000);
#endif

	// Initialise PRU PWM
	PRUPWM *myPWM = new PRUPWM(PWM_HZ);
	// Start the PRU
	myPWM->start();
        printf("PRU initialized\r\n");


        for(int ch=0;ch<8;ch++){
	   myPWM->setChannelValue(ch,PWM_FLY_ARM);
        }
        printf("PRU Motors frequency=%d Hz ch=all @ %d ns\r\n",PWM_HZ,PWM_FLY_ARM);


	

     int speeds[8];
     for(int ch=0;ch<8;ch++){
       speeds[ch]=PWM_FLY_ARM;
     }

     for(;;){
        for(int ch=0;ch<8;ch++){
           
           bool need_change=false;
           //
           if(speeds[ch] < pdata->dutyns[ch]){
              speeds[ch] += PWM_STEP;
              if(speeds[ch]>PWM_FLY_MAX) speeds[ch]=PWM_FLY_MAX;
              need_change=true;
           }
           if(speeds[ch] > pdata->dutyns[ch]){
              speeds[ch] -= PWM_STEP;
              if(speeds[ch]<PWM_FLY_ARM) speeds[ch]=PWM_FLY_ARM;
	      need_change=true;
           }

           if(need_change){
              myPWM->setChannelValue(ch,speeds[ch]);
              //printf("PRU Motor frequency=%d Hz ch=%d @ %d ns\r\n",PWM_HZ,ch,speeds[ch]);
           }
        }
        
        // wait 2,5ms for 400hz pwm complete duty
        usleep(2500);
        //command for terminate this thread
        if(pdata->cmd=='a'){
          break;
        }
     }


        // apply ARM
        for(int ch=0;ch<8;ch++){
	   myPWM->setChannelValue(ch,PWM_FLY_ARM);
        }
        printf("PRU Motors frequency=%d Hz ch=all @ %d ns\r\n",PWM_HZ,PWM_FLY_ARM);
        printf("all motors has stoped\r\n");



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

	   if (motors.cmd =='1'){ motors.dutyns[0]+=PWM_STEP; } //
	   if (motors.cmd =='2'){ motors.dutyns[1]+=PWM_STEP; } //
	   if (motors.cmd =='3'){ motors.dutyns[2]+=PWM_STEP; }
	   if (motors.cmd =='4'){ motors.dutyns[3]+=PWM_STEP; }
	   if (motors.cmd =='5'){ motors.dutyns[4]+=PWM_STEP; }
	   if (motors.cmd =='6'){ motors.dutyns[5]+=PWM_STEP; } //
	   if (motors.cmd =='7'){ motors.dutyns[6]+=PWM_STEP; }
	   if (motors.cmd =='8'){ motors.dutyns[7]+=PWM_STEP; } //

	   if (motors.cmd =='q'){ motors.dutyns[0]-=PWM_STEP; }
	   if (motors.cmd =='w'){ motors.dutyns[1]-=PWM_STEP; }
	   if (motors.cmd =='e'){ motors.dutyns[2]-=PWM_STEP; }
	   if (motors.cmd =='r'){ motors.dutyns[3]-=PWM_STEP; }
	   if (motors.cmd =='t'){ motors.dutyns[4]-=PWM_STEP; }
	   if (motors.cmd =='y'){ motors.dutyns[5]-=PWM_STEP; }
	   if (motors.cmd =='u'){ motors.dutyns[6]-=PWM_STEP; }
	   if (motors.cmd =='i'){ motors.dutyns[7]-=PWM_STEP; }
    
	   if (motors.cmd =='z'){ for(int i=0;i<8;i++) motors.dutyns[i]=PWM_FLY_ARM;   }
	   if (motors.cmd =='x'){ for(int i=0;i<8;i++) motors.dutyns[i]=PWM_FLY_MIN;   }
	   if (motors.cmd =='c'){ for(int i=0;i<8;i++) motors.dutyns[i]=PWM_FLY_MAX;   }
              
           if (motors.cmd =='a'){ break; }
	   if (motors.cmd =='s'){ for(int i=0;i<8;i++) motors.dutyns[i]=PWM_CALIB_MIN; }
	   if (motors.cmd =='d'){ for(int i=0;i<8;i++) motors.dutyns[i]=PWM_CALIB_MAX; }

	   if (motors.cmd =='+'){ for(int i=0;i<8;i++) motors.dutyns[i]+=PWM_STEP; }
	   if (motors.cmd =='-'){ for(int i=0;i<8;i++) motors.dutyns[i]-=PWM_STEP; }
	   

	}
        
        printf("wait motorserver terminate...\r\n");
        pthread_join(th1, NULL);
        printf("motorserver terminated\r\n");
        

}
 
