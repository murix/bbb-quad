

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>

//SPI
#include <linux/spi/spidev.h>

//TAP
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/if_tun.h>



#include "cc1101_linux.h"




/* Radio configurations exported from SmartRF Studio*/

/*
 * Deviation = 5.157471 
 * Base frequency = 433.999969 
 * Carrier frequency = 433.999969 
 * Channel number = 0 
 * Carrier frequency = 433.999969 
 * Modulated = true 
 * Modulation format = GFSK 
 * Manchester enable = false 
 * Sync word qualifier mode = 30/32 sync word bits detected 
 * Preamble count = 4 
 * Channel spacing = 199.951172 
 * Carrier frequency = 433.999969 
 * Data rate = 1.19948 
 * RX filter BW = 58.035714 
 * Data format = Normal mode 
 * Length config = Variable packet length mode. Packet length configured by the first byte after sync word 
 * CRC enable = true 
 * Packet length = 255 
 * Device address = 0 
 * Address config = No address check 
 * CRC autoflush = false 
 * PA ramping = false 
 * TX power = -10 
 */
const registerSetting_t CC11001_DataRate_1200[] = {
		{CC1101_IOCFG2    ,0x29},  /* IOCFG2        GDO2 Output Pin Configuration                  */
		{CC1101_IOCFG1    ,0x2E},  /* IOCFG1        GDO1 Output Pin Configuration                  */
		{CC1101_IOCFG0    ,0x06},  /* IOCFG0        GDO0 Output Pin Configuration                  */
		{CC1101_FIFOTHR   ,0x07},  /* FIFOTHR       RX FIFO and TX FIFO Thresholds                 */
		{CC1101_SYNC1     ,0xD3},  /* SYNC1         Sync Word, High Byte                           */
		{CC1101_SYNC0     ,0x91},  /* SYNC0         Sync Word, Low Byte                            */
		{CC1101_PKTLEN    ,0xFF},  /* PKTLEN        Packet Length                                  */
		{CC1101_PKTCTRL1  ,0x04},  /* PKTCTRL1      Packet Automation Control                      */
		{CC1101_PKTCTRL0  ,0x05},  /* PKTCTRL0      Packet Automation Control                      */
		{CC1101_ADDR      ,0x00},  /* ADDR          Device Address                                 */
		{CC1101_CHANNR    ,0x00},  /* CHANNR        Channel Number                                 */
		{CC1101_FSCTRL1   ,0x06},  /* FSCTRL1       Frequency Synthesizer Control                  */
		{CC1101_FSCTRL0   ,0x00},  /* FSCTRL0       Frequency Synthesizer Control                  */
		{CC1101_FREQ2     ,0x10},  /* FREQ2         Frequency Control Word, High Byte              */
		{CC1101_FREQ1     ,0xB1},  /* FREQ1         Frequency Control Word, Middle Byte            */
		{CC1101_FREQ0     ,0x3B},  /* FREQ0         Frequency Control Word, Low Byte               */
		{CC1101_MDMCFG4   ,0xF5},  /* MDMCFG4       Modem Configuration                            */
		{CC1101_MDMCFG3   ,0x83},  /* MDMCFG3       Modem Configuration                            */
		{CC1101_MDMCFG2   ,0x13},  /* MDMCFG2       Modem Configuration                            */
		{CC1101_MDMCFG1   ,0x22},  /* MDMCFG1       Modem Configuration                            */
		{CC1101_MDMCFG0   ,0xF8},  /* MDMCFG0       Modem Configuration                            */
		{CC1101_DEVIATN   ,0x15},  /* DEVIATN       Modem Deviation Setting                        */
		{CC1101_MCSM2     ,0x07},  /* MCSM2         Main Radio Control State Machine Configuration */
		{CC1101_MCSM1     ,0x30},  /* MCSM1         Main Radio Control State Machine Configuration */
		{CC1101_MCSM0     ,0x18},  /* MCSM0         Main Radio Control State Machine Configuration */
		{CC1101_FOCCFG    ,0x16},  /* FOCCFG        Frequency Offset Compensation Configuration    */
		{CC1101_BSCFG     ,0x6C},  /* BSCFG         Bit Synchronization Configuration              */
		{CC1101_AGCCTRL2  ,0x03},  /* AGCCTRL2      AGC Control                                    */
		{CC1101_AGCCTRL1  ,0x40},  /* AGCCTRL1      AGC Control                                    */
		{CC1101_AGCCTRL0  ,0x91},  /* AGCCTRL0      AGC Control                                    */
		{CC1101_WOREVT1   ,0x87},  /* WOREVT1       High Byte Event0 Timeout                       */
		{CC1101_WOREVT0   ,0x6B},  /* WOREVT0       Low Byte Event0 Timeout                        */
		{CC1101_WORCTRL   ,0xFB},  /* WORCTRL       Wake On Radio Control                          */
		{CC1101_FREND1    ,0x56},  /* FREND1        Front End RX Configuration                     */
		{CC1101_FREND0    ,0x10},  /* FREND0        Front End TX Configuration                     */
		{CC1101_FSCAL3    ,0xE9},  /* FSCAL3        Frequency Synthesizer Calibration              */
		{CC1101_FSCAL2    ,0x2A},  /* FSCAL2        Frequency Synthesizer Calibration              */
		{CC1101_FSCAL1    ,0x00},  /* FSCAL1        Frequency Synthesizer Calibration              */
		{CC1101_FSCAL0    ,0x1F},  /* FSCAL0        Frequency Synthesizer Calibration              */
		{CC1101_RCCTRL1   ,0x41},  /* RCCTRL1       RC Oscillator Configuration                    */
		{CC1101_RCCTRL0   ,0x00},  /* RCCTRL0       RC Oscillator Configuration                    */
		{CC1101_FSTEST    ,0x59},  /* FSTEST        Frequency Synthesizer Calibration Control      */
		{CC1101_PTEST     ,0x7F},  /* PTEST         Production Test                                */
		{CC1101_AGCTEST   ,0x3F},  /* AGCTEST       AGC Test                                       */
		{CC1101_TEST2     ,0x81},  /* TEST2         Various Test Settings                          */
		{CC1101_TEST1     ,0x35},  /* TEST1         Various Test Settings                          */
		{CC1101_TEST0     ,0x09}  /* TEST0         Various Test Settings                           */
}; 

/* 
 *  Deviation = 20.629883 
 *  Base frequency = 433.999969 
 *  Carrier frequency = 433.999969 
 *  Channel number = 0 
 *  Carrier frequency = 433.999969 
 *  Modulated = true 
 *  Modulation format = GFSK 
 *  Manchester enable = false 
 *  Sync word qualifier mode = 30/32 sync word bits detected 
 *  Preamble count = 4 
 *  Channel spacing = 199.951172 
 *  Carrier frequency = 433.999969 
 *  Data rate = 38.3835 
 *  RX filter BW = 101.562500 
 *  Data format = Normal mode 
 *  Length config = Variable packet length mode. Packet length configured by the first byte after sync word 
 *  CRC enable = true 
 *  Packet length = 255 
 *  Device address = 0 
 *  Address config = No address check 
 *  CRC autoflush = false 
 *  PA ramping = false 
 *  TX power = -10 
 */

