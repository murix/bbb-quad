
/// BBB
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>
#include "i2c-dev.h"
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>  /* File control definitions */
#include <unistd.h> /* UNIX standard function definitions */
#include <sys/time.h>
#include <termios.h> /* POSIX terminal control definitions */

namespace gy86 {

#define ADDR_MPU6050 0x68
#define ADDR_HMC5883 0x1e
#define ADDR_MS5611  0x77

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


const float def_sea_press = 1013.25;

double start_time = 0;

//
unsigned long micros(void){
	struct timeval start;
	gettimeofday(&start, NULL);
	double seconds = start.tv_sec + start.tv_usec/1000000.0;
	if(start_time==0){
		start_time=seconds;
		gettimeofday(&start, NULL);
		seconds = start.tv_sec + start.tv_usec/1000000.0;
	}

	double micros = (seconds-start_time)*1000000.0;


	unsigned long ret = micros;
	//printf("micros %lu\r\n",ret);
	return micros;
}

inline float to_degrees(float radians){
	return radians*(180.0/M_PI);
}
inline float to_radian(float degree){
	return degree * M_PI/180;
}

class sensor_hmc5883 {
public:
	float mag[3];
	int fd;

	sensor_hmc5883(int fd){
		this->fd=fd;
		hmc5883_configure();
		mag[0]=0;
		mag[1]=0;
		mag[2]=0;
	}

	void update(){
		if(is_ready()){
			mag[0]= hmc5883_read16(0x3,0x4)/1090.0;
			mag[1]= hmc5883_read16(0x5,0x6)/1090.0;
			mag[2]= hmc5883_read16(0x7,0x8)/1090.0;
		}
	}

