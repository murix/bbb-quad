#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

static void pabort(const char *s)
{
	perror(s);
	abort();
}


static void transfer(int fd,uint8_t* txbuf,int txlen,uint8_t* rxbuf,int rxlen)
{
	int ret;
	struct spi_ioc_transfer tr;
	tr.tx_buf = (unsigned long)txbuf; //tx 
	tr.rx_buf = (unsigned long)rxbuf; //rx
	tr.len = txlen;                   //bytes
	tr.speed_hz= 5000000;
	tr.bits_per_word=8;
	tr.delay_usecs=0;
	tr.cs_change=1;
	
	
	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr); if (ret < 1) pabort("can't send spi message");
}


static int openspi(void){
  
        system("echo ADAFRUIT-SPI0 > /sys/devices/bone_capemgr.9/slots");
        //system("echo BB-SPIDEV0 > /sys/devices/bone_capemgr.9/slots");
	usleep(500);
	
        const char *device = "/dev/spidev1.0";
        uint8_t bits = 8;
        uint32_t speed = 5000000;
        uint8_t mode = 0;


	int ret = 0;
	int fd = 0;

	fd = open(device, O_RDWR);                        if (fd < 0)    pabort("can't open device");
	ret = ioctl(fd, SPI_IOC_WR_MODE,          &mode); if (ret == -1) pabort("can't set bits per mode");
	ret = ioctl(fd, SPI_IOC_RD_MODE,          &mode); if (ret == -1) pabort("can't set bits per mode");
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits); if (ret == -1) pabort("can't set bits per word");
	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits); if (ret == -1) pabort("can't get bits per word");
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed); if (ret == -1) pabort("can't set max speed hz");
	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed); if (ret == -1) pabort("can't get max speed hz");

	printf("spi mode: 0x%x\r\n", mode);
	printf("spi bits per word: %d\r\n", bits);
	printf("spi max speed: %d Hz\r\n", speed);

        return fd;
}


int main(int argc, char *argv[])
{
        int fd = openspi();

        uint8_t txbuf[65],rxbuf[65],txlen,rxlen;
       
        txbuf[0]=0x36;
        txlen=1;
        rxlen=1; 
	transfer(fd,txbuf,txlen,rxbuf,rxlen);
        printf("cb=%02x\r\n",rxbuf[0]);

	close(fd);

	return 0;
}
