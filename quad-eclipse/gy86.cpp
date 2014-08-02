
/// BBB
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
#include <fcntl.h>  /* File control definitions */
#include <unistd.h> /* UNIX standard function definitions */
#include <sys/time.h>
#include <termios.h> /* POSIX terminal control definitions */


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

class sensor_hmc5883 {
public:
	float mx;
	float my;
	float mz;
	sensor_hmc5883(){

	}
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
	sensor_ms5611(){

	}
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
	sensor_mpu6050(){

	}
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

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////



class simple_imu {

	//http://www.analog.com/static/imported-files/application_notes/AN-1057.pdf
	// theta = atan( x / sqrt (y^2 + z^2)  )
	// psi   = atan( y / sqrt (x^2 + z^2)  )
	// phi   = atan( sqrt( x^2 + y^2 ) / z )
	//
	sensor_mpu6050 acc_gyro;
	sensor_hmc5883 mag;
	sensor_ms5611 baro;

	void bbb_sensors_init(){
		printf("enable I2C-2 overlay\r\n");
		system("echo BB-I2C1 > /sys/devices/bone_capemgr.9/slots");
		printf("wait I2C-2 overlay to be ready\r\n");
		usleep(1000000);
		int file;
		if ((file = open("/dev/i2c-2",O_RDWR)) < 0) {
			printf("Failed to open the bus.");
			exit(1);
		}
		acc_gyro=sensor_mpu6050(file);
		mag=sensor_hmc5883(file);
		baro=sensor_ms5611(file);
	}

	simple_imu(){


	}

};

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////



class quaternion_imu {
private:
	sensor_mpu6050 acc_gyro;
	sensor_hmc5883 mag;
	sensor_ms5611 baro;

	void bbb_sensors_init(){
		printf("enable I2C-2 overlay\r\n");
		system("echo BB-I2C1 > /sys/devices/bone_capemgr.9/slots");
		printf("wait I2C-2 overlay to be ready\r\n");
		usleep(1000000);
		int file;
		if ((file = open("/dev/i2c-2",O_RDWR)) < 0) {
			printf("Failed to open the bus.");
			exit(1);
		}
		acc_gyro=sensor_mpu6050(file);
		mag=sensor_hmc5883(file);
		baro=sensor_ms5611(file);
	}

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


	quaternion_imu(){
		bbb_sensors_init();

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
		// zero gyro
		zeroGyro();
		// load calibration from eeprom
		calLoad();

	}
	void calibration_init(){
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


	void zeroGyro(){
		const int totSamples = 10;
		float tmpOffsets[] = {0,0,0};
		for (int i = 0; i < totSamples; i++){
			acc_gyro.update();
			tmpOffsets[0] += acc_gyro.gx;
			tmpOffsets[1] += acc_gyro.gy;
			tmpOffsets[2] += acc_gyro.gz;
		}
		gyro_off_x = tmpOffsets[0] / totSamples;
		gyro_off_y = tmpOffsets[1] / totSamples;
		gyro_off_z = tmpOffsets[2] / totSamples;
	}
	void getQ(float * q){
		acc_gyro.update();
		mag.update();

		now = micros();
		sampleFreq = 1.0 / ((now - lastUpdate) / 1000000.0);
		lastUpdate = now;

		AHRSupdate(acc_gyro.gx * M_PI/180, acc_gyro.gy * M_PI/180, acc_gyro.gz * M_PI/180,
				acc_gyro.ax,acc_gyro.ay,acc_gyro.az,
				mag.mx,mag.my,mag.mz,true);

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
		baro.update();
		float temp = baro.T;
		float press = baro.P;
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
	void AHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz,bool use_mag){

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

		if(use_mag){
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

void test1(){
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

	//vmodem support
	int fd=get_vmodem_fd();
	char buf[1024];

	while(1){
		acc_gyro.update();
		mag.update();
		baro.update();
		float rx=atan2f(acc_gyro.ay,acc_gyro.az);
		float ry=-atan2f(acc_gyro.ax,acc_gyro.az);
		float rz=-atan2f(mag.my,mag.mx);
		float x= to_degrees( rx );
		float y= to_degrees( ry );
		float z= to_degrees( rz );
		float h= baro.altimeter(p0,baro.P,baro.T);
		printf("mpu6050 acc=%+3.2f %+3.2f %+3.2f temp=%+3.2f gyro=%+3.2f %+3.2f %+3.2f | ",acc_gyro.ax,acc_gyro.ay,acc_gyro.az,acc_gyro.tp,acc_gyro.gx,acc_gyro.gy,acc_gyro.gz);
		printf("hmc5883 mag=%+3.2f %+3.2f %+3.2f | ",mag.mx,mag.my,mag.mz);
		printf("angles = %f %f %f | ",x,y,z);
		printf("ms5611 p=%f t=%f h=%f\r\n",baro.P,baro.T,h);

		//vmodem print
		int len = sprintf(buf,"%f|%f|%f|\n",rx,ry,rz);
		write(fd,buf,len);
	}

}

void test2(){

	//vmodem support
	int fd=get_vmodem_fd();
	char buf[1024];


	float angles[3];
	quaternion_imu uav;
	while(1){
		uav.getEulerRad(angles);
		printf("%f %f %f \r\n",to_degrees(angles[0]),to_degrees(angles[1]),to_degrees(angles[2]));

		//vmodem print
		int len = sprintf(buf,"%f|%f|%f|\n",angles[0],angles[1],angles[2]);
		write(fd,buf,len);
	}
}

void test3(){


}

int main(int argc,char** argv){

	if(argc==2){
		int mode=atoi(argv[1]);
		printf("mode=%d\r\n",mode);
		switch(mode){
		case 1: test1(); break;
		case 2: test2(); break;
		case 3: test3(); break;
		default:
			printf("unknown mode\r\n");
			break;
		}
	}
	else {
		printf("%s <mode>\r\n",argv[0]);
	}

	return 0;
}
