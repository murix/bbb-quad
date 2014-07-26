#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define ADDR_MPU6050 0x68
#define ADDR_HMC5883 0x1e
#define ADDR_MS5611  0x77

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

inline float to_degrees(float radians){
 return radians*(180.0/M_PI);
}

int hmc5883_configure(int file){
    uint8_t buffer[2];

    if (ioctl(file,I2C_SLAVE,ADDR_HMC5883) < 0) {
        printf("Failed to acquire bus access and/or talk to slave.\n");
    }

    //master enable
    buffer[0]=0x02;
    buffer[1]=0x00;
    write(file,buffer,2);
    //wait to be ready
      buffer[0]=0x09;
      write(file,buffer,1);
      read(file,buffer,1);
      if( (buffer[0] & 0x01) == 1){
         return 1;
      } else {
         return -1;
     }



}

float hmc5883_read16(int file,uint8_t reg_msb,uint8_t reg_lsb){

    uint8_t buffer[2];
    uint8_t msb;
    uint8_t lsb;
    uint16_t *p16;
    p16=(uint16_t*)buffer;

    buffer[0]=reg_msb;
    write(file,buffer,1);
    read(file,&msb,1);

    buffer[0]=reg_lsb;
    write(file,buffer,1);
    read(file,&lsb,1);
        
    buffer[0]=lsb;
    buffer[1]=msb;

    int16_t value = p16[0];

    float fvalue = value;

    return fvalue;
}


void ms5611_configure(int file,int over,double* sp,double* salt,double* st,double p0){
    
    uint8_t buffer[3];
    //uint16_t* p16;
    //p16=(uint16_t*)buffer;

    if (ioctl(file,I2C_SLAVE,ADDR_MS5611) < 0) {
        printf("Failed to acquire bus access and/or talk to slave.\n");
    }

#if 0
    //reset
    buffer[0]=0x1e;
    write(file,buffer,1);
    //wait wake up
    usleep(10000);
#endif

int wait_eeprom=1;

    buffer[0]=0xa2;
    write(file,buffer,1);
    usleep(wait_eeprom);
    read(file,buffer,2);
    uint16_t c1=buffer[0]*256+buffer[1];

    buffer[0]=0xa4;
    write(file,buffer,1);
    usleep(wait_eeprom);
    read(file,buffer,2);
    uint16_t c2=buffer[0]*256+buffer[1];

    buffer[0]=0xa6;
    write(file,buffer,1);
    usleep(wait_eeprom);
    read(file,buffer,2);
    uint16_t c3=buffer[0]*256+buffer[1];

    buffer[0]=0xa8;
    write(file,buffer,1);
    usleep(wait_eeprom);
    read(file,buffer,2);
    uint16_t c4=buffer[0]*256+buffer[1];

    buffer[0]=0xaa;
    write(file,buffer,1);
    usleep(wait_eeprom);
    read(file,buffer,2);
    uint16_t c5=buffer[0]*256+buffer[1];

    buffer[0]=0xac;
    write(file,buffer,1);
    usleep(wait_eeprom);
    read(file,buffer,2);
    uint16_t c6=buffer[0]*256+buffer[1];



    int sleep_us=0;
    switch(over){
     case 0: sleep_us=700; break;
     case 2: sleep_us=1200; break;
     case 4: sleep_us=2300; break;
     case 6: sleep_us=4600; break;
     case 8: sleep_us=9100; break;
     default: sleep_us=10000; break;
    }

    //
    buffer[0]=0x40+over;
    write(file,buffer,1);

    //
    usleep(sleep_us);

    //read ADC
    buffer[0]=0x0;
    write(file,buffer,1);
    read(file,buffer,3);
    uint32_t d1=buffer[0]*65536+buffer[1]*256+buffer[2];

    //
    buffer[0]=0x50+over;
    write(file,buffer,1);

    //
    usleep(sleep_us);

    //read ADC
    buffer[0]=0x0;
    write(file,buffer,1);
    read(file,buffer,3);
    uint32_t d2=buffer[0]*65536+buffer[1]*256+buffer[2];

int debug=0;

if(debug){
    c1=40127;
    c2=36924;
    c3=23317;
    c4=23282;
    c5=33464;
    c6=28312;
}
if(debug){
  d1=9085466;
  d2=8569150;
}

   int64_t dt =( (int64_t  )d2) - (( int64_t) c5*(1<<8));
   int64_t temp = 2000 + dt*c6/(1<<23);

   int64_t off= (( int64_t)  c2*(1<<16) ) + ( ( int64_t)  c4*dt/(1<<7) );

   int64_t sens= (( int64_t) c1*(1<<15) )+ (( int64_t)   c3*dt/(1<<8));
   int64_t p=(d1*sens/(1<<21)-off)/(1<<15);

if(debug){

 if(dt  !=2366) printf("dt error\r\n");
 if(temp!=2007) printf("temp error\r\n");
 if(off !=2420281617) printf("off error\r\n");
 if(sens!=1315097036) printf("sens error\r\n");
 if(p   !=100009) printf("p error\r\n");


}

   //z=RT/gM*loge(p0/0)
   //z=height difference between start height and measurement height
   //R=gas constant=8.31432
   //T=air temperature in measured kelvin
   //g=earth gravity=9.80665
   //M=molar mass of the gas=0.0289644
   //p0=pressure at start height
   //p=pressure at the measurement height
   double R=8.31432;
   double tn=temp/100.0;
   double T=tn+273.15;
   double g=9.80665;
   double M=0.0289644;
   double pn=p/100.0;
   double z=((R*T)/(g*M))*log(p0/pn);

   double alt = 44330.0*(1.0-pow(((p/100.0)/p0),1.0/5.255));

if(debug){
   printf("c1=%d ",c1);
   printf("c2=%d ",c2);
   printf("c3=%d ",c3);
   printf("c4=%d ",c4);
   printf("c5=%d ",c5);
   printf("c6=%d ",c6);
   printf("d1=%d ",d1);
   printf("d2=%d ",d2);
   printf("dt=%lld ",dt);
   printf("temp=%lld (%f) ",temp,tn);
   printf("off=%lld ",off);
   printf("sens=%lld ",sens);
   printf("p=%lld (%f) ",p,pn);
   printf("p0=%f ",p0);
   printf("alt=%f\r\n",alt);
}

   *sp=p/100.0;
   *salt=z;
   *st=temp/100.0;
}