const registerSetting_t CC11001_DataRate_38400[] = {
		{CC1101_IOCFG2    ,0x29},  /* IOCFG2        GDO2 Output Pin Configuration                  */
		{CC1101_IOCFG1    ,0x2E},  /* IOCFG1        GDO1 Output Pin Configuration                  */
		{CC1101_IOCFG0    ,0x06},  /* IOCFG0        GDO0 Output Pin Configuration                  */
		{CC1101_FIFOTHR   ,0x07},  /* FIFOTHR       RX FIFO and TX FIFO Thresholds                 */
		{CC1101_SYNC1     ,0xD3},  /* SYNC1         Sync Word, High Byte                           */
		{CC1101_SYNC0     ,0x91},  /* SYNC0         Sync Word, Low Byte                            */
		{CC1101_PKTLEN    ,0xFF},  /* PKTLEN        Packet Length                                  */
		{CC1101_PKTCTRL1  ,0x04},  /* PKTCTRL1      Packet Automation Control                      */
		{CC1101_PKTCTRL0  ,0x05},  /* PKTCTRL0      Packet Automation Control                      */
		{CC1101_ADDR      ,0x00},  /* ADDR          Device Address                                 */
		{CC1101_CHANNR    ,0x00},  /* CHANNR        Channel Number                                 */
		{CC1101_FSCTRL1   ,0x06},  /* FSCTRL1       Frequency Synthesizer Control                  */
		{CC1101_FSCTRL0   ,0x00},  /* FSCTRL0       Frequency Synthesizer Control                  */
		{CC1101_FREQ2     ,0x10},  /* FREQ2         Frequency Control Word, High Byte              */
		{CC1101_FREQ1     ,0xB1},  /* FREQ1         Frequency Control Word, Middle Byte            */
		{CC1101_FREQ0     ,0x3B},  /* FREQ0         Frequency Control Word, Low Byte               */
		{CC1101_MDMCFG4   ,0xCA},  /* MDMCFG4       Modem Configuration                            */
		{CC1101_MDMCFG3   ,0x83},  /* MDMCFG3       Modem Configuration                            */
		{CC1101_MDMCFG2   ,0x13},  /* MDMCFG2       Modem Configuration                            */
		{CC1101_MDMCFG1   ,0x22},  /* MDMCFG1       Modem Configuration                            */
		{CC1101_MDMCFG0   ,0xF8},  /* MDMCFG0       Modem Configuration                            */
		{CC1101_DEVIATN   ,0x35},  /* DEVIATN       Modem Deviation Setting                        */
		{CC1101_MCSM2     ,0x07},  /* MCSM2         Main Radio Control State Machine Configuration */
		{CC1101_MCSM1     ,0x30},  /* MCSM1         Main Radio Control State Machine Configuration */
		{CC1101_MCSM0     ,0x18},  /* MCSM0         Main Radio Control State Machine Configuration */
		{CC1101_FOCCFG    ,0x16},  /* FOCCFG        Frequency Offset Compensation Configuration    */
		{CC1101_BSCFG     ,0x6C},  /* BSCFG         Bit Synchronization Configuration              */
		{CC1101_AGCCTRL2  ,0x43},  /* AGCCTRL2      AGC Control                                    */
		{CC1101_AGCCTRL1  ,0x40},  /* AGCCTRL1      AGC Control                                    */
		{CC1101_AGCCTRL0  ,0x91},  /* AGCCTRL0      AGC Control                                    */
		{CC1101_WOREVT1   ,0x87},  /* WOREVT1       High Byte Event0 Timeout                       */
		{CC1101_WOREVT0   ,0x6B},  /* WOREVT0       Low Byte Event0 Timeout                        */
		{CC1101_WORCTRL   ,0xFB},  /* WORCTRL       Wake On Radio Control                          */
		{CC1101_FREND1    ,0x56},  /* FREND1        Front End RX Configuration                     */
		{CC1101_FREND0    ,0x10},  /* FREND0        Front End TX Configuration                     */
		{CC1101_FSCAL3    ,0xE9},  /* FSCAL3        Frequency Synthesizer Calibration              */
		{CC1101_FSCAL2    ,0x2A},  /* FSCAL2        Frequency Synthesizer Calibration              */
		{CC1101_FSCAL1    ,0x00},  /* FSCAL1        Frequency Synthesizer Calibration              */
		{CC1101_FSCAL0    ,0x1F},  /* FSCAL0        Frequency Synthesizer Calibration              */
		{CC1101_RCCTRL1   ,0x41},  /* RCCTRL1       RC Oscillator Configuration                    */
		{CC1101_RCCTRL0   ,0x00},  /* RCCTRL0       RC Oscillator Configuration                    */
		{CC1101_FSTEST    ,0x59},  /* FSTEST        Frequency Synthesizer Calibration Control      */
		{CC1101_PTEST     ,0x7F},  /* PTEST         Production Test                                */
		{CC1101_AGCTEST   ,0x3F},  /* AGCTEST       AGC Test                                       */
		{CC1101_TEST2     ,0x81},  /* TEST2         Various Test Settings                          */
		{CC1101_TEST1     ,0x35},  /* TEST1         Various Test Settings                          */
		{CC1101_TEST0     ,0x09}  /* TEST0         Various Test Settings                           */
}; 

/* 
 *  Deviation = 126.953125 
 *  Base frequency = 433.999969 
 *  Carrier frequency = 433.999969 
 *  Channel number = 0 
 *  Carrier frequency = 433.999969 
 *  Modulated = true 
 *  Modulation format = GFSK 
 *  Manchester enable = false 
 *  Sync word qualifier mode = 30/32 sync word bits detected 
 *  Preamble count = 4 
 *  Channel spacing = 199.951172 
 *  Carrier frequency = 433.999969 
 *  Data rate = 249.939 
 *  RX filter BW = 541.666667 
 *  Data format = Normal mode 
 *  Length config = Variable packet length mode. Packet length configured by the first byte after sync word 
 *  CRC enable = true 
 *  Packet length = 255 
 *  Device address = 0 
 *  Address config = No address check 
 *  CRC autoflush = false 
 *  PA ramping = false 
 *  TX power = -10 
 */

