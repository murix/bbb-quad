




namespace gy86 {


template <typename T>
T sign(T t) { return (t < 0) ? T(-1) : T(1); }








#define ADDR_MPU6050 0x68

#define ADDR_MS5611  0x77

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


const double def_sea_press = 1013.25;





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
		double h= baro.altimeter(p0,baro.P,baro.T,1);

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

void test_quaternion(char* title,bool use_mag){
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
	//int fd=get_vmodem_fd();
	char buf[1024];

	double angles[3];
	quaternion_imu imu;


	while(1){
		//
		acc_gyro.update();
		mag.update();
		baro.update();
		if(use_mag){
			//
			imu.input_update(acc_gyro.acc,acc_gyro.gyro,mag.mag,baro.P,baro.T);
		} else {
			double magd[3]={0,0,0};
			imu.input_update(acc_gyro.acc,acc_gyro.gyro,magd,baro.P,baro.T);
		}

		imu.getEulerRad(angles);
		//vmodem print - cube test
		int len = sprintf(buf,"%s|%f|%f|%f|%f|%f|%f|\r",
				title,
				angles[2],
				-angles[1],
				-angles[0],
				0,0,0);
		//write(fd,buf,len);
		printf("%s\n",buf);
	}
}




void test_gyro_only_imu(char* title){


	system("echo BB-I2C1 > /sys/devices/bone_capemgr.9/slots");

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

	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////
	// udp server

	imu_data_t idata;

	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////

	// offsets
	double goff[3]={0,0,0};

	// samples
	int gsample=10;

	for(int i=0;i<gsample;i++){

		// read sensor
		acc_gyro.update();

		// sum reads
		for(int i=0;i<3;i++){
			goff[i]+=acc_gyro.gyro[i];
		}
	}

	// offset is average
	for(int i=0;i<3;i++){
		goff[i]/=gsample;
	}

	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////
	// integrate gyroscope step to obtain rotation angles with drift

	double gint[3]={0,0,0};
	double gstep[3]={0,0,0};

	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////
	// time tracking

	double tback=get_timestamp_in_seconds();
	double tnow=get_timestamp_in_seconds();

	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////

	while(1){

		// update time
		tback=tnow;
		tnow=get_timestamp_in_seconds();
		double time_interval=tnow-tback;
		double frequency=1/time_interval;

		// update from sensors
		acc_gyro.update();
		//mag.update();

		// copy sensors to udp server
		for(int i=0;i<3;i++){
			idata.acc[i]=acc_gyro.acc[i];
			idata.gyro[i]=acc_gyro.gyro[i];
			idata.mag[i]=mag.mag[i];
		}

		// calculate gyroscope step
		for(int i=0;i<3;i++){
			// remove gyroscope offset
			double angular_speed=acc_gyro.gyro[i]-goff[i];
			//gyroscope step = angular_speed * time_interval
			gstep[i]= (angular_speed)*time_interval;
		}

		// gyroscope angle convert degree to radian
		for(int i=0;i<3;i++){
			gstep[i]=to_radian(gstep[i]);
		}

		// gyroscope integrate steps
		gint[0]-=gstep[0];
		gint[1]+=gstep[1];
		gint[2]+=gstep[2];

		// orientation from accelerometer
		double accN =sqrt(pow(acc_gyro.acc[0],2) +pow(acc_gyro.acc[1],2) +pow(acc_gyro.acc[2],2));
		double axn=acc_gyro.acc[0]/accN;
		double ayn=acc_gyro.acc[1]/accN;
		double azn=acc_gyro.acc[2]/accN;
		double apitch=atan2(-azn,ayn);
		double aroll=atan2(-axn,ayn);

		// orientation from magnetometer
		double magN =sqrt(pow(mag.mag[0],2)      +pow(mag.mag[1],2)      +pow(mag.mag[2],2));
		double mxn=mag.mag[0]/magN;
		double myn=mag.mag[1]/magN;
		double mzn=mag.mag[2]/magN;

		// copy time to udp
		idata.dt = time_interval;
		idata.hz = frequency;
		idata.gyro_angles[0]=gint[0];
		idata.gyro_angles[1]=gint[1];
		idata.gyro_angles[2]=gint[2];

		idata.acc_angles[0]=acos(axn/accN);
		idata.acc_angles[1]=acos(ayn/accN);
		idata.acc_angles[2]=acos(azn/accN);
		idata.mag_angles[0]=0;
		idata.mag_angles[1]=0;
		idata.mag_angles[2]=0;

		idata.accN=accN;
		idata.magN=magN;
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

	double goff[3]={0,0,0};
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
	double grstep[3]={0,0,0};

	///////////////////////////////////////////
	///////////////////////////////////////////
	///////////////////////////////////////////
	//complementary filter output in radians
	double cr[3]={0,0,0};

	///////////////////////////////////////////
	///////////////////////////////////////////
	///////////////////////////////////////////
	// altimeter
	double havg=0;
	double hoff=0;
	int hsamples=100;
	double hring[hsamples];
	unsigned int hidx=0;

	///////////////////////////////////////////
	///////////////////////////////////////////
	///////////////////////////////////////////
	// pitch and roll offset

	double vpitch[hsamples];
	double pitch_avg = 0;
	double pitch_off = 0;

	double vroll[hsamples];
	double roll_avg = 0;
	double roll_off = 0;

	///////////////////////////////////////////
	///////////////////////////////////////////
	///////////////////////////////////////////
	// yaw tracking

	double yaw_step_now=0;
	double yaw_step_back=0;


	///////////////////////////////////////////
	///////////////////////////////////////////
	///////////////////////////////////////////
	// position tracking

	double acc_back[3]={0,0,0};
	double acc_now[3]={0,0,0};
	double acc_v_back[3]={0,0,0};
	double acc_v_now[3]={0,0,0};
	double acc_p[3]={0,0,0};

	///////////////////////////////////////////
	///////////////////////////////////////////
	///////////////////////////////////////////
	// Android Accelerometer: Low-Pass Filter Estimated Linear Acceleration
	// http://www.kircherelectronics.com/blog/index.php/11-android/sensors/10-low-pass-filter-linear-acceleration
	double gravity[3]={0,0,0};
	double linearAcceleration[3]={0,0,0};
	double input[3]={0,0,0};

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

		double freq = 1/tdiff;
		if(debug)printf("frequency=%fHz tdiff=%f\r\n",freq,tdiff);


		//////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
		//// Linear acceleration and gravity compensation

		double desired_reponse_time=0.2;
		double alpha = desired_reponse_time/(desired_reponse_time/tdiff);
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
		double pitch=atan2f(acc_gyro.acc[1], sqrt(acc_gyro.acc[0]*acc_gyro.acc[0]+acc_gyro.acc[2]*acc_gyro.acc[2]) );
		double roll=-atan2f(acc_gyro.acc[0], sqrt(acc_gyro.acc[1]*acc_gyro.acc[1]+acc_gyro.acc[2]*acc_gyro.acc[2]) );
		double yaw=atan2f(sqrt(acc_gyro.acc[1]*acc_gyro.acc[1]+acc_gyro.acc[0]*acc_gyro.acc[0]) ,acc_gyro.acc[2]);

		if(debug)printf("angles by acc = %f|%f|%f\r\n",pitch,roll,yaw);

		//////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
		// Magnetometer heading
		// COMPASS HEADING USING MAGNETOMETERS AN-203

		double heading = 0;
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
				0.0);
				//havg-hoff);

		write(fd,buf,len);

		printf("%s\n",buf);
	}

}

};



//libjsoncpp-dev
//thrift-compiler

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
		case 9: gy86::test_quaternion("GY86 9DOF Quaternion IMU",true); break;
		case 10: gy86::test_quaternion("GY86 6DOF Quaternion IMU",false); break;
		default: printf("unknown mode\r\n"); break;
		}
	}
	else {
		printf("%s <mode>\r\n",argv[0]);
	}

	return 0;
}