	void set_addr(){
		if (ioctl(fd,I2C_SLAVE,ADDR_HMC5883) < 0) {
			printf("hmc5883 i2c addr error\r\n");
			exit(1);
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
	int fd;
	double P,T;
	uint16_t c[8];
	uint32_t d[3];


	sensor_ms5611(int fd){
		this->fd=fd;
		reset();
		read_calib();
		update();
	}
	void update(){
		//read_adc(0);
		read_adc(8);
		calculate();
	}
	double altimeter(double p0,double p,double t,double g_factor){
		//R=gas constant=8.31432
		//tk=air temperature in measured kelvin
		//g=earth gravity=9.80665
		//M=molar mass of the gas=0.0289644
		double tk= t+273.15;
		double R=8.31432;
		double g=9.80665;
		double M=0.0289644;
		return ((R*tk)/(g*M))*log(p0/p);
	}
	void set_addr(){
		if (ioctl(fd,I2C_SLAVE,ADDR_MS5611) < 0) {
			printf("ms5611 i2c addr error\r\n");
			exit(1);
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

	double dt,temp,off,sens,p;
	double t2,off2,sens2;
	void calculate(){
		//first order
		dt=d[2]-c[5]*pow(2,8);
		temp=2000 + dt*c[6]/pow(2,23);
		off=c[2]*pow(2,16)+(c[4]*dt)/pow(2,7);
		sens=c[1]*pow(2,15)+(c[3]*dt)/pow(2,8);
		//second order
		if(temp<2000){
			t2=pow(dt,2)/pow(2,31);
			off2=5*pow(temp-2000,2)/pow(2,1);
			sens2=5*pow(temp-2000,2)/pow(2,2);
			if(temp<-1500){
				off2=off2+7*pow(temp+1500,2);
				sens2=sens2+11*pow(temp+1500,2)/pow(2,1);
			}
		} else {
			t2=0;
			off2=0;
			sens2=0;
		}
		temp=temp-t2;
		off=off-off2;
		sens=sens-sens2;
		//pressure
		p=(d[1]*sens/pow(2,21)-off)/pow(2,15);
		//convert to C and mbar.
		T=temp/100.0;
		P=p/100.0;
	}

};


class sensor_mpu6050 {
public:
	int fd;
	float acc[3];
	float gyro[3];
	float tc;

	sensor_mpu6050(int fd){
		this->fd=fd;
	}
	void update(){
		ioctl(fd,I2C_SLAVE,0x68);
		//master enable
		i2c_smbus_write_byte_data(fd,0x6a,0x00);
		//i2c bypass
		i2c_smbus_write_byte_data(fd,0x37,0x02);
		//wake up from sleep
		i2c_smbus_write_byte_data(fd,0x6b,0x00);
		//read all
		uint16_t vu[7];
		int16_t vs[7];
		i2c_smbus_read_i2c_block_data(fd,0x3b,14,(uint8_t*)vu);
		for(int i=0;i<7;i++){
			vs[i]=(int16_t) __bswap_16(vu[i]);
		}
		//
		acc[0] =vs[0]/16384.0;
		acc[1] =vs[1]/16384.0;
		acc[2] =vs[2]/16384.0;
		tc     =vs[3]/340.0 + 36.53;
		gyro[0]=vs[4]/131.0;
		gyro[1]=vs[5]/131.0;
		gyro[2]=vs[6]/131.0;
	}
};






/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////



class quaternion_imu {
private:
	float input_acc[3];
	float input_gyro[3];
	float input_mag[3];
	float input_baro_p;
	float input_baro_t;



	void arr3_rad_to_deg(float * arr) {
		arr[0] *= 180/M_PI;
		arr[1] *= 180/M_PI;
		arr[2] *= 180/M_PI;
	}

	float invSqrt(float number) {
		union {
			float f;
			int32_t i;
		} y;

		y.f = number;
		y.i = 0x5f375a86 - (y.i >> 1);
		y.f = y.f * ( 1.5f - ( number * 0.5f * y.f * y.f ) );
		return y.f;
	}

	float gyro_off_x, gyro_off_y, gyro_off_z;
	float acc_off_x, acc_off_y, acc_off_z;
	float magn_off_x, magn_off_y, magn_off_z;
	float acc_scale_x, acc_scale_y, acc_scale_z;
	float magn_scale_x, magn_scale_y, magn_scale_z;
	float iq0, iq1, iq2, iq3; // quaternion elements representing the estimated orientation
	float exInt, eyInt, ezInt;  // scaled integral error
	volatile float twoKp;      // 2 * proportional gain (Kp)
	volatile float twoKi;      // 2 * integral gain (Ki)
	volatile float q0, q1, q2, q3; // quaternion of sensor frame relative to auxiliary frame
	volatile float integralFBx,  integralFBy, integralFBz;
	unsigned long lastUpdate, now; // sample period expressed in milliseconds
	float sampleFreq; // half the sample period expressed in seconds
public:

	// acc
	// gyro in degree/s
	// mag
	// baro_p in hpa
	// baro_t in celcius

	void input_update(float acc[3],float gyro[3],float mag[3],float baro_p,float baro_t){
		for(int i=0;i<3;i++){
			input_acc[i]=acc[i];
			input_gyro[i]=gyro[i];
			input_mag[i]=mag[i];
		}
		input_baro_p=baro_p;
		input_baro_t=baro_t;
	}

	quaternion_imu(){
		// initialize quaternion
		q0 = 1.0f;
		q1 = 0.0f;
		q2 = 0.0f;
		q3 = 0.0f;
		exInt = 0.0;
		eyInt = 0.0;
		ezInt = 0.0;
		twoKp = (2.0f * 0.5f); // 2 * proportional gain
		twoKi = (2.0f * 0.1f); // 2 * integral gain

		integralFBx = 0.0f,  integralFBy = 0.0f, integralFBz = 0.0f;
		lastUpdate = 0;
		now = 0;

		// initialize scale factors to neutral values
		calibration_init();
		// load calibration from eeprom
		calLoad();

	}
	void calibration_init(){
		gyro_off_x=0;
		gyro_off_y=0;
		gyro_off_z=0;

		acc_off_x = 0;
		acc_off_y = 0;
		acc_off_z = 0;
		acc_scale_x = 1;
		acc_scale_y = 1;
		acc_scale_z = 1;
		magn_off_x = 0;
		magn_off_y = 0;
		magn_off_z = 0;
		magn_scale_x = 1;
		magn_scale_y = 1;
		magn_scale_z = 1;
	}
	void calLoad(){
		gyro_off_x=0;
		gyro_off_y=0;
		gyro_off_z=0;

		acc_off_x = 0;
		acc_off_y = 0;
		acc_off_z = 0;
		acc_scale_x = 1;
		acc_scale_y = 1;
		acc_scale_z = 1;
		magn_off_x = 0;
		magn_off_y = 0;
		magn_off_z = 0;
		magn_scale_x = 1;
		magn_scale_y = 1;
		magn_scale_z = 1;
	}


	void getQ(float * q){

		now = micros();
		sampleFreq = 1.0 / ((now - lastUpdate) / 1000000.0);
		lastUpdate = now;

		AHRSupdate(
				input_gyro[0] * M_PI/180,
				input_gyro[1] * M_PI/180,
				input_gyro[2] * M_PI/180,
				input_acc[0],
				input_acc[1],
				input_acc[2],
				input_mag[0],
				input_mag[1],
				input_mag[2]
		);

		q[0] = q0;
		q[1] = q1;
		q[2] = q2;
		q[3] = q3;
	}
	void getEuler(float * angles){
		getEulerRad(angles);
		arr3_rad_to_deg(angles);
	}
	void getEulerRad(float * angles){
		float q[4]; // quaternion
		getQ(q);
		angles[0] = atan2(2 * q[1] * q[2] - 2 * q[0] * q[3], 2 * q[0]*q[0] + 2 * q[1] * q[1] - 1); // psi
		angles[1] = -asin(2 * q[1] * q[3] + 2 * q[0] * q[2]); // theta
		angles[2] = atan2(2 * q[2] * q[3] - 2 * q[0] * q[1], 2 * q[0] * q[0] + 2 * q[3] * q[3] - 1); // phi
	}
	void getYawPitchRoll(float * ypr){
		getYawPitchRollRad(ypr);
		arr3_rad_to_deg(ypr);
	}
	void getYawPitchRollRad(float * ypr){
		float q[4]; // quaternion
		float gx, gy, gz; // estimated gravity direction
		getQ(q);


		gx = 2 * (q[1]*q[3] - q[0]*q[2]);
		gy = 2 * (q[0]*q[1] + q[2]*q[3]);
		gz = q[0]*q[0] - q[1]*q[1] - q[2]*q[2] + q[3]*q[3];

		ypr[0] = atan2(2 * q[1] * q[2] - 2 * q[0] * q[3], 2 * q[0]*q[0] + 2 * q[1] * q[1] - 1);
		ypr[1] = atan(gx / sqrt(gy*gy + gz*gz));
		ypr[2] = atan(gy / sqrt(gx*gx + gz*gz));
	}
	float getBaroAlt(){
		return getBaroAlt(def_sea_press);
	}
	float getBaroAlt(float sea_press){
		float temp = input_baro_t;
		float press = input_baro_p;
		return ((pow((sea_press / press), 1/5.257) - 1.0) * (temp + 273.15)) / 0.0065;
	}
	void gravityCompensateAcc(float * acc, float * q){
		float g[3];

		// get expected direction of gravity in the sensor frame
		g[0] = 2 * (q[1] * q[3] - q[0] * q[2]);
		g[1] = 2 * (q[0] * q[1] + q[2] * q[3]);
		g[2] = q[0] * q[0] - q[1] * q[1] - q[2] * q[2] + q[3] * q[3];

		// compensate accelerometer readings with the expected direction of gravity
		acc[0] = acc[0] - g[0];
		acc[1] = acc[1] - g[1];
		acc[2] = acc[2] - g[2];
	}
	void AHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz){

		float recipNorm;
		float q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;
		float halfex = 0.0f, halfey = 0.0f, halfez = 0.0f;
		float qa, qb, qc;

		// Auxiliary variables to avoid repeated arithmetic
		q0q0 = q0 * q0;
		q0q1 = q0 * q1;
		q0q2 = q0 * q2;
		q0q3 = q0 * q3;
		q1q1 = q1 * q1;
		q1q2 = q1 * q2;
		q1q3 = q1 * q3;
		q2q2 = q2 * q2;
		q2q3 = q2 * q3;
		q3q3 = q3 * q3;

		// Use magnetometer measurement only when valid (avoids NaN in magnetometer normalisation)
		if((mx != 0.0f) && (my != 0.0f) && (mz != 0.0f)) {

			float hx, hy, bx, bz;
			float halfwx, halfwy, halfwz;

			// Normalise magnetometer measurement
			recipNorm = invSqrt(mx * mx + my * my + mz * mz);
			mx *= recipNorm;
			my *= recipNorm;
			mz *= recipNorm;

			// Reference direction of Earth's magnetic field
			hx = 2.0f * (mx * (0.5f - q2q2 - q3q3) + my * (q1q2 - q0q3) + mz * (q1q3 + q0q2));
			hy = 2.0f * (mx * (q1q2 + q0q3) + my * (0.5f - q1q1 - q3q3) + mz * (q2q3 - q0q1));
			bx = sqrt(hx * hx + hy * hy);
			bz = 2.0f * (mx * (q1q3 - q0q2) + my * (q2q3 + q0q1) + mz * (0.5f - q1q1 - q2q2));

			// Estimated direction of magnetic field
			halfwx = bx * (0.5f - q2q2 - q3q3) + bz * (q1q3 - q0q2);
			halfwy = bx * (q1q2 - q0q3) + bz * (q0q1 + q2q3);
			halfwz = bx * (q0q2 + q1q3) + bz * (0.5f - q1q1 - q2q2);

			// Error is sum of cross product between estimated direction and measured direction of field vectors
			halfex = (my * halfwz - mz * halfwy);
			halfey = (mz * halfwx - mx * halfwz);
			halfez = (mx * halfwy - my * halfwx);
		}

		// Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
		if((ax != 0.0f) && (ay != 0.0f) && (az != 0.0f)) {
			float halfvx, halfvy, halfvz;

			// Normalise accelerometer measurement
			recipNorm = invSqrt(ax * ax + ay * ay + az * az);
			ax *= recipNorm;
			ay *= recipNorm;
			az *= recipNorm;

			// Estimated direction of gravity
			halfvx = q1q3 - q0q2;
			halfvy = q0q1 + q2q3;
			halfvz = q0q0 - 0.5f + q3q3;

			// Error is sum of cross product between estimated direction and measured direction of field vectors
			halfex += (ay * halfvz - az * halfvy);
			halfey += (az * halfvx - ax * halfvz);
			halfez += (ax * halfvy - ay * halfvx);
		}

		// Apply feedback only when valid data has been gathered from the accelerometer or magnetometer
		if(halfex != 0.0f && halfey != 0.0f && halfez != 0.0f) {
			// Compute and apply integral feedback if enabled
			if(twoKi > 0.0f) {
				integralFBx += twoKi * halfex * (1.0f / sampleFreq);  // integral error scaled by Ki
				integralFBy += twoKi * halfey * (1.0f / sampleFreq);
				integralFBz += twoKi * halfez * (1.0f / sampleFreq);
				gx += integralFBx;  // apply integral feedback
				gy += integralFBy;
				gz += integralFBz;
			}
			else {
				integralFBx = 0.0f; // prevent integral windup
				integralFBy = 0.0f;
				integralFBz = 0.0f;
			}

			// Apply proportional feedback
			gx += twoKp * halfex;
			gy += twoKp * halfey;
			gz += twoKp * halfez;
		}

		// Integrate rate of change of quaternion
		gx *= (0.5f * (1.0f / sampleFreq));   // pre-multiply common factors
		gy *= (0.5f * (1.0f / sampleFreq));
		gz *= (0.5f * (1.0f / sampleFreq));
		qa = q0;
		qb = q1;
		qc = q2;
		q0 += (-qb * gx - qc * gy - q3 * gz);
		q1 += (qa * gx + qc * gz - q3 * gy);
		q2 += (qa * gy - qb * gz + q3 * gx);
		q3 += (qa * gz + qb * gy - qc * gx);

		// Normalise quaternion
		recipNorm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
		q0 *= recipNorm;
		q1 *= recipNorm;
		q2 *= recipNorm;
		q3 *= recipNorm;



	}

};

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////


int get_vmodem_fd(){

	char* dev="/dev/vmodem";

	int fd = open( dev, O_RDWR| O_NOCTTY );

	struct termios tty;
	struct termios tty_old;
	memset (&tty, 0, sizeof tty);

	/* Error Handling */
	if ( tcgetattr (fd, &tty ) != 0 )
	{
		perror(dev); exit(1);
	}

	/* Save old tty parameters */
	tty_old = tty;

	/* Set Baud Rate */
	cfsetospeed (&tty, (speed_t)B115200);
	cfsetispeed (&tty, (speed_t)B115200);

	/* Setting other Port Stuff */
	tty.c_cflag     &=  ~PARENB;        // Make 8n1
	tty.c_cflag     &=  ~CSTOPB;
	tty.c_cflag     &=  ~CSIZE;
	tty.c_cflag     |=  CS8;

	tty.c_cflag     &=  ~CRTSCTS;       // no flow control
	tty.c_cc[VMIN]      =   1;                  // read doesn't block
	tty.c_cc[VTIME]     =   5;                  // 0.5 seconds read timeout
	tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines

	/* Make raw */
	cfmakeraw(&tty);

	/* Flush Port, then applies attributes */
	tcflush( fd, TCIFLUSH );
	if ( tcsetattr ( fd, TCSANOW, &tty ) != 0)
	{
		perror(dev); exit(1);
	}

	return fd;
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

void test_basic(){



	printf("sensors: basic test\r\n");

	//load capes

	printf("enable I2C-2 overlay\r\n");
	system("echo BB-I2C1 > /sys/devices/bone_capemgr.9/slots");
	printf("wait I2C-2 overlay to be ready\r\n");

	//wait capes apply
	usleep(1*1000*1000);

	//open i2c
	int file;
	if ((file = open("/dev/i2c-2",O_RDWR)) < 0) {
		printf("Failed to open the bus.");
		exit(1);
	}
	printf("i2c fd=%d\r\n",file);

	//start raw sensors
	sensor_mpu6050 acc_gyro(file);
	sensor_hmc5883 mag(file);
	sensor_ms5611 baro(file);

	//save initial pressure
	double p0=baro.P;

	//
	while(1){
		//update raw sensors
		acc_gyro.update();
		mag.update();
		baro.update();

		//calculate relative altitude from start point
		float h= baro.altimeter(p0,baro.P,baro.T,1);

		//print
		printf("mpu6050 acc=%+3.2f %+3.2f %+3.2f temp=%+3.2f gyro=%+3.2f %+3.2f %+3.2f | ",
				acc_gyro.acc[0],
				acc_gyro.acc[1],
				acc_gyro.acc[2],
				acc_gyro.tc,
				acc_gyro.gyro[0],
				acc_gyro.gyro[1],
				acc_gyro.gyro[2]);
		printf("hmc5883 mag=%+3.2f %+3.2f %+3.2f | ",
				mag.mag[0],
				mag.mag[1],
				mag.mag[2]);
		printf("ms5611 p=%f t=%f h=%f\r\n",baro.P,baro.T,h);
	}

}

void test_quaternion(char* title){
	printf("quaternion imu cube-test\r\n");

	//load capes
	printf("enable I2C-2 overlay\r\n");
	system("echo BB-I2C1 > /sys/devices/bone_capemgr.9/slots");
	printf("wait I2C-2 overlay to be ready\r\n");

	//wait capes apply
	usleep(1000000);

	//open i2c
	int file;
	if ((file = open("/dev/i2c-2",O_RDWR)) < 0) {
		printf("Failed to open the bus.");
		exit(1);
	}

	//start raw sensors
	sensor_mpu6050 acc_gyro(file);
	sensor_hmc5883 mag(file);
	sensor_ms5611 baro(file);



	//vmodem support - cube test
	int fd=get_vmodem_fd();
	char buf[1024];

	float angles[3];
	quaternion_imu imu;



	while(1){
		//
		acc_gyro.update();
		mag.update();
		baro.update();

		//
		imu.input_update(acc_gyro.acc,acc_gyro.gyro,mag.mag,baro.P,baro.T);
		imu.getEulerRad(angles);
		//vmodem print - cube test
		int len = sprintf(buf,"%s|%f|%f|%f|%f|%f|%f|\r",
				title,
				angles[2],
				-angles[1],
				-angles[0],
				0,0,0);
		write(fd,buf,len);
		printf("%s\n",buf);
	}
}

void test_accel_only_imu(char* title){
	printf("accelerometer only cube-test\r\n");

	//load capes
	printf("enable I2C-2 overlay\r\n");
	system("echo BB-I2C1 > /sys/devices/bone_capemgr.9/slots");
	printf("wait I2C-2 overlay to be ready\r\n");

	//wait capes apply
	usleep(1*1000*1000);

	//open i2c
	int file;
	if ((file = open("/dev/i2c-2",O_RDWR)) < 0) {
		printf("Failed to open the bus.");
		exit(1);
	}

	//start raw sensors
	sensor_mpu6050 acc_gyro(file);
	sensor_hmc5883 mag(file);
	sensor_ms5611 baro(file);

	//vmodem support
	int fd=get_vmodem_fd();
	char buf[1024];

	while(1){
		acc_gyro.update();
		mag.update();

		//http://www.analog.com/static/imported-files/application_notes/AN-1057.pdf

		float pitch=atan2f(acc_gyro.acc[1], sqrt(acc_gyro.acc[0]*acc_gyro.acc[0]+acc_gyro.acc[2]*acc_gyro.acc[2]) );
		float roll=-atan2f(acc_gyro.acc[0], sqrt(acc_gyro.acc[1]*acc_gyro.acc[1]+acc_gyro.acc[2]*acc_gyro.acc[2]) );
		float yaw=atan2f(sqrt(acc_gyro.acc[1]*acc_gyro.acc[1]+acc_gyro.acc[0]*acc_gyro.acc[0]) ,acc_gyro.acc[2]);
		yaw=0;

		//vmodem print
		int len = sprintf(buf,"%s|%f|%f|%f|%f|%f|%f|\r",title,pitch,roll,yaw,0,0,0);
		write(fd,buf,len);

		printf("%s\n",buf);
	}
}

double get_timestamp_in_seconds(){
	struct timeval start;
	gettimeofday(&start, NULL);
	double seconds = start.tv_sec + start.tv_usec/1000000.0;
	return seconds;
}

void test_gyro_only_imu(char* title){
	printf("gyroscope only cube-test\r\n");

	//load capes
	printf("enable I2C-2 overlay\r\n");
	system("echo BB-I2C1 > /sys/devices/bone_capemgr.9/slots");
	printf("wait I2C-2 overlay to be ready\r\n");

	//wait capes apply
	usleep(1000000);

	//open i2c
	int file;
	if ((file = open("/dev/i2c-2",O_RDWR)) < 0) {
		printf("Failed to open the bus.");
		exit(1);
	}

	//start raw sensors
	sensor_mpu6050 acc_gyro(file);
	sensor_hmc5883 mag(file);
	sensor_ms5611 baro(file);

	//vmodem support
	int fd=get_vmodem_fd();
	char buf[1024];

	float goff[3]={0,0,0};
	int gsample=10;
	for(int i=0;i<gsample;i++){
		acc_gyro.update();
		for(int i=0;i<3;i++){
			goff[i]+=acc_gyro.gyro[i];
		}
	}
	for(int i=0;i<3;i++){
		goff[i]/=gsample;
	}

	float gint[3]={0,0,0};

	double tback=get_timestamp_in_seconds();
	double tnow=get_timestamp_in_seconds();

	float gstep[3]={0,0,0};

	while(1){

		tback=tnow;
		tnow=get_timestamp_in_seconds();
		double tdiff=tnow-tback;

		acc_gyro.update();

		for(int i=0;i<3;i++){
			gstep[i]= (acc_gyro.gyro[i]-goff[i])*tdiff;
		}

		for(int i=0;i<3;i++){
			gint[i]+=gstep[i];
		}

		float rx=to_radian(gint[0]);
		float ry=to_radian(gint[1]);
		float rz=to_radian(gint[2]);

		//vmodem print
		int len = sprintf(buf,"%s|%f|%f|%f|%f|%f|%f|\r",title,rx,ry,rz,0,0,0);
		write(fd,buf,len);

		printf("%s\n",buf);
	}
}


void test_ms5611(){

	//load capes
	printf("enable I2C-2 overlay\r\n");
	system("echo BB-I2C1 > /sys/devices/bone_capemgr.9/slots");
	printf("wait I2C-2 overlay to be ready\r\n");

	//wait capes apply
	usleep(1000000);

	//open i2c
	int file;
	if ((file = open("/dev/i2c-2",O_RDWR)) < 0) {
		printf("Failed to open the bus.");
		exit(1);
	}

	sensor_ms5611 baro(file);
	while(1){
		baro.update();
		double altimeter=(baro.altimeter(1013.25,baro.P,baro.T,1)*100);
		printf("temp(C)=%f press(mbar)=%f altimeter(cm)=%f\r\n",
				baro.T,
				baro.P,
				altimeter);
	}


}


// Inertial measurement unit (IMU)
// Attitude and heading reference system (ARHS)
// http://www.x-io.co.uk/open-source-imu-and-ahrs-algorithms/

void test_6dof_imu(char* title,bool use_altimeter,bool use_position,bool use_mag){

	//load capes
	printf("enable I2C-2 overlay\r\n");
	system("echo BB-I2C1 > /sys/devices/bone_capemgr.9/slots");
	printf("wait I2C-2 overlay to be ready\r\n");

	//wait capes apply
	usleep(1000000);

	//open i2c
	int file;
	if ((file = open("/dev/i2c-2",O_RDWR)) < 0) {
		printf("Failed to open the bus.");
		exit(1);
	}

	//start raw sensors
	sensor_mpu6050 acc_gyro(file);
	sensor_hmc5883 mag(file);
	sensor_ms5611 baro(file);

	//vmodem support
	int fd=get_vmodem_fd();


	////////////////////////////////////////////
	////////////////////////////////////////////
	////////////////////////////////////////////
	// calculate gyroscope offset

	float goff[3]={0,0,0};
	int gsamples=30;

	for(int i=0;i<gsamples;i++){
		acc_gyro.update();
		for(int i=0;i<3;i++){
			goff[i]+=acc_gyro.gyro[i];
		}
	}
	for(int i=0;i<3;i++){
		goff[i]/=gsamples;
	}

	///////////////////////////////////////////
	///////////////////////////////////////////
	///////////////////////////////////////////
	//time tracking
	double tback=get_timestamp_in_seconds();
	double tnow=get_timestamp_in_seconds();

	///////////////////////////////////////////
	///////////////////////////////////////////
	///////////////////////////////////////////
	//gyroscope distance in radians
	float grstep[3]={0,0,0};

	///////////////////////////////////////////
	///////////////////////////////////////////
	///////////////////////////////////////////
	//complementary filter output in radians
	float cr[3]={0,0,0};

	///////////////////////////////////////////
	///////////////////////////////////////////
	///////////////////////////////////////////
	// altimeter
	float havg=0;
	float hoff=0;
	int hsamples=100;
	float hring[hsamples];
	unsigned int hidx=0;

	///////////////////////////////////////////
	///////////////////////////////////////////
	///////////////////////////////////////////
	// pitch and roll offset

	float vpitch[hsamples];
	float pitch_avg = 0;
	float pitch_off = 0;

	float vroll[hsamples];
	float roll_avg = 0;
	float roll_off = 0;

	///////////////////////////////////////////
	///////////////////////////////////////////
	///////////////////////////////////////////
	// yaw tracking

	float yaw_step_now=0;
	float yaw_step_back=0;


	///////////////////////////////////////////
	///////////////////////////////////////////
	///////////////////////////////////////////
	// position tracking

	float acc_back[3]={0,0,0};
	float acc_now[3]={0,0,0};
	float acc_v_back[3]={0,0,0};
	float acc_v_now[3]={0,0,0};
	float acc_p[3]={0,0,0};

	///////////////////////////////////////////
	///////////////////////////////////////////
	///////////////////////////////////////////
	// Android Accelerometer: Low-Pass Filter Estimated Linear Acceleration
	// http://www.kircherelectronics.com/blog/index.php/11-android/sensors/10-low-pass-filter-linear-acceleration
	float gravity[3]={0,0,0};
	float linearAcceleration[3]={0,0,0};
	float input[3]={0,0,0};

	bool debug=false;
	bool calib_ok=false;

	while(1){
		//////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
		// Update sensors

		acc_gyro.update();
		mag.update();
		baro.update();

		if(debug)printf("acc=%f|%f|%f atc=%f gyro=%f|%f|%f mag=%f|%f|%f btc=%f p=%f\r\n",
				acc_gyro.acc[0],
				acc_gyro.acc[1],
				acc_gyro.acc[2],
				acc_gyro.tc,
				acc_gyro.gyro[0],
				acc_gyro.gyro[1],
				acc_gyro.gyro[2],
				mag.mag[0],
				mag.mag[1],
				mag.mag[2],
				baro.T,
				baro.P);

		//////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
		//// Time tracking

		tback=tnow;
		tnow=get_timestamp_in_seconds();
		double tdiff=tnow-tback;

		float freq = 1/tdiff;
		if(debug)printf("frequency=%fHz tdiff=%f\r\n",freq,tdiff);


		//////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
		//// Linear acceleration and gravity compensation

		float desired_reponse_time=0.2;
		float alpha = desired_reponse_time/(desired_reponse_time/tdiff);
		if(debug)printf("alpha=%f\r\n",alpha);

		for(int i=0;i<3;i++){
			input[i]=acc_gyro.acc[i];
			gravity[i] = alpha * gravity[i] + (1 - alpha) * input[i];
			linearAcceleration[i] = input[i] - gravity[i];
		}

		if(debug)printf("g=%f|%f|%f l=%f|%f|%f\r\n",
				gravity[0],
				gravity[1],
				gravity[2],
				linearAcceleration[0],
				linearAcceleration[1],
				linearAcceleration[2]
		);

		//////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
		/// Position tracking

		//Implementing Positioning Algorithms Using Accelerometers
		//http://cache.freescale.com/files/sensors/doc/app_note/AN3397.pdf
		//earth gravity=9.80665

		//backup acceleration
		for(int i=0;i<3;i++){
			acc_back[i]=acc_now[3];
		}
		//update acceleration
		for(int i=0;i<3;i++){
			acc_now[i]=acc_gyro.acc[i];
		}
		//backup speed
		for(int i=0;i<3;i++) acc_v_back[i]=acc_v_now[i];
		//speed via trapezoidal integrate of acceleration
		for(int i=0;i<3;i++) acc_v_now[i]+= acc_now[i]+(acc_now[i]-acc_back[i])/2.0;
		//position via trapezoidal integrate of speed
		for(int i=0;i<3;i++) acc_p[i]=acc_v_now[i]+(acc_v_now[i]-acc_v_back[i])/2.0;

		if(debug)printf("speed=%f|%f|%f position=%f|%f|%f\r\n",
				acc_v_now[0],
				acc_v_now[1],
				acc_v_now[2],
				acc_p[0],
				acc_p[1],
				acc_p[2]
		);

		//////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
		// Gyroscope relative angles

		for(int i=0;i<3;i++){
			grstep[i]= to_radian((acc_gyro.gyro[i]-goff[i])*tdiff);
		}

		if(debug)printf("gyro step=%f|%f|%f\r\n",
				grstep[0],grstep[1],grstep[2]);

		//////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
		// Accelerometer absolute angles

		//http://www.analog.com/static/imported-files/application_notes/AN-1057.pdf
		float pitch=atan2f(acc_gyro.acc[1], sqrt(acc_gyro.acc[0]*acc_gyro.acc[0]+acc_gyro.acc[2]*acc_gyro.acc[2]) );
		float roll=-atan2f(acc_gyro.acc[0], sqrt(acc_gyro.acc[1]*acc_gyro.acc[1]+acc_gyro.acc[2]*acc_gyro.acc[2]) );
		float yaw=atan2f(sqrt(acc_gyro.acc[1]*acc_gyro.acc[1]+acc_gyro.acc[0]*acc_gyro.acc[0]) ,acc_gyro.acc[2]);

		if(debug)printf("angles by acc = %f|%f|%f\r\n",pitch,roll,yaw);

		//////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
		// Magnetometer heading
		// COMPASS HEADING USING MAGNETOMETERS AN-203

		float heading = 0;
		if(mag.mag[1]>0) heading = 90  - to_degrees(atan2f(mag.mag[0],mag.mag[1]));
		if(mag.mag[1]<0) heading = 270 - to_degrees(atan2f(mag.mag[0],mag.mag[1]));
		if(mag.mag[1]==0 && mag.mag[0]<0) heading = 180.0;
		if(mag.mag[1]==0 && mag.mag[0]>0) heading = 0.0;
		heading = to_radian(heading);

		if(debug)printf("heading=%f (%f degrees)\r\n",heading,to_degrees(heading));

		//////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
		// Complementary filter - gyroscope and accelerometer

		cr[0]=(1-alpha)*(cr[0]+grstep[0])+(alpha)*(pitch);
		cr[1]=(1-alpha)*(cr[1]+grstep[1])+(alpha)*(roll);
		if(debug)printf("comp filter gyro+acc pitch=%f roll=%f\r\n",cr[0],cr[1]);

		if(use_mag){
			// Complementary filter - gyroscope and magnetometer
			cr[2]=(1-alpha)*(cr[1]+grstep[1])+(alpha)*(heading);
			if(debug)printf("comp filter gyro+mag yaw=%f\r\n",cr[2]);
		} else {
			// yaw via gyroscope trapezoidal integrate
			yaw_step_back = yaw_step_now;
			yaw_step_now = grstep[2];
			cr[2]+= yaw_step_now + (yaw_step_now-yaw_step_back)/2.0;
			if(debug)printf("yaw by gyro integration = %f\r\n",cr[2]);
		}

		//////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
		// update circular buffer index


		//////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
		// altimeter MAVG


		hring[hidx%hsamples]=baro.altimeter(1013.25,baro.P,baro.T,1)*100;
		vpitch[hidx%hsamples]=cr[0];
		vroll[hidx%hsamples]=cr[1];
		hidx++;
		if(hidx>hsamples){
			havg=0;
			pitch_avg=0;
			roll_avg=0;
			for(int i=0;i<hsamples;i++){
				havg+=hring[i];
				pitch_avg+=vpitch[i];
				roll_avg+=vroll[i];
			}
			havg/=hsamples;
			pitch_avg/=hsamples;;
			roll_avg/=hsamples;;

			if(!calib_ok){
				calib_ok=true;
				pitch_off=pitch_avg;
				roll_off=roll_avg;
				hoff=havg;
			}

		}

		if(debug)printf("havg=%f\r\n",havg);
		if(debug)printf("pitch_avg=%f roll_avg=%f\r\n",pitch_avg,roll_avg);

		//////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////



		//vmodem print

		char buf[1024];
		int len = sprintf(buf,"%s|%f|%f|%f|%f|%f|%f|\r",
				title,
				cr[0]-pitch_off,
				cr[1]-roll_off,
				cr[2],
				0.0,
				0.0,
				havg-hoff);

		write(fd,buf,len);

		printf("%s\n",buf);
	}

}

};

int main(int argc,char** argv){

	if(argc==2){
		int mode=atoi(argv[1]);
		printf("mode=%d\r\n",mode);
		switch(mode){
		case 0: gy86::test_ms5611(); break;
		case 1: gy86::test_basic(); break;
		case 2: gy86::test_accel_only_imu("GY-86 3DOF accelerometer only IMU"); break;
		case 3: gy86::test_gyro_only_imu("GY-86 3DOF gyroscope only IMU"); break;
		case 4: gy86::test_6dof_imu("GY86 6DOF comp filter",false,false,false); break;
		case 5: gy86::test_6dof_imu("GY86 6DOF comp filter with altimeter",true,false,false); break;
		case 6: gy86::test_6dof_imu("GY86 6DOF comp filter with altimeter and position",true,true,false); break;
		case 7: gy86::test_6dof_imu("GY86 9DOF comp filter with altimeter and mag",true,false,true); break;
		case 8: gy86::test_6dof_imu("GY86 9DOF comp filter with altimeter and position and mag",true,true,true); break;
		case 9: gy86::test_quaternion("GY86 9DOF Quaternion IMU"); break;
		default: printf("unknown mode\r\n"); break;
		}
	}
	else {
		printf("%s <mode>\r\n",argv[0]);
	}

	return 0;
}