const registerSetting_t CC11001_DataRate_250000[] = {
		{CC1101_IOCFG2    ,0x29},  /* IOCFG2        GDO2 Output Pin Configuration                     */
		{CC1101_IOCFG1    ,0x2E},  /* IOCFG1        GDO1 Output Pin Configuration                     */
		{CC1101_IOCFG0    ,0x06},  /* IOCFG0        GDO0 Output Pin Configuration                     */
		{CC1101_FIFOTHR   ,0x07},  /* FIFOTHR       RX FIFO and TX FIFO Thresholds                    */
		{CC1101_SYNC1     ,0xD3},  /* SYNC1         Sync Word, High Byte                              */
		{CC1101_SYNC0     ,0x91},  /* SYNC0         Sync Word, Low Byte                               */
		{CC1101_PKTLEN    ,0xFF},  /* PKTLEN        Packet Length                                     */
		{CC1101_PKTCTRL1  ,0x04},  /* PKTCTRL1      Packet Automation Control                         */
		{CC1101_PKTCTRL0  ,0x05},  /* PKTCTRL0      Packet Automation Control                         */
		{CC1101_ADDR      ,0x00},  /* ADDR          Device Address                                    */
		{CC1101_CHANNR    ,0x00},  /* CHANNR        Channel Number                                    */
		{CC1101_FSCTRL1   ,0x0C},  /* FSCTRL1       Frequency Synthesizer Control                     */
		{CC1101_FSCTRL0   ,0x00},  /* FSCTRL0       Frequency Synthesizer Control                     */
		{CC1101_FREQ2     ,0x10},  /* FREQ2         Frequency Control Word, High Byte                 */
		{CC1101_FREQ1     ,0xB1},  /* FREQ1         Frequency Control Word, Middle Byte               */
		{CC1101_FREQ0     ,0x3B},  /* FREQ0         Frequency Control Word, Low Byte                  */
		{CC1101_MDMCFG4   ,0x2D},  /* MDMCFG4       Modem Configuration                               */
		{CC1101_MDMCFG3   ,0x3B},  /* MDMCFG3       Modem Configuration                               */
		{CC1101_MDMCFG2   ,0x13},  /* MDMCFG2       Modem Configuration                               */
		{CC1101_MDMCFG1   ,0x22},  /* MDMCFG1       Modem Configuration                               */
		{CC1101_MDMCFG0   ,0xF8},  /* MDMCFG0       Modem Configuration                               */
		{CC1101_DEVIATN   ,0x62},  /* DEVIATN       Modem Deviation Setting                           */
		{CC1101_MCSM2     ,0x07},  /* MCSM2         Main Radio Control State Machine Configuration    */
		{CC1101_MCSM1     ,0x30},  /* MCSM1         Main Radio Control State Machine Configuration    */
		{CC1101_MCSM0     ,0x18},  /* MCSM0         Main Radio Control State Machine Configuration    */
		{CC1101_FOCCFG    ,0x1D},  /* FOCCFG        Frequency Offset Compensation Configuration       */
		{CC1101_BSCFG     ,0x1C},  /* BSCFG         Bit Synchronization Configuration                 */
		{CC1101_AGCCTRL2  ,0xC7},  /* AGCCTRL2      AGC Control                                       */
		{CC1101_AGCCTRL1  ,0x00},  /* AGCCTRL1      AGC Control                                       */
		{CC1101_AGCCTRL0  ,0xB0},  /* AGCCTRL0      AGC Control                                       */
		{CC1101_WOREVT1   ,0x87},  /* WOREVT1       High Byte Event0 Timeout                          */
		{CC1101_WOREVT0   ,0x6B},  /* WOREVT0       Low Byte Event0 Timeout                           */
		{CC1101_WORCTRL   ,0xFB},  /* WORCTRL       Wake On Radio Control                             */
		{CC1101_FREND1    ,0xB6},  /* FREND1        Front End RX Configuration                        */
		{CC1101_FREND0    ,0x10},  /* FREND0        Front End TX Configuration                        */
		{CC1101_FSCAL3    ,0xEA},  /* FSCAL3        Frequency Synthesizer Calibration                 */
		{CC1101_FSCAL2    ,0x2A},  /* FSCAL2        Frequency Synthesizer Calibration                 */
		{CC1101_FSCAL1    ,0x00},  /* FSCAL1        Frequency Synthesizer Calibration                 */
		{CC1101_FSCAL0    ,0x1F},  /* FSCAL0        Frequency Synthesizer Calibration                 */
		{CC1101_RCCTRL1   ,0x41},  /* RCCTRL1       RC Oscillator Configuration                       */
		{CC1101_RCCTRL0   ,0x00},  /* RCCTRL0       RC Oscillator Configuration                       */
		{CC1101_FSTEST    ,0x59},  /* FSTEST        Frequency Synthesizer Calibration Control         */
		{CC1101_PTEST     ,0x7F},  /* PTEST         Production Test                                   */
		{CC1101_AGCTEST   ,0x3F},  /* AGCTEST       AGC Test                                          */
		{CC1101_TEST2     ,0x88},  /* TEST2         Various Test Settings                             */
		{CC1101_TEST1     ,0x31},  /* TEST1         Various Test Settings                             */
		{CC1101_TEST0     ,0x09}   /* TEST0         Various Test Settings                             */
};




/* PA tables for different frequency bands - Use of which is decided by PER test GUI */
/* NB: If any of the output power tables are changed it must be reflected in per_test_gui.c */
const uint8_t CC1101_315MHz_PowerTable[]=
{
		0x12, /* -30 dBm  - min */
		0x0D, /* -20 dBm  - next lowest power */
		0x1C, /* -15 dBm  */
		0x34, /* -10 dBm  */
		0x51, /*   0 dBm  */
		0x85, /*   5 dBm  */
		0xCB,  /*   7 dBm  - next highest power */
		0xC2, /*  10 dBm  - max */
};

const uint8_t CC1101_434MHz_PowerTable[]=
{
		0x12, /* -30 dBm - min */
		0x0E, /* -20 dBm - next lowest power */
		0x1D, /* -15 dBm */
		0x34, /* -10 dBm */
		0x60, /*   0 dBm */
		0x84, /*   5 dBm */
		0xC8,  /*   7 dBm - next highest power */
		0xC0, /*  10 dBm - max */
};

const uint8_t CC1101_868MHz_PowerTable[]=
{
		0x03, /* -30 dBm  - min */
		0x0F, /* -20 dBm  - next lowest power */
		0x1E, /* -15 dBm  */
		0x27, /* -10 dBm  */
		0x50, /*   0 dBm  */
		0x81, /*   5 dBm  */
		0xCB,  /*   7 dBm  - next highest power */
		0xC2, /*  10 dBm  - max */
};


const uint8_t CC1101_915MHz_PowerTable[]=
{
		0x03, /* -30 dBm - min */
		0x0E, /* -20 dBm - next lowest power */
		0x1E, /* -15 dBm */
		0x27, /* -10 dBm */
		0x8E, /*   0 dBm */
		0xCD, /*   5 dBm */
		0xC7,  /*   7 dBm - next highest power */
		0xC0, /*  10 dBm - max */
};


const registerSetting_t CC11001_frequency_315mhz[]{
		{CC1101_FREQ2,       0x0c},
		{CC1101_FREQ1,       0x1d},
		{CC1101_FREQ0,       0x89},
};
const registerSetting_t CC11001_frequency_434mhz[]{
		{CC1101_FREQ2,       0x10},
		{CC1101_FREQ1,       0xB1},
		{CC1101_FREQ0,       0x3B},
};
const registerSetting_t CC11001_frequency_868mhz[]{
		{CC1101_FREQ2,       0x21},
		{CC1101_FREQ1,       0x62},
		{CC1101_FREQ0,       0x76},
};
const registerSetting_t CC11001_frequency_915mhz[]{
		{CC1101_FREQ2,       0x23},
		{CC1101_FREQ1,       0x31},
		{CC1101_FREQ0,       0x3B},
};







