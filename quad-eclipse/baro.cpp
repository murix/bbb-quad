/*
 * baro.cpp
 *
 *  Created on: 26/07/2014
 *      Author: root
 */


#define ADDR_MS5611  0x77

#define CMD_RESET 0x1E // ADC reset command
#define CMD_ADC_READ 0x00 // ADC read command
#define CMD_ADC_CONV 0x40 // ADC conversion command
#define CMD_ADC_D1 0x00 // ADC D1 conversion
#define CMD_ADC_D2 0x10 // ADC D2 conversion
#define CMD_ADC_256 0x00 // ADC OSR=256
#define CMD_ADC_512 0x02 // ADC OSR=512
#define CMD_ADC_1024 0x04 // ADC OSR=1024
#define CMD_ADC_2048 0x06 // ADC OSR=2048
#define CMD_ADC_4096 0x08 // ADC OSR=4096
#define CMD_PROM_RD 0xA0 // Prom read command

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


class ms5611 {
public:
	int fd;

	void i2c_start(){
		if (ioctl(fd,I2C_SLAVE,ADDR_MS5611) < 0) {
			printf("Failed to acquire bus access and/or talk to slave.\n");
		}
	}
	void i2c_send(char cmd)
	{
		i2c_start();
		write(fd,&cmd,1);
	}
	void i2c_recv(unsigned char* buffer,int buffer_len){
		i2c_start();
		read(fd,buffer,buffer_len);
	}
	void cmd_reset(void)
	{
		i2c_send(CMD_RESET); // send reset sequence
		usleep(3 * 1000); // wait for the reset sequence timing
	}
	unsigned long cmd_adc(char cmd)
	{
		unsigned int ret;
		unsigned long temp=0;
		i2c_send(CMD_ADC_CONV+cmd); // send conversion command
		switch (cmd & 0x0f) // wait necessary conversion time
		{
		case CMD_ADC_256 : usleep(900); break;
		case CMD_ADC_512 : usleep(3 * 1000); break;
		case CMD_ADC_1024: usleep(4 * 1000); break;
		case CMD_ADC_2048: usleep(6 * 1000); break;
		case CMD_ADC_4096: usleep(10 * 1000); break;
		}
		i2c_send(CMD_ADC_READ);
		unsigned char b[3];
		i2c_recv(b,3);

		temp=65536*b[0]+256*b[1]+b[2];
		return temp;
	}

	unsigned int cmd_prom(char coef_num)
	{
		unsigned int ret;
		unsigned int rC=0;
		i2c_send(CMD_PROM_RD+coef_num*2); // send PROM READ command
		unsigned char b[2];
		i2c_recv(b,2);
		rC=256*b[0]+b[1];
		return rC;
	}

	unsigned char crc4(unsigned int n_prom[])
	{
		int cnt; // simple counter
		unsigned int n_rem; // crc reminder
		unsigned int crc_read; // original value of the crc
		unsigned char n_bit;
		n_rem = 0x00;
		crc_read=n_prom[7]; //save read CRC
		n_prom[7]=(0xFF00 & (n_prom[7])); //CRC byte is replaced by 0
		for (cnt = 0; cnt < 16; cnt++) // operation is performed on bytes
		{// choose LSB or MSB
			if (cnt%2==1) n_rem ^= (unsigned short) ((n_prom[cnt>>1]) & 0x00FF);
			else n_rem ^= (unsigned short) (n_prom[cnt>>1]>>8);
			for (n_bit = 8; n_bit > 0; n_bit--)
			{
				if (n_rem & (0x8000))
				{
					n_rem = (n_rem << 1) ^ 0x3000;
				}
				else
				{
					n_rem = (n_rem << 1);
				}
			}
		}
		n_rem= (0x000F & (n_rem >> 12)); // final 4-bit reminder is CRC code
		n_prom[7]=crc_read; // restore the crc_read to its original place
		return (n_rem ^ 0x0);
	}

	void beaglebone_start(){

	    printf("enable I2C-2 overlay\r\n");
	    system("echo BB-I2C1 > /sys/devices/bone_capemgr.9/slots");
	    printf("wait I2C-2 overlay to be ready\r\n");
	    usleep(1* 1000*1000);

	    if ((fd = open("/dev/i2c-2",O_RDWR)) < 0) {
	        printf("Failed to open the bus.");
	        exit(1);
	    }
	}

	void main(){

		beaglebone_start();

		unsigned long D1; // ADC value of the pressure conversion
		unsigned long D2; // ADC value of the temperature conversion
		unsigned int C[8]; // calibration coefficients
		double P; // compensated pressure value
		double T; // compensated temperature value
		double dT; // difference between actual and measured temperature
		double OFF; // offset at actual temperature
		double SENS; // sensitivity at actual temperature
		int i;
		unsigned char n_crc; // crc value of the prom

		D1=0;
		D2=0;
		cmd_reset(); // reset IC
		for (i=0;i<8;i++){
			C[i]=cmd_prom(i);
			printf("prom[%d]=%08x (%d)\r\n",i,C[i],C[i]);
		} // read coefficients
		n_crc=crc4(C); // calculate the CRC
		printf("calculated crc=%08x\r\n",n_crc);

		for(;;) // loop without stopping
		{
			D2=cmd_adc(CMD_ADC_D2+CMD_ADC_4096); // read D2
			D1=cmd_adc(CMD_ADC_D1+CMD_ADC_4096); // read D1
			// calculate 1st order pressure and temperature (MS5607 1st order algorithm)
			dT=D2-C[5]*pow(2,8);
			OFF=C[2]*pow(2,17)+dT*C[4]/pow(2,6);
			SENS=C[1]*pow(2,16)+dT*C[3]/pow(2,7);

			T=(2000+(dT*C[6])/pow(2,23))/100;
			P=(((D1*SENS)/pow(2,21)-OFF)/pow(2,15))/100;
			// place to use P, T, put them on LCD, send them trough RS232 interface...
			printf("P=%d T=%d\r\n",P,T);

		}


	}

};


//********************************************************
//! @brief main program
//!
//! @return 0
//********************************************************
int main (void)
{

	ms5611 b;
	b.main();


	return 0;
}


