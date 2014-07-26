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

class sensor_hmc5883 {
public:
	float mx;
	float my;
	float mz;
	sensor_hmc5883(int fd){
		this->fd=fd;
		hmc5883_configure();
		mx=0;
		my=0;
		mz=0;
	}
	void update(){
		if(is_ready()){
			mx= hmc5883_read16(0x3,0x4)/1090.0;
			my= hmc5883_read16(0x5,0x6)/1090.0;
			mz= hmc5883_read16(0x7,0x8)/1090.0;
		}
	}
private:
	int fd;
	void set_addr(){
		if (ioctl(fd,I2C_SLAVE,ADDR_HMC5883) < 0) {
			printf("Failed to acquire bus access and/or talk to slave.\n");
		}
	}
	void hmc5883_configure(){
		set_addr();
		//master enable
		uint8_t buffer[2];
		buffer[0]=0x02;
		buffer[1]=0x00;
		write(fd,buffer,2);
	}

	bool is_ready(){
		set_addr();

		//wait to be ready
		uint8_t buffer[2];
		buffer[0]=0x09;
		write(fd,buffer,1);
		read(fd,buffer,1);
		if( (buffer[0] & 0x01) == 1){
			return true;
		} else {
			return false;
		}
	}

	float hmc5883_read16(uint8_t reg_msb,uint8_t reg_lsb){
		set_addr();

		uint8_t buffer[2];
		uint8_t msb;
		uint8_t lsb;
		uint16_t *p16;
		p16=(uint16_t*)buffer;
		buffer[0]=reg_msb;
		write(fd,buffer,1);
		read(fd,&msb,1);
		buffer[0]=reg_lsb;
		write(fd,buffer,1);
		read(fd,&lsb,1);
		buffer[0]=lsb;
		buffer[1]=msb;
		int16_t value = p16[0];
		float fvalue = value;
		return fvalue;
	}


};

class sensor_ms5611 {
public:
	sensor_ms5611(int fd){
		this->fd=fd;
		reset();
		read_calib();
		update();
	}
	double P,T;
	void update(){
		read_adc(8);
		calculate();
	}
	double altimeter(double p0,double p,double t){
		//R=gas constant=8.31432
		//tk=air temperature in measured kelvin
		//g=earth gravity=9.80665
		//M=molar mass of the gas=0.0289644
		double tk= t+273.15;
		double R=8.31432;
		//TODO: get gravity from accelerometer
		double g=9.80665;
		double M=0.0289644;
		return ((R*tk)/(g*M))*log(p0/p);
	}


private:
	int fd;
	void set_addr(){
		if (ioctl(fd,I2C_SLAVE,ADDR_MS5611) < 0) {
			printf("Failed to acquire bus access and/or talk to slave.\n");
		}
	}
	void reset(){
		set_addr();
		//reset
		uint8_t buffer[3];
		buffer[0]=0x1e;
		write(fd,buffer,1);
		//wait 3ms to be ready
		usleep(3*1000);
	}

	uint16_t c[8];
	void read_calib(){
		set_addr();

		uint8_t buffer[3];
		int wait_eeprom=1;

		buffer[0]=0xa2;
		write(fd,buffer,1);
		usleep(wait_eeprom);
		read(fd,buffer,2);
		c[1]=buffer[0]*256+buffer[1];

		buffer[0]=0xa4;
		write(fd,buffer,1);
		usleep(wait_eeprom);
		read(fd,buffer,2);
		c[2]=buffer[0]*256+buffer[1];

		buffer[0]=0xa6;
		write(fd,buffer,1);
		usleep(wait_eeprom);
		read(fd,buffer,2);
		c[3]=buffer[0]*256+buffer[1];

		buffer[0]=0xa8;
		write(fd,buffer,1);
		usleep(wait_eeprom);
		read(fd,buffer,2);
		c[4]=buffer[0]*256+buffer[1];

		buffer[0]=0xaa;
		write(fd,buffer,1);
		usleep(wait_eeprom);
		read(fd,buffer,2);
		c[5]=buffer[0]*256+buffer[1];

		buffer[0]=0xac;
		write(fd,buffer,1);
		usleep(wait_eeprom);
		read(fd,buffer,2);
		c[6]=buffer[0]*256+buffer[1];

	}

	uint32_t d[3];
	void read_adc(int over){
		set_addr();

		uint8_t buffer[3];

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
		write(fd,buffer,1);

		//
		usleep(sleep_us);

		//read ADC
		buffer[0]=0x0;
		write(fd,buffer,1);
		read(fd,buffer,3);
		d[1]=buffer[0]*65536+buffer[1]*256+buffer[2];

		//
		buffer[0]=0x50+over;
		write(fd,buffer,1);

		//
		usleep(sleep_us);

		//read ADC
		buffer[0]=0x0;
		write(fd,buffer,1);
		read(fd,buffer,3);
		d[2]=buffer[0]*65536+buffer[1]*256+buffer[2];

	}