void spidev_cc1101_apply_profile(context_spi_t* spidev_data,cc1101_profile_datarate_t prof_datarate,cc1101_profile_freq_t prof_freq,cc1101_profile_powerlevel_t power_level)
{
	//qualquer configuracao só deve ser feita no modo idle
	spidev_cc1101_cmd(spidev_data,CC1101_SIDLE);


	printf("CC1101 Profile: Apply datarate settings\r\n");
	switch(prof_datarate)
	{
	case CC1101_PROFILE_DATARATE_1200:
		for(uint16_t i = 0; i < (sizeof(CC11001_DataRate_1200)/sizeof(registerSetting_t));i++)
		{
			spidev_cc1101_reg_write(spidev_data,CC11001_DataRate_1200[i].addr,CC11001_DataRate_1200[i].data);
		}
		break;
	case CC1101_PROFILE_DATARATE_38400:
		for(uint16_t i = 0; i < (sizeof(CC11001_DataRate_38400)/sizeof(registerSetting_t));i++)
		{
			spidev_cc1101_reg_write(spidev_data,CC11001_DataRate_38400[i].addr,CC11001_DataRate_38400[i].data);
		}
		break;
	case CC1101_PROFILE_DATARATE_250000:
		for(uint16_t i = 0; i < (sizeof(CC11001_DataRate_250000)/sizeof(registerSetting_t));i++)
		{
			spidev_cc1101_reg_write(spidev_data,CC11001_DataRate_250000[i].addr,CC11001_DataRate_250000[i].data);
		}
		break;
	}


	printf("CC1101 Profile: Apply Frequency settings\r\n");

	switch(prof_freq)
	{
	case CC1101_PROFILE_FREQ_315:
		for(uint16_t i = 0; i < (sizeof (CC11001_frequency_315mhz)/sizeof(registerSetting_t));i++)
		{
			spidev_cc1101_reg_write(spidev_data,CC11001_frequency_315mhz[i].addr,CC11001_frequency_315mhz[i].data);
		}
		break;
	case CC1101_PROFILE_FREQ_433:
		for(uint16_t i = 0; i < (sizeof (CC11001_frequency_434mhz)/sizeof(registerSetting_t));i++)
		{
			spidev_cc1101_reg_write(spidev_data,CC11001_frequency_434mhz[i].addr,CC11001_frequency_434mhz[i].data);
		}
		break;
	case CC1101_PROFILE_FREQ_868:
		for(uint16_t i = 0; i < (sizeof (CC11001_frequency_868mhz)/sizeof(registerSetting_t));i++)
		{
			spidev_cc1101_reg_write(spidev_data,CC11001_frequency_868mhz[i].addr,CC11001_frequency_868mhz[i].data);
		}
		break;
	case CC1101_PROFILE_FREQ_915:
		for(uint16_t i = 0; i < (sizeof (CC11001_frequency_915mhz)/sizeof(registerSetting_t));i++)
		{
			spidev_cc1101_reg_write(spidev_data,CC11001_frequency_915mhz[i].addr,CC11001_frequency_915mhz[i].data);
		}
		break;
	}

	/////////////////////////////////////////////////////////////////////////

	printf("CC1101 Profile: Apply Powerlevel settings\r\n");

	uint8_t reg_level;
	switch(prof_freq)
	{
	case CC1101_PROFILE_FREQ_315:
		reg_level =  CC1101_315MHz_PowerTable[power_level];
		break;
	case CC1101_PROFILE_FREQ_433:
		reg_level =  CC1101_434MHz_PowerTable[power_level];
		break;
	case CC1101_PROFILE_FREQ_868:
		reg_level =  CC1101_868MHz_PowerTable[power_level];
		break;
	case CC1101_PROFILE_FREQ_915:
		reg_level =  CC1101_915MHz_PowerTable[power_level];
		break;
	}

	uint8_t patable[8];
	for(int i=0;i<8;i++) patable[i]=reg_level;
	spidev_cc1101_patable_write(spidev_data,patable);

	//spidev_cc1101_reg_write(spidev_data,CC1101_PA_TABLE0,reg_level);

	/////////////////////////////////////////////////////////////////////////

	printf("CC1101 Profile: Apply Packet automation\r\n");

	//Packet Automation Control
	spidev_cc1101_reg_write(spidev_data,CC1101_PKTCTRL1,SAFE_CC1101_PKTCTRL1);
	spidev_cc1101_reg_write(spidev_data,CC1101_PKTCTRL0,SAFE_CC1101_PKTCTRL0);

	printf("CC1101 Profile: Apply Modem settings\r\n");

	//radio machine state
	spidev_cc1101_reg_write(spidev_data,CC1101_MCSM2,SAFE_CC1101_MCSM2);
	spidev_cc1101_reg_write(spidev_data,CC1101_MCSM1,SAFE_CC1101_MCSM1);
	spidev_cc1101_reg_write(spidev_data,CC1101_MCSM0,SAFE_CC1101_MCSM0);

	printf("CC1101 Profile: Packet length\r\n");

	/* length of configuration packet + filter byte */
	//payload = 61 => 1 byte len + 61 payload + 1 byte rssi + 1 byte = 64 bytes
	spidev_cc1101_reg_write(spidev_data,CC1101_PKTLEN,SAFE_CC1101_PKTLEN);

	return;
}



int8_t spidev_cc1101_read_rssi_in_dbm(context_spi_t* spidev_data)
{

	return spidev_cc1101_convert_rssi_to_dbm(spidev_cc1101_reg_read(spidev_data,CC1101_RSSI));

}

int8_t spidev_cc1101_convert_rssi_to_dbm(uint8_t rawRssi)
{
	int16_t rssiConverted;

	if(rawRssi >= 128)
	{
		rssiConverted = (int16_t)(((int16_t)(rawRssi-256)/2) - CC1101_RSSI_OFFSET);
	}
	else
	{
		rssiConverted = (int16_t)((rawRssi/2) - CC1101_RSSI_OFFSET);
	}
	/* Restricting to 8 bit signed number range */
	if(rssiConverted < -128)
	{
		rssiConverted = -128;
	}
	return (int8_t)rssiConverted;
} 








void spidev_abort(const char *s)
{
	perror(s);
	abort();
}

void spidev_init(context_spi_t* data){

#if 0
	//
	system("echo ADAFRUIT-SPI0 > /sys/devices/bone_capemgr.9/slots");
	//system("echo BB-SPIDEV0 > /sys/devices/bone_capemgr.9/slots");
#endif


	//
	data->device = "/dev/spidev1.0";
	data->fd = open(data->device, O_RDWR);
	if (data->fd < 0)    spidev_abort("can't open device");


	data->mode = 0;
	data->ret = ioctl(data->fd, SPI_IOC_WR_MODE,          &data->mode); if (data->ret == -1) spidev_abort("can't set mode");
	data->ret = ioctl(data->fd, SPI_IOC_RD_MODE,          &data->mode); if (data->ret == -1) spidev_abort("can't get mode");
	printf("spi mode          : %d \r\n", data->mode);

	data->bits = 8;
	data->ret = ioctl(data->fd, SPI_IOC_WR_BITS_PER_WORD, &data->bits); if (data->ret == -1) spidev_abort("can't set bits per word");
	data->ret = ioctl(data->fd, SPI_IOC_RD_BITS_PER_WORD, &data->bits); if (data->ret == -1) spidev_abort("can't get bits per word");
	printf("spi bits per word : %d \r\n", data->bits);

	//cc1101 max speed without delays 6.5 MHz

	data->speed = (6*1000*1000);
	data->ret = ioctl(data->fd, SPI_IOC_WR_MAX_SPEED_HZ, &data->speed); if (data->ret == -1) spidev_abort("can't set max speed hz");
	data->ret = ioctl(data->fd, SPI_IOC_RD_MAX_SPEED_HZ, &data->speed); if (data->ret == -1) spidev_abort("can't get max speed hz");
	printf("spi max speed     : %d Hz\r\n", data->speed);

}


void spidev_transfer(context_spi_t* data)
{

	struct spi_ioc_transfer tr;

	tr.tx_buf = (unsigned long) data->txbuf;
	tr.rx_buf = (unsigned long) data->rxbuf;
	tr.len    = data->len;

	//really black magic
	tr.speed_hz=0;
	tr.bits_per_word=0;
	tr.delay_usecs=0;
	tr.cs_change=1;


	if ( ioctl(data->fd, SPI_IOC_MESSAGE(1), &tr)< 1)
		spidev_abort("can't send spi message");

}

//////////////////////////////////////////////////////////////////////////

