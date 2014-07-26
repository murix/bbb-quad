#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define ADDR_MPU6050 0x68
#define ADDR_HMC5883 0x1e
#define ADDR_MS5611  0x77

void hmc5883_configure(int file){
    if (ioctl(file,I2C_SLAVE,ADDR_HMC5883) < 0) {
        printf("Failed to acquire bus access and/or talk to slave.\n");
    }

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
while(1){
    mpu6050_configure(file);
    float ax = mpu6050_read16(file,0x3b,0x3c)/16384.0;
    float ay = mpu6050_read16(file,0x3d,0x3e)/16384.0;
    float az = mpu6050_read16(file,0x3f,0x40)/16384.0;
    float tp = mpu6050_read16(file,0x41,0x42)/340.0+36.53;
    float gx = mpu6050_read16(file,0x43,0x44)/131.0;
    float gy = mpu6050_read16(file,0x45,0x46)/131.0;
    float gz = mpu6050_read16(file,0x47,0x48)/131.0;
    printf("%+3.2f %+3.2f %+3.2f %+3.2f %+3.2f %+3.2f %+3.2f\r\n",ax,ay,az,tp,gx,gy,gz);

    hmc5883_configure(file);

    ms5611_configure(file);

 }
  return 0;
}