	void debug(){

		c[1]=40127;
		c[2]=36924;
		c[3]=23317;
		c[4]=23282;
		c[5]=33464;
		c[6]=28312;
		d[1]=9085466;
		d[2]=8569150;

		calculate();

		printf("c1=%d\r\n",c[1]);
		printf("c2=%d\r\n",c[2]);
		printf("c3=%d\r\n",c[3]);
		printf("c4=%d\r\n",c[4]);
		printf("c5=%d\r\n",c[5]);
		printf("c6=%d\r\n",c[6]);
		printf("d1=%d\r\n",d[1]);
		printf("d2=%d\r\n",d[2]);
		printf("dt  =%lld\r\n",dt);
		printf("temp=%lld\r\n",temp);
		printf("off =%lld\r\n",off);
		printf("sens=%lld\r\n",sens);
		printf("p   =%lld\r\n",p);

		if(dt  !=2366) printf("dt error\r\n");
		if(temp!=2007) printf("temp error\r\n");
		if(off !=atol("2420281617")) printf("off error\r\n");
		if(sens!=atol("1315097036")) printf("sens error\r\n");
		if(p   !=100009) printf("p error\r\n");
	}

	int64_t dt,temp,off,sens,p;
	void calculate(){
		dt =( (int64_t  )d[2]) - (( int64_t) c[5]*(1<<8));
		temp = 2000 + dt*c[6]/(1<<23);
		off= (( int64_t)  c[2]*(1<<16) ) + ( ( int64_t)  c[4]*dt/(1<<7) );
		sens= (( int64_t) c[1]*(1<<15) )+ (( int64_t)   c[3]*dt/(1<<8));
		p=(d[1]*sens/(1<<21)-off)/(1<<15);
		//
		T=temp/100.0;
		P=p/100.0;
	}

};

class sensor_mpu6050 {
public:
	sensor_mpu6050(int fd){
		this->fd=fd;
		init();
		update();
	}

	float ax,ay,az,tp,gx,gy,gz;
	void update(){
		ax = mpu6050_read16(0x3b,0x3c)/16384.0;
		ay = mpu6050_read16(0x3d,0x3e)/16384.0;
		az = mpu6050_read16(0x3f,0x40)/16384.0;
		tp = mpu6050_read16(0x41,0x42)/340.0+36.53;
		gx = mpu6050_read16(0x43,0x44)/131.0;
		gy = mpu6050_read16(0x45,0x46)/131.0;
		gz = mpu6050_read16(0x47,0x48)/131.0;
	}

private:
	int fd;
	void set_addr(){
		if (ioctl(fd,I2C_SLAVE,ADDR_MPU6050) < 0) {
			printf("Failed to acquire bus access and/or talk to slave.\n");
		}
	}
	void init(){
		set_addr();

		uint8_t buffer[2];
		//master enable
		buffer[0]=0x6a;
		buffer[1]=0x00;
		write(fd,buffer,2);
		//i2c bypass
		buffer[0]=0x37;
		buffer[1]=0x02;
		write(fd,buffer,2);
		//wake up from sleep
		buffer[0]=0x6b;
		buffer[1]=0x00;
		write(fd,buffer,2);

	}

	float mpu6050_read16(uint8_t reg_msb,uint8_t reg_lsb){
		set_addr();

		uint8_t buffer[2];
		uint8_t msb;
		uint8_t lsb;
		uint16_t *p16;
		p16=(uint16_t*)buffer;

		buffer[0]=reg_msb;
		write(fd,buffer,1);
		read(fd,&msb,1);

		buffer[0]=reg_lsb;
		write(fd,buffer,1);
		read(fd,&lsb,1);

		buffer[0]=lsb;
		buffer[1]=msb;

		int16_t value = p16[0];

		float fvalue = value;

		return fvalue;
	}

};



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

	sensor_mpu6050 acc_gyro(file);
	sensor_hmc5883 mag(file);
	sensor_ms5611 baro(file);

	double p0=baro.P;

	while(1){
		acc_gyro.update();
		mag.update();
		baro.update();

		float x= to_degrees( atan2f(acc_gyro.ay,acc_gyro.az) );
		float y= to_degrees( -atan2f(acc_gyro.ax,acc_gyro.az) );
		float z= to_degrees( -atan2f(mag.my,mag.mx) );
		float h= baro.altimeter(p0,baro.P,baro.T);

		printf("mpu6050 acc=%+3.2f %+3.2f %+3.2f temp=%+3.2f gyro=%+3.2f %+3.2f %+3.2f | ",acc_gyro.ax,acc_gyro.ay,acc_gyro.az,acc_gyro.tp,acc_gyro.gx,acc_gyro.gy,acc_gyro.gz);
		printf("hmc5883 mag=%+3.2f %+3.2f %+3.2f | ",mag.mx,mag.my,mag.mz);
		printf("angles = %f %f %f | ",x,y,z);
		printf("ms5611 p=%f t=%f h=%f\r\n",baro.P,baro.T,h);

	}
	return 0;
}