uint8_t spidev_cc1101_cmd(context_spi_t* spidev_data,uint8_t cmd){

	//
	spidev_data->txbuf[0]=cmd;
	spidev_data->len=1;

	//
	spidev_transfer(spidev_data);

	printf("CC1101 STROBE %s\r\n",spidev_cc1101_strobe_str(cmd));

	//return status byte
	return spidev_data->rxbuf[0];
}

//////////////////////////////////////////////////////////////////////////

uint8_t spidev_cc1101_reg_write(context_spi_t* spidev_data,uint8_t reg,uint8_t val)
{
	//
	spidev_data->txbuf[0]=reg + CC1101_WRITE_BURST_OFFSET;
	spidev_data->txbuf[1]=val;
	spidev_data->len=2;

	//
	spidev_transfer(spidev_data);

	//
	uint8_t read = spidev_cc1101_reg_read(spidev_data,reg);

	if( read == val){
		printf("reg[%s] write %02x read %02x \033[1;32mok\033[m\r\n",spidev_cc1101_reg_str(reg),val,read);
	}
	else {
		printf("reg[%s] write %02x read %02x \033[1;31merror\033[m\r\n",spidev_cc1101_reg_str(reg),val,read);
	}

	//return status byte
	return spidev_data->rxbuf[0];
}

uint8_t spidev_cc1101_reg_read(context_spi_t* spidev_data,uint8_t reg)
{

	uint8_t val_bak=0xff;
	uint8_t val_now=0xff;
	uint32_t retry=1;
	while(1){
		//
		spidev_data->txbuf[0]=reg + CC1101_READ_BURST_OFFSET;
		spidev_data->txbuf[1]=0x0;
		spidev_data->len=2;

		//
		spidev_transfer(spidev_data);

		val_bak=val_now;
		val_now=spidev_data->rxbuf[1];

		//se termina quando ler duas vezes o mesmo valor,
		//este eh um bug de hardware, veja a errata do cc1101
		//o rssi é o unico aceitavel de fazer leitura unica
		if( (val_bak==val_now && retry>=2) || (reg==CC1101_RSSI)){
			//
			return val_now;
		}
		else {
			if(retry>=2){
				if(reg==CC1101_MARCSTATE){
					printf("spidev_cc1101_reg_read error read reg[%s] back=%s now=%s retry=%d\r\n",spidev_cc1101_reg_str(reg),spidev_cc1101_marc_str(val_bak),spidev_cc1101_marc_str(val_now),retry);
				}
				else {
					printf("spidev_cc1101_reg_read error read reg[%s] back=%02x now=%02x retry=%d\r\n",spidev_cc1101_reg_str(reg),val_bak,val_now,retry);
				}

			}
		}
		//
		retry++;
	}



}

//////////////////////////////////////////////////////////////////////////


uint8_t spidev_cc1101_patable_write(context_spi_t* spidev_data,uint8_t patable[8])
{
	//
	spidev_data->txbuf[0]=CC1101_PATABLE + CC1101_WRITE_BURST_OFFSET;
	spidev_data->txbuf[1]=patable[0];
	spidev_data->txbuf[2]=patable[1];
	spidev_data->txbuf[3]=patable[2];
	spidev_data->txbuf[4]=patable[3];
	spidev_data->txbuf[5]=patable[4];
	spidev_data->txbuf[6]=patable[5];
	spidev_data->txbuf[7]=patable[6];
	spidev_data->txbuf[8]=patable[7];
	spidev_data->len=9;

	//
	spidev_transfer(spidev_data);

	//return status byte
	return spidev_data->rxbuf[0];
}


uint8_t* spidev_cc1101_patable_read(context_spi_t* spidev_data)
{
	spidev_data->txbuf[0]=CC1101_PATABLE + CC1101_READ_BURST_OFFSET;
	spidev_data->txbuf[1]=0x0;
	spidev_data->txbuf[2]=0x0;
	spidev_data->txbuf[3]=0x0;
	spidev_data->txbuf[4]=0x0;
	spidev_data->txbuf[5]=0x0;
	spidev_data->txbuf[6]=0x0;
	spidev_data->txbuf[7]=0x0;
	spidev_data->txbuf[8]=0x0;
	spidev_data->len=9;

	//
	spidev_transfer(spidev_data);

	//return patable array
	return &spidev_data->rxbuf[1];
}


//////////////////////////////////////////////////////////////////////////

uint8_t spidev_cc1101_txfifo_write(context_spi_t* spidev_data,uint8_t packet[64],uint8_t len)
{
	//
	if(len>61){
		len=61;
		printf("warning: spidev_cc1101_txfifo_write len>61 -> cut to 61\r\n");
	}

	//0x3F: Single byte access to TX FIFO
	//0x7F: Burst access to TX FIFO
	//0xBF: Single byte access to RX FIFO
	//0xFF: Burst access to RX FIFO
	//
	spidev_data->txbuf[0]= CC1101_FIFO+CC1101_WRITE_BURST_OFFSET;
	spidev_data->txbuf[1]=len;

	for(int i=0;i<len;i++){
		spidev_data->txbuf[i+2]=packet[i];
	}
	spidev_data->len=2+len;

	printf("CC1101 TX FIFO send [%d]=",spidev_data->len);
	for(int i=0;i<spidev_data->len;i++){
		printf("%02x",spidev_data->txbuf[i]);
	}
	printf("\r\n");

	//
	spidev_transfer(spidev_data);

	printf("CC1101 TX FIFO recv [%d]=",spidev_data->len);
	for(int i=0;i<spidev_data->len;i++){
		printf("%02x",spidev_data->rxbuf[i]);
	}
	printf("\r\n");

	//return chip status
	return spidev_data->rxbuf[0];
}

uint8_t* spidev_cc1101_rxfifo_read(context_spi_t* spidev_data)
{

	//
	spidev_data->txbuf[0]=CC1101_FIFO + CC1101_READ_BURST_OFFSET;
	for(int i=0;i<64;i++){
		spidev_data->txbuf[i+1]=0;
	}
	spidev_data->len=1+64;

	//
	spidev_transfer(spidev_data);

	//return rxfifo array
	return &spidev_data->rxbuf[1];
}


//////////////////////////////////////////////////////////////////////////