void mpu6050_configure(int file){
    uint8_t buffer[2];

    if (ioctl(file,I2C_SLAVE,ADDR_MPU6050) < 0) {
        printf("Failed to acquire bus access and/or talk to slave.\n");
    }

    //master enable
    buffer[0]=0x6a;
    buffer[1]=0x00;
    write(file,buffer,2);

    //i2c bypass
    buffer[0]=0x37;
    buffer[1]=0x02;
    write(file,buffer,2);

    //wake up from sleep
    buffer[0]=0x6b;
    buffer[1]=0x00;
    write(file,buffer,2);

}

float mpu6050_read16(int file,uint8_t reg_msb,uint8_t reg_lsb){

    uint8_t buffer[2];
    uint8_t msb;
    uint8_t lsb;
    uint16_t *p16;
    p16=(uint16_t*)buffer;

    buffer[0]=reg_msb;
    write(file,buffer,1);
    read(file,&msb,1);

    buffer[0]=reg_lsb;
    write(file,buffer,1);
    read(file,&lsb,1);
        
    buffer[0]=lsb;
    buffer[1]=msb;

    int16_t value = p16[0];

    float fvalue = value;

    return fvalue;
}

int main(int argc,char** argv){

       printf("enable I2C-2 overlay\r\n");
       system("echo BB-I2C1 > /sys/devices/bone_capemgr.9/slots");
       printf("wait I2C-2 overlay to be ready\r\n");
       usleep(1000000);
    

    int file;
    if ((file = open("/dev/i2c-2",O_RDWR)) < 0) {
        printf("Failed to open the bus.");
        exit(1);
    }

    while(hmc5883_configure(file)!=1);
    float mx= hmc5883_read16(file,0x3,0x4)/1090.0;
    float my= hmc5883_read16(file,0x5,0x6)/1090.0;
    float mz= hmc5883_read16(file,0x7,0x8)/1090.0;

    double p,p0,alt,tt;
    ms5611_configure(file,8,&p,&alt,&tt,1013.25);
    p0=p;

   while(1){
    mpu6050_configure(file);
    float ax = mpu6050_read16(file,0x3b,0x3c)/16384.0;
    float ay = mpu6050_read16(file,0x3d,0x3e)/16384.0;
    float az = mpu6050_read16(file,0x3f,0x40)/16384.0;
    float tp = mpu6050_read16(file,0x41,0x42)/340.0+36.53;
    float gx = mpu6050_read16(file,0x43,0x44)/131.0;
    float gy = mpu6050_read16(file,0x45,0x46)/131.0;
    float gz = mpu6050_read16(file,0x47,0x48)/131.0;

    if(hmc5883_configure(file)==1){
     mx= hmc5883_read16(file,0x3,0x4)/1090.0;
     my= hmc5883_read16(file,0x5,0x6)/1090.0;
     mz= hmc5883_read16(file,0x7,0x8)/1090.0;
    }

    float z= to_degrees( -atan2f(my,mx) );
    float y= to_degrees( -atan2f(ax,az) );
    float x= to_degrees( atan2f(ay,az) );

    ms5611_configure(file,8,&p,&alt,&tt,p0);


    printf("mpu6050 acc=%+3.2f %+3.2f %+3.2f temp=%+3.2f gyro=%+3.2f %+3.2f %+3.2f | ",ax,ay,az,tp,gx,gy,gz);
    printf("hmc5883 mag=%+3.2f %+3.2f %+3.2f | ",mx,my,mz);
    printf("angles = %f %f %f | ",x,y,z);
    printf("ms5611 p=%f t=%f h=%f\r\n",p,tt,alt*100);

    //ms5611_configure(file,0);
    //ms5611_configure(file,2);
    //ms5611_configure(file,4);
    //ms5611_configure(file,6);


    //printf("-------------------------------------------------------------------------\r\n");

  }
  return 0;
}
