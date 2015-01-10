/*
 * ms5611.cpp
 *
 *  Created on: 10/08/2014
 *      Author: root
 */


//i2c defines
#include "i2c-dev.h"
//ioctl
#include <sys/ioctl.h>
//usleep
#include <unistd.h>
//printf
#include <stdio.h>
//pow,sqrt
#include <math.h>

#include "timestamps.h"

#include "ms5611.h"

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

ms5611::ms5611(int fd){
	this->fd=fd;
	init();
	update();

}

void ms5611::i2c_start(){
	if (ioctl(fd,I2C_SLAVE,ADDR_MS5611) < 0) {
		perror("i2c slave hmc5883 Failed");
	}
}

void ms5611::i2c_send(char cmd)
{
	i2c_start();
	write(fd,&cmd,1);
}

void ms5611::i2c_recv(unsigned char* buffer,int buffer_len){
	i2c_start();
	read(fd,buffer,buffer_len);
}

void ms5611::cmd_reset(void)
{
	i2c_send(CMD_RESET); // send reset sequence
	usleep(3 * 1000); // wait for the reset sequence timing - 3ms
}


unsigned int ms5611::cmd_prom(char coef_num)
{
	unsigned int rC=0;
	i2c_send(CMD_PROM_RD+coef_num*2); // send PROM READ command
	unsigned char b[2];
	i2c_recv(b,2);
	rC=256*b[0]+b[1];
	return rC;
}

unsigned char ms5611::crc4(unsigned int n_prom[])
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



void ms5611::init(){
	int i;
	unsigned char n_crc; // crc value of the prom

	cmd_reset(); // reset IC
	for (i=0;i<8;i++){
		C[i]=cmd_prom(i);
		printf("ms5611 prom[%d]=%08x (%d)\r\n",i,C[i],C[i]);
	} // read coefficients
	n_crc=crc4(C); // calculate the CRC
	printf("ms5611 calculated crc=%08x\r\n",n_crc);

	this->state=START_D2;
}


unsigned long ms5611::cmd_adc(char cmd)
{
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




void ms5611::update(){

	unsigned char b[3];

	if(this->state==START_D2){
		i2c_send(CMD_ADC_CONV+CMD_ADC_D2+CMD_ADC_4096);
		this->read_started=get_timestamp_in_seconds();
		this->state=WAIT_D2;
		return;
	}
	if(this->state==WAIT_D2){
		//wait 10ms without block
		if(get_timestamp_in_seconds()-this->read_started<0.01){
			this->state=WAIT_D2;
		}
		else {
			this->state=READ_D2;
		}
		return;
	}
	if(this->state==READ_D2){
		i2c_send(CMD_ADC_READ);
		i2c_recv(b,3);
		D2 = 65536*b[0]+256*b[1]+b[2];
		this->state=START_D1;
		return;
	}
	if(this->state==START_D1){
		i2c_send(CMD_ADC_CONV+CMD_ADC_D1+CMD_ADC_4096);
		this->read_started=get_timestamp_in_seconds();
		this->state=WAIT_D1;
		return;
	}
	if(this->state==WAIT_D1){
		//wait 10ms without block
		if(get_timestamp_in_seconds()-this->read_started<0.01){
			this->state=WAIT_D1;
		}
		else {
			this->state=READ_D1;
		}
		return;
	}
	if(this->state==READ_D1){
		i2c_send(CMD_ADC_READ);
		i2c_recv(b,3);
		D1 = 65536*b[0]+256*b[1]+b[2];
		this->state=COMPUTE_ALL;
		return;
	}

	if(this->state==COMPUTE_ALL){

		//Calculate temperature
		dT=D2-C[5]*pow(2,8);
		T=(2000+(dT*C[6])/pow(2,23));
		//Calculate temperature compensated pressure
		OFF=C[2]*pow(2,16)+dT*C[4]/pow(2,7);
		SENS=C[1]*pow(2,15)+dT*C[3]/pow(2,8);
		///////////////////////////////////////////////////
		//second order
		double t2,off2,sens2;
		// menor que 20.0C
		if(T<2000){
			t2=pow(dT,2)/pow(2,31);
			off2=5*pow(T-2000,2)/pow(2,1);
			sens2=5*pow(T-2000,2)/pow(2,2);
			//menor que 15.0C
			if(T<-1500){
				off2=off2+7*pow(T+1500,2);
				sens2=sens2+11*pow(T+1500,2)/pow(2,1);
			}
		}
		// maior que 20.0C
		else {
			t2=0;
			off2=0;
			sens2=0;
		}
		T-=t2;
		OFF-=off2;
		SENS-=sens2;
		///////////////////////////////////////////////////
		//calcule pressure
		P=(((D1*SENS)/pow(2,21)-OFF)/pow(2,15));
		///////////////////////////////////////////////////
		T/=100.0;
		P/=100.0;
		this->state=START_D2;
		return;
	}

}