const char* spidev_cc1101_strobe_str(int reg){
	switch(reg){

	case CC1101_SRES : return "SRES"; //
	case CC1101_SFSTXON : return "SFSTXON"; //
	case CC1101_SXOFF : return "SXOFF"; //
	case CC1101_SCAL : return "SCAL"; //
	case CC1101_SRX : return "SRX"; //
	case CC1101_STX : return "STX"; //
	case CC1101_SIDLE : return "SIDLE"; //
	case CC1101_SWOR : return "SWOR"; //
	case CC1101_SPWD : return "SPWD"; //
	case CC1101_SFRX : return "SFRX"; //
	case CC1101_SFTX : return "SFTX"; //
	case CC1101_SWORRST : return "SWORRST"; //
	case CC1101_SNOP : return "SNOP"; //

	default: return "UNKNOWN";

	}
}
const char* spidev_cc1101_reg_str(int reg){
	switch(reg){

	case CC1101_IOCFG2         : return "IOCFG2        "; //- GDO2 output pin configuration  */
	case CC1101_IOCFG1         : return "IOCFG1        "; //- GDO1 output pin configuration  */
	case CC1101_IOCFG0         : return "IOCFG0        "; //- GDO0 output pin configuration  */
	case CC1101_FIFOTHR        : return "FIFOTHR       "; //- RX FIFO and TX FIFO thresholds */
	case CC1101_SYNC1          : return "SYNC1         "; //- Sync word, high byte */
	case CC1101_SYNC0          : return "SYNC0         "; //- Sync word, low byte */
	case CC1101_PKTLEN         : return "PKTLEN        "; //- Packet length */
	case CC1101_PKTCTRL1       : return "PKTCTRL1      "; //- Packet automation control */
	case CC1101_PKTCTRL0       : return "PKTCTRL0      "; //- Packet automation control */
	case CC1101_ADDR           : return "ADDR          "; //- Device address */
	case CC1101_CHANNR         : return "CHANNR        "; //- Channel number */
	case CC1101_FSCTRL1        : return "FSCTRL1       "; //- Frequency synthesizer control */
	case CC1101_FSCTRL0        : return "FSCTRL0       "; //- Frequency synthesizer control */
	case CC1101_FREQ2          : return "FREQ2         "; //- Frequency control word, high byte */
	case CC1101_FREQ1          : return "FREQ1         "; //- Frequency control word, middle byte */
	case CC1101_FREQ0          : return "FREQ0         "; //- Frequency control word, low byte */
	case CC1101_MDMCFG4        : return "MDMCFG4       "; //- Modem configuration */
	case CC1101_MDMCFG3        : return "MDMCFG3       "; //- Modem configuration */
	case CC1101_MDMCFG2        : return "MDMCFG2       "; //- Modem configuration */
	case CC1101_MDMCFG1        : return "MDMCFG1       "; //- Modem configuration */
	case CC1101_MDMCFG0        : return "MDMCFG0       "; //- Modem configuration */
	case CC1101_DEVIATN        : return "DEVIATN       "; //- Modem deviation setting */
	case CC1101_MCSM2          : return "MCSM2         "; //- Main Radio Control State Machine configuration */
	case CC1101_MCSM1          : return "MCSM1         "; //- Main Radio Control State Machine configuration */
	case CC1101_MCSM0          : return "MCSM0         "; //- Main Radio Control State Machine configuration */
	case CC1101_FOCCFG         : return "FOCCFG        "; //- Frequency Offset Compensation configuration */
	case CC1101_BSCFG          : return "BSCFG         "; //- Bit Synchronization configuration */
	case CC1101_AGCCTRL2       : return "AGCCTRL2      "; //- AGC control */
	case CC1101_AGCCTRL1       : return "AGCCTRL1      "; //- AGC control */
	case CC1101_AGCCTRL0       : return "AGCCTRL0      "; //- AGC control */
	case CC1101_WOREVT1        : return "WOREVT1       "; //- High byte Event0 timeout */
	case CC1101_WOREVT0        : return "WOREVT0       "; //- Low byte Event0 timeout */
	case CC1101_WORCTRL        : return "WORCTRL       "; //- Wake On Radio control */
	case CC1101_FREND1         : return "FREND1        "; //- Front end RX configuration */
	case CC1101_FREND0         : return "FREDN0        "; //- Front end TX configuration */
	case CC1101_FSCAL3         : return "FSCAL3        "; //- Frequency synthesizer calibration */
	case CC1101_FSCAL2         : return "FSCAL2        "; //- Frequency synthesizer calibration */
	case CC1101_FSCAL1         : return "FSCAL1        "; //- Frequency synthesizer calibration */
	case CC1101_FSCAL0         : return "FSCAL0        "; //- Frequency synthesizer calibration */
	case CC1101_RCCTRL1        : return "RCCTRL1       "; //- RC oscillator configuration */
	case CC1101_RCCTRL0        : return "RCCTRL0       "; //- RC oscillator configuration */
	case CC1101_FSTEST         : return "FSTEST        "; //- Frequency synthesizer calibration control */
	case CC1101_PTEST          : return "PTEST         "; //- Production test */
	case CC1101_AGCTEST        : return "AGCTEST       "; //- AGC test */
	case CC1101_TEST2          : return "TEST2         "; //- Various test settings */
	case CC1101_TEST1          : return "TEST1         "; //- Various test settings */
	case CC1101_TEST0          : return "TEST0         "; //- Various test settings */
	case CC1101_PARTNUM        : return "PARTNUM       "; //  - Chip ID */
	case CC1101_VERSION        : return "VERSION       "; //  - Chip ID */
	case CC1101_FREQEST        : return "FREQEST       "; //  - Frequency Offset Estimate from demodulator */
	case CC1101_LQI            : return "LQI           "; //  - Demodulator estimate for Link Quality */
	case CC1101_RSSI           : return "RSSI          "; //  - Received signal strength indication */
	case CC1101_MARCSTATE      : return "MARCSTATE     "; //  - Main Radio Control State Machine state */
	case CC1101_WORTIME1       : return "WORTIME1      "; //  - High byte of WOR time */
	case CC1101_WORTIME0       : return "WORTIME0      "; //  - Low byte of WOR time */
	case CC1101_PKTSTATUS      : return "PKTSTATUS     "; //  - Current GDOx status and packet status */
	case CC1101_VCO_VC_DAC     : return "VCO_VC_DAC    "; // - Current setting from PLL calibration module */
	case CC1101_TXBYTES        : return "TXBYTES       "; //  - Underflow and number of bytes */
	case CC1101_RXBYTES        : return "RXBYTES       "; //  - overflow and number of bytes */
	case CC1101_RCCTRL1_STATUS : return "RCCTRL1_STATUS"; //
	case CC1101_RCCTRL0_STATUS : return "RCCTRL0_STATUS"; //
	case CC1101_PATABLE        : return "PATABLE       "; //
	case CC1101_FIFO           : return "FIFO          "; //

	default: return "UNKNOWN";
	}

}

const char* spidev_cc1101_marc_str(int state){
	switch(state){

	case CC1101_MARC_SLEEP            : return "SLEEP";
	case CC1101_MARC_IDLE             : return "IDLE";
	case CC1101_MARC_XOFF             : return "XOFF";
	case CC1101_MARC_VCOON_MC         : return "VCOON_MC";
	case CC1101_MARC_REGON_MC         : return "REGON_MC";
	case CC1101_MARC_MANCAL           : return "MANCAL";
	case CC1101_MARC_VCOON            : return "VCOON";
	case CC1101_MARC_REGON            : return "REGON";
	case CC1101_MARC_STARTCAL         : return "STARTCAL";
	case CC1101_MARC_BWBOOST          : return "BWBOOST";
	case CC1101_MARC_FS_LOCK          : return "FS_LOCK";
	case CC1101_MARC_IFADCON          : return "IFADCON";
	case CC1101_MARC_ENDCAL           : return "ENDCAL";
	case CC1101_MARC_RX               : return "RX";
	case CC1101_MARC_RX_END           : return "RX_END";
	case CC1101_MARC_RX_RST           : return "RX_RST";
	case CC1101_MARC_TXRX_SWITCH      : return "TXRX_SWITCH";
	case CC1101_MARC_RXFIFO_OVERFLOW  : return "RXFIFO_OVERFLOW";
	case CC1101_MARC_FSTXON           : return "FSTXON";
	case CC1101_MARC_TX               : return "TX";
	case CC1101_MARC_TX_END           : return "TX_END";
	case CC1101_MARC_RXTX_SWITCH      : return "RXTX_SWITCH";
	case CC1101_MARC_TXFIFO_UNDERFLOW : return "TXFIFO_UNDERFLOW";

	default: return "UNKNOWN";
	}
}

