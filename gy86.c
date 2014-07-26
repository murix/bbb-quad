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


void ms5611_configure(int file){
    if (ioctl(file,I2C_SLAVE,ADDR_MS5611) < 0) {
        printf("Failed to acquire bus access and/or talk to slave.\n");
    }

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
    int file;
    if ((file = open("/dev/i2c-2",O_RDWR)) < 0) {
        printf("Failed to open the bus.");
        exit(1);
    }

    while(hmc5883_configure(file)!=1);
    float mx= hmc5883_read16(file,0x3,0x4)/1090.0;
    float my= hmc5883_read16(file,0x5,0x6)/1090.0;
    float mz= hmc5883_read16(file,0x7,0x8)/1090.0;

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

    ms5611_configure(file);

    printf("mpu6050 acc=%+3.2f %+3.2f %+3.2f temp=%+3.2f gyro=%+3.2f %+3.2f %+3.2f \r\n",ax,ay,az,tp,gx,gy,gz);
    printf("hmc5883 mag=%+3.2f %+3.2f %+3.2f \r\n",mx,my,mz);
    printf("ms5611 p=  \r\n");
    printf("angles = %f %f %f\r\n",x,y,z);
    printf("-------------------------------------------------------------------------\r\n");

  }
  return 0;
}