void spidev_cc1101_regdump(context_spi_t* spidev_data){

	//
	printf("-------- RW BYTE Registers ------------\r\n");
	for(int reg=CC1101_IOCFG2;reg<=CC1101_TEST0;reg++){
		printf("RW reg[%s]=%02x\r\n",spidev_cc1101_reg_str(reg),spidev_cc1101_reg_read(spidev_data,reg));
	}


	//
	printf("-------- RO BYTE Registers--------\r\n");
	for(int reg=CC1101_PARTNUM;reg<=CC1101_RCCTRL0_STATUS;reg++){
		printf("RO reg[%s]=%02x\r\n",spidev_cc1101_reg_str(reg),spidev_cc1101_reg_read(spidev_data,reg));
	}

	printf("-------- BURST Registers--------\r\n");
	uint8_t* patable=spidev_cc1101_patable_read(spidev_data);
	printf("PATABLE=");
	for(int i=0;i<8;i++){
		printf("%02x",patable[i]);
	}
	printf("\r\n");

	uint8_t* rxfifo=spidev_cc1101_rxfifo_read(spidev_data);
	printf("RXFIFO=");
	for(int i=0;i<64;i++){
		printf("%02x",rxfifo[i]);
	}
	printf("\r\n");

	//
	printf("--------- RSSI to DBM testing ---------\r\n");
	int8_t rssidbm = spidev_cc1101_read_rssi_in_dbm(spidev_data);
	printf("RSSI = %d dbm\r\n",rssidbm);
}


//////////////////////////////////////////////////////////////////////////

void spidev_cc1101_handle_idle_rx_radio_tx_tap(context_spi_t* spidev_data,int tap_fd){

	//printf("spidev_cc1101_handle_idle_rx_radio_tx_tap\r\n");


	int marc = spidev_cc1101_reg_read(spidev_data,CC1101_MARCSTATE);
	if(marc!=CC1101_MARC_IDLE){
		printf("erro spidev_cc1101_handle_idle_rx_radio_tx_tap so pode ser chamado em idle\r\n");
	}

	//verificar se tem dados na fifo RX do radio
	int rxbytes = spidev_cc1101_reg_read(spidev_data,CC1101_RXBYTES);
	//se tiver
	if(rxbytes>0){

		//entao eh pacote novo
		printf("Radio RX -> pacote novo rxbytes=%d\r\n",rxbytes);

		//ler o conteudo do pacote
		uint8_t* cc1101_rx_packet=spidev_cc1101_rxfifo_read(spidev_data);

		//
		if(rxbytes>64){
			printf("Radio error rxbytes=%d -> corta em 64\r\n",rxbytes);
			rxbytes=64;
		}

		//
		printf("Radio RX packet=[");
		for(int i=0;i<rxbytes;i++) printf("%02x",cc1101_rx_packet[i]);
		printf("]\r\n");

		//
		uint8_t payloadlen=cc1101_rx_packet[0];
		uint8_t* payload=&cc1101_rx_packet[1];
		uint8_t rssi=cc1101_rx_packet[rxbytes-2];
		uint8_t lqicrc=cc1101_rx_packet[rxbytes-1];

		//
		int8_t rssi_dbm=spidev_cc1101_convert_rssi_to_dbm(rssi);
		uint8_t crcok= lqicrc & CC1101_LQI_CRC_OK_BM;
		uint8_t lqiest= lqicrc & CC1101_LQI_EST_BM;

		//
		printf("Radio RX packet=[%d bytes][",payloadlen);
		for(int i=0;i<payloadlen;i++) printf("%02x",payload[i]);
		printf("][rssi %d dbm][crcok=%d lqi=%d]\r\n",rssi_dbm,crcok,lqiest);

		printf("Radio RX packet=[%d bytes][",payloadlen);
		for(int i=0;i<payloadlen;i++) printf("%02d",i+1);
		printf("][rssi %d dbm][crcok=%d lqi=%d]\r\n",rssi_dbm,crcok,lqiest);


		if(rxbytes==payloadlen+3){
			printf("Radio RX -> executa TAP TX\r\n");
			//escreve para o conteudo do pacote excluindo o primeiro byte
			write(tap_fd,payload,payloadlen);
		}

	}
}




int spidev_cc1101_handle_idle_rx_tap_tx_radio(context_spi_t* spidev_data,int tap_fd){

	//printf("spidev_cc1101_handle_idle_rx_tap_tx_radio\r\n");

	uint8_t tap_packetrx[2000];

	fd_set tap_rfds;
	FD_ZERO(&tap_rfds);
	FD_SET(tap_fd, &tap_rfds);

	struct timeval tap_tv;
	tap_tv.tv_sec = 0;
	tap_tv.tv_usec = 1; // 1 us

	int tap_rx_len = 0;

	int tap_retval = select(tap_fd+1, &tap_rfds, NULL, NULL, &tap_tv);
	if (tap_retval == -1){
		perror("select()");
	}
	else if (tap_retval){
		tap_rx_len = read(tap_fd,&tap_packetrx,sizeof(tap_packetrx));

		printf("TAP RX pacote[%d]=",tap_rx_len);
		for(int k=0;k<tap_rx_len;k++)printf("%02x",tap_packetrx[k]);
		printf("\r\n");
	}
	else {
		tap_rx_len=0;
		//printf("RX TAP sem pacotes novos!\r\n");
	}

	//receber pacote do tap e enviar para o Radio
	if(tap_rx_len){
		printf("RX TAP -> executa Radio TX\r\n");


		//coloca o Radio em IDLE
		spidev_cc1101_cmd(spidev_data,CC1101_SIDLE);

		//limpa a fifo TX
		spidev_cc1101_cmd(spidev_data,CC1101_SFRX);
		spidev_cc1101_cmd(spidev_data,CC1101_SFTX);

		int lenclear=spidev_cc1101_reg_read(spidev_data,CC1101_TXBYTES);
		printf("CC1101 FIFO TX len=%d (clear)\r\n",lenclear);
		lenclear=spidev_cc1101_reg_read(spidev_data,CC1101_RXBYTES);
		printf("CC1101 FIFO RX len=%d (clear)\r\n",lenclear);

		//escreve pacote na fifo
		if(tap_rx_len>61){
			printf("pacote cortado de %d para 61 bytes\r\n",tap_rx_len);
			tap_rx_len=61;
		}

		//iniciar transmissao - escrever na TX-FIFO sem dar STX depois buga o radio
		spidev_cc1101_txfifo_write(spidev_data,tap_packetrx,tap_rx_len);

		int lenfill=spidev_cc1101_reg_read(spidev_data,CC1101_TXBYTES);
		printf("CC1101 FIFO TX len=%d (fill)\r\n",lenfill);
		lenfill=spidev_cc1101_reg_read(spidev_data,CC1101_RXBYTES);
		printf("CC1101 FIFO RX len=%d (fill)\r\n",lenfill);


		spidev_cc1101_cmd(spidev_data,CC1101_STX);

		//sair para esperar o fim da transmissao
		return 1;
	}

	return 0;
}





int tap_alloc(char *dev)
{
	struct ifreq ifr;
	int fd, err;

	if( (fd = open("/dev/net/tun", O_RDWR)) < 0 )
		spidev_abort("erro tap");

	memset(&ifr, 0, sizeof(ifr));

	ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
	if( (err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0 ){
		close(fd);
		return err;
	}
	strcpy(dev, ifr.ifr_name);
	return fd;
}

int tap_setup(char* ip){
	char tap_cmd[256];
	char tap_interface[64];

	//alocate and
	int tap_fd = tap_alloc(tap_interface);

	//configure
	sprintf(tap_cmd,"ifconfig %s %s mtu 68",tap_interface,ip);
	printf("%s\r\n",tap_cmd);
	system(tap_cmd);

	//
	return tap_fd;
}

void spidev_cc1101_reset(context_spi_t* spidev_data){


	// blocking function
	while(1){

		// send reset
		printf("SRES cb=%02x\r\n",spidev_cc1101_cmd(spidev_data,CC1101_SRES));

		//try idle for 5 ms
		for(int count_sidle=0;count_sidle<5000;count_sidle++){
			//try idle
			int cb = spidev_cc1101_cmd(spidev_data,CC1101_SIDLE);
			if(cb!=0xff){
				printf("SIDLE count_sidle=%d cb=%02x\r\n",count_sidle,cb);
				return;
			}
			//wait 1us
			usleep(1);
		}

	}

}


void spidev_cc1101_reset_and_configuration(context_spi_t* spidev_data){

	// cc1101 reset
	spidev_cc1101_reset(spidev_data);

	// cc1101 after reset reg dump
	printf("----------- CC1101 Reset regdump -------------\r\n");
	spidev_cc1101_regdump(spidev_data);

	// cc1101 configuration
	spidev_cc1101_apply_profile(spidev_data,CC1101_PROFILE_DATARATE_250000,CC1101_PROFILE_FREQ_433,CC1101_PROFILE_POWERLEVEL_7);

	// cc1101 after configuration reg dump
	printf("----------- CC1101 Profile regdump -------------\r\n");
	spidev_cc1101_regdump(spidev_data);

	//go to idle state
	spidev_cc1101_cmd(spidev_data,CC1101_SIDLE);
}


void spidev_cc1101_run_state_machine(context_spi_t* spidev_data,int tap_fd){

	// cc1101 reset and configuration
	spidev_cc1101_reset_and_configuration(spidev_data);


	while(1){



		//read radio state
		int marc = spidev_cc1101_reg_read(spidev_data,CC1101_MARCSTATE);

		//state machine
		switch(marc){


		case CC1101_MARC_SLEEP:
			//go to idle
			printf("marc=%s\r\n",spidev_cc1101_marc_str(marc));
			spidev_cc1101_cmd(spidev_data,CC1101_SIDLE);
			break;

		case CC1101_MARC_IDLE:
			printf("marc=%s\r\n",spidev_cc1101_marc_str(marc));
			//se estiver idle é porque o tx e rx acabaram

			//primeiro olhar no radio se precisa enviar para o tap
			spidev_cc1101_handle_idle_rx_radio_tx_tap(spidev_data,tap_fd);

			//olhar no tap se tem coisa para enviar no radio
			if( ! spidev_cc1101_handle_idle_rx_tap_tx_radio(spidev_data,tap_fd) ){

				//se nao tiver nada para enviar pelo radio, entao coloca o radio em modo RX

				// limpar fifo RX
				spidev_cc1101_cmd(spidev_data,CC1101_SFTX);
				spidev_cc1101_cmd(spidev_data,CC1101_SFRX);
				// iniciar RX
				spidev_cc1101_cmd(spidev_data,CC1101_SRX);
			}
			break;

		case CC1101_MARC_XOFF:
			//go to idle
			printf("marc=%s\r\n",spidev_cc1101_marc_str(marc));
			spidev_cc1101_cmd(spidev_data,CC1101_SIDLE);
			break;


		case CC1101_MARC_VCOON_MC:
			printf("marc=%s\r\n",spidev_cc1101_marc_str(marc));
			break;


		case CC1101_MARC_REGON_MC:
			printf("marc=%s\r\n",spidev_cc1101_marc_str(marc));
			break;


		case CC1101_MARC_MANCAL:
			printf("marc=%s\r\n",spidev_cc1101_marc_str(marc));
			break;


		case CC1101_MARC_VCOON:
			printf("marc=%s\r\n",spidev_cc1101_marc_str(marc));
			break;


		case CC1101_MARC_REGON:
			printf("marc=%s\r\n",spidev_cc1101_marc_str(marc));
			break;


		case CC1101_MARC_STARTCAL:
			printf("marc=%s\r\n",spidev_cc1101_marc_str(marc));
			break;


		case CC1101_MARC_BWBOOST:
			printf("marc=%s\r\n",spidev_cc1101_marc_str(marc));
			break;


		case CC1101_MARC_FS_LOCK:
			printf("marc=%s\r\n",spidev_cc1101_marc_str(marc));
			break;


		case CC1101_MARC_IFADCON:
			printf("marc=%s\r\n",spidev_cc1101_marc_str(marc));
			break;


		case CC1101_MARC_ENDCAL:
			printf("marc=%s\r\n",spidev_cc1101_marc_str(marc));
			break;


		case CC1101_MARC_RX:
		{
			//int8_t rssidbm = spidev_cc1101_read_rssi_in_dbm(spidev_data);
			//printf("marc=%s RSSI = %d dbm\r\n",spidev_cc1101_marc_str(marc),rssidbm);

			//durante o radio RX verificar se tem algo do TAP para enviar no radio
			spidev_cc1101_handle_idle_rx_tap_tx_radio(spidev_data,tap_fd);
		}
		break;

		case CC1101_MARC_RX_END:
			printf("marc=%s\r\n",spidev_cc1101_marc_str(marc));
			break;


		case CC1101_MARC_RX_RST:
			printf("marc=%s\r\n",spidev_cc1101_marc_str(marc));
			break;


		case CC1101_MARC_TXRX_SWITCH:
			printf("marc=%s\r\n",spidev_cc1101_marc_str(marc));
			break;

		case CC1101_MARC_RXFIFO_OVERFLOW:
			printf("marc=%s\r\n",spidev_cc1101_marc_str(marc));
			break;


		case CC1101_MARC_FSTXON:
			printf("marc=%s\r\n",spidev_cc1101_marc_str(marc));
			break;

		case CC1101_MARC_TX:
		{
			//deve ir sozinho para TX_END
			int len=spidev_cc1101_reg_read(spidev_data,CC1101_TXBYTES);
			printf("marc=%s txlen=%d\r\n",spidev_cc1101_marc_str(marc),len);

			//if(len==0){
			// spidev_cc1101_cmd(spidev_data,CC1101_SIDLE);
			//}
		}
		break;

		case CC1101_MARC_TX_END:
			//deve voltar sozinho para idle
			printf("marc=%s\r\n",spidev_cc1101_marc_str(marc));
			break;


		case CC1101_MARC_RXTX_SWITCH:
			printf("marc=%s\r\n",spidev_cc1101_marc_str(marc));
			break;


		case CC1101_MARC_TXFIFO_UNDERFLOW:
			printf("marc=%s\r\n",spidev_cc1101_marc_str(marc));
			break;

		default:
			printf("marc=%s\r\n",spidev_cc1101_marc_str(marc));
			spidev_cc1101_reset_and_configuration(spidev_data);
			break;
		}


	}

}

int main(int argc,char** argv){

	if(argc!=2){
		printf("./%s <ip>\r\n",argv[0]);
		exit(1);
	}

	//open tap
	int tap_fd = tap_setup(argv[1]);

	//spi data
	context_spi_t spidev_data;

	//init spi
	spidev_init(&spidev_data);

	//radio state machine
	spidev_cc1101_run_state_machine(&spidev_data,tap_fd);


	return 0;
}


