

#ifndef CC1101_LINUX_H
#define CC1101_LINUX_H

#ifdef __cplusplus
extern "C" {
#endif


//
#include <stdint.h>
  
///////////////////////////////////////////////////////////////////////////////////

// Register adresses
  
#define CC1101_IOCFG2      0x00      /*  IOCFG2   - GDO2 output pin configuration  */
#define CC1101_IOCFG1      0x01      /*  IOCFG1   - GDO1 output pin configuration  */
#define CC1101_IOCFG0      0x02      /*  IOCFG1   - GDO0 output pin configuration  */
#define CC1101_FIFOTHR     0x03      /*  FIFOTHR  - RX FIFO and TX FIFO thresholds */
#define CC1101_SYNC1       0x04      /*  SYNC1    - Sync word, high byte */
#define CC1101_SYNC0       0x05      /*  SYNC0    - Sync word, low byte */
#define CC1101_PKTLEN      0x06      /*  PKTLEN   - Packet length */
#define CC1101_PKTCTRL1    0x07      /*  PKTCTRL1 - Packet automation control */
#define CC1101_PKTCTRL0    0x08      /*  PKTCTRL0 - Packet automation control */
#define CC1101_ADDR        0x09      /*  ADDR     - Device address */
#define CC1101_CHANNR      0x0A      /*  CHANNR   - Channel number */
#define CC1101_FSCTRL1     0x0B      /*  FSCTRL1  - Frequency synthesizer control */
#define CC1101_FSCTRL0     0x0C      /*  FSCTRL0  - Frequency synthesizer control */
#define CC1101_FREQ2       0x0D      /*  FREQ2    - Frequency control word, high byte */
#define CC1101_FREQ1       0x0E      /*  FREQ1    - Frequency control word, middle byte */
#define CC1101_FREQ0       0x0F      /*  FREQ0    - Frequency control word, low byte */
#define CC1101_MDMCFG4     0x10      /*  MDMCFG4  - Modem configuration */
#define CC1101_MDMCFG3     0x11      /*  MDMCFG3  - Modem configuration */
#define CC1101_MDMCFG2     0x12      /*  MDMCFG2  - Modem configuration */
#define CC1101_MDMCFG1     0x13      /*  MDMCFG1  - Modem configuration */
#define CC1101_MDMCFG0     0x14      /*  MDMCFG0  - Modem configuration */
#define CC1101_DEVIATN     0x15      /*  DEVIATN  - Modem deviation setting */
#define CC1101_MCSM2       0x16      /*  MCSM2    - Main Radio Control State Machine configuration */
#define CC1101_MCSM1       0x17      /*  MCSM1    - Main Radio Control State Machine configuration */
#define CC1101_MCSM0       0x18      /*  MCSM0    - Main Radio Control State Machine configuration */
#define CC1101_FOCCFG      0x19      /*  FOCCFG   - Frequency Offset Compensation configuration */
#define CC1101_BSCFG       0x1A      /*  BSCFG    - Bit Synchronization configuration */
#define CC1101_AGCCTRL2    0x1B      /*  AGCCTRL2 - AGC control */
#define CC1101_AGCCTRL1    0x1C      /*  AGCCTRL1 - AGC control */
#define CC1101_AGCCTRL0    0x1D      /*  AGCCTRL0 - AGC control */
#define CC1101_WOREVT1     0x1E      /*  WOREVT1  - High byte Event0 timeout */
#define CC1101_WOREVT0     0x1F      /*  WOREVT0  - Low byte Event0 timeout */
#define CC1101_WORCTRL     0x20      /*  WORCTRL  - Wake On Radio control */
#define CC1101_FREND1      0x21      /*  FREND1   - Front end RX configuration */
#define CC1101_FREND0      0x22      /*  FREDN0   - Front end TX configuration */
#define CC1101_FSCAL3      0x23      /*  FSCAL3   - Frequency synthesizer calibration */
#define CC1101_FSCAL2      0x24      /*  FSCAL2   - Frequency synthesizer calibration */
#define CC1101_FSCAL1      0x25      /*  FSCAL1   - Frequency synthesizer calibration */
#define CC1101_FSCAL0      0x26      /*  FSCAL0   - Frequency synthesizer calibration */
#define CC1101_RCCTRL1     0x27      /*  RCCTRL1  - RC oscillator configuration */
#define CC1101_RCCTRL0     0x28      /*  RCCTRL0  - RC oscillator configuration */
#define CC1101_FSTEST      0x29      /*  FSTEST   - Frequency synthesizer calibration control */
#define CC1101_PTEST       0x2A      /*  PTEST    - Production test */
#define CC1101_AGCTEST     0x2B      /*  AGCTEST  - AGC test */
#define CC1101_TEST2       0x2C      /*  TEST2    - Various test settings */
#define CC1101_TEST1       0x2D      /*  TEST1    - Various test settings */
#define CC1101_TEST0       0x2E      /*  TEST0    - Various test settings */

// status registers
#define CC1101_PARTNUM     0x30      /*  PARTNUM    - Chip ID */
#define CC1101_VERSION     0x31      /*  VERSION    - Chip ID */
#define CC1101_FREQEST     0x32      /*  FREQEST    - Frequency Offset Estimate from demodulator */
#define CC1101_LQI         0x33      /*  LQI        - Demodulator estimate for Link Quality */
#define CC1101_RSSI        0x34      /*  RSSI       - Received signal strength indication */
#define CC1101_MARCSTATE   0x35      /*  MARCSTATE  - Main Radio Control State Machine state */
#define CC1101_WORTIME1    0x36      /*  WORTIME1   - High byte of WOR time */
#define CC1101_WORTIME0    0x37      /*  WORTIME0   - Low byte of WOR time */
#define CC1101_PKTSTATUS   0x38      /*  PKTSTATUS  - Current GDOx status and packet status */
#define CC1101_VCO_VC_DAC  0x39      /*  VCO_VC_DAC - Current setting from PLL calibration module */
#define CC1101_TXBYTES     0x3A      /*  TXBYTES    - Underflow and number of bytes */
#define CC1101_RXBYTES     0x3B      /*  RXBYTES    - Overflow and number of bytes */
#define CC1101_RCCTRL1_STATUS 0x3C      /*  RCCTRL1_STATUS - Last RC oscillator calibration result  */
#define CC1101_RCCTRL0_STATUS 0x3D      /*  RCCTRL0_STATUS - Last RC oscillator calibration result  */

// burst write registers
#define CC1101_PATABLE     0x3E      /*  PA_TABLE0 - PA control settings table */
#define CC1101_FIFO        0x3F      /*  TXFIFO  - Transmit FIFO */


///////////////////////////////////////////////////////////////////////////////////
// Command strobe registers
#define CC1101_SRES                     0x30      /*  SRES    - Reset chip. */
#define CC1101_SFSTXON                  0x31      /*  SFSTXON - Enable and calibrate frequency synthesizer. */
#define CC1101_SXOFF                    0x32      /*  SXOFF   - Turn off crystal oscillator. */
#define CC1101_SCAL                     0x33      /*  SCAL    - Calibrate frequency synthesizer and turn it off. */
#define CC1101_SRX                      0x34      /*  SRX     - Enable RX. Perform calibration if enabled. */
#define CC1101_STX                      0x35      /*  STX     - Enable TX. If in RX state, only enable TX if CCA passes. */
#define CC1101_SIDLE                    0x36      /*  SIDLE   - Exit RX / TX, turn off frequency synthesizer. */
#define CC1101_SWOR                     0x38      /*  SWOR    - Start automatic RX polling sequence (Wake-on-Radio) */
#define CC1101_SPWD                     0x39      /*  SPWD    - Enter power down mode when CSn goes high. */
#define CC1101_SFRX                     0x3A      /*  SFRX    - Flush the RX FIFO buffer. */
#define CC1101_SFTX                     0x3B      /*  SFTX    - Flush the TX FIFO buffer. */
#define CC1101_SWORRST                  0x3C      /*  SWORRST - Reset real time clock. */
#define CC1101_SNOP                     0x3D      /*  SNOP    - No operation. Returns status byte. */

///////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////
// radio state machine
  
#define CC1101_MARC_SLEEP                0x00             
#define CC1101_MARC_IDLE                 0x01              
#define CC1101_MARC_XOFF                 0x02              
#define CC1101_MARC_VCOON_MC             0x03          
#define CC1101_MARC_REGON_MC             0x04          
#define CC1101_MARC_MANCAL               0x05            
#define CC1101_MARC_VCOON                0x06             
#define CC1101_MARC_REGON                0x07             
#define CC1101_MARC_STARTCAL             0x08          
#define CC1101_MARC_BWBOOST              0x09           
#define CC1101_MARC_FS_LOCK              0x0A           
#define CC1101_MARC_IFADCON              0x0B           
#define CC1101_MARC_ENDCAL               0x0C            
#define CC1101_MARC_RX                   0x0D                
#define CC1101_MARC_RX_END               0x0E            
#define CC1101_MARC_RX_RST               0x0F            
#define CC1101_MARC_TXRX_SWITCH          0x10       
#define CC1101_MARC_RXFIFO_OVERFLOW      0x11   
#define CC1101_MARC_FSTXON               0x12            
#define CC1101_MARC_TX                   0x13                
#define CC1101_MARC_TX_END               0x14            
#define CC1101_MARC_RXTX_SWITCH          0x15       
#define CC1101_MARC_TXFIFO_UNDERFLOW     0x16



/////////////////////////////////////////////////////

// registers offsets for burst access
#define CC1101_WRITE_BYTE_OFFSET   0x0
#define CC1101_WRITE_BURST_OFFSET  0x40
#define CC1101_READ_BYTE_OFFSET    0x80
#define CC1101_READ_BURST_OFFSET   0xc0

//
#define CC1101_LQI_CRC_OK_BM 0x80  /* */
#define CC1101_LQI_EST_BM    0x7F  /* */

//
#define CC1101_RSSI_OFFSET   74   /* rssi offset */

//
#define SAFE_CC1101_PKTLEN   61   /* max safe length */

//
#define SAFE_CC1101_MCSM2    0x07 /*  RX Time disabled - no termination / Disable low quality drop */
#define SAFE_CC1101_MCSM1    0x00 /* CCA disabled, Idle after RX, Idle after TX */
#define SAFE_CC1101_MCSM0    0x1c /* Calibrate from IDLE to RX/TX, PO_TIMEOUT max settings */

//
#define SAFE_CC1101_PKTCTRL1 0x04 /* PQT=0, CRC_AUTOFLUSH=off, APPEND_STATUS=on, ADR_CHK[1:0]=off  */
#define SAFE_CC1101_PKTCTRL0 0x05 /* white=off fifo=on crc=on packet=variable  */

/////////////////////////////////////////////////////

typedef struct
{
  uint16_t  addr;
  uint8_t   data;
} registerSetting_t;

/////////////////////////////////////////////////////

typedef enum {
   CC1101_PROFILE_DATARATE_1200,
   CC1101_PROFILE_DATARATE_38400,
   CC1101_PROFILE_DATARATE_250000,

} cc1101_profile_datarate_t;

typedef enum {
   CC1101_PROFILE_FREQ_315,
   CC1101_PROFILE_FREQ_433,
   CC1101_PROFILE_FREQ_868,
   CC1101_PROFILE_FREQ_915,
} cc1101_profile_freq_t;

typedef enum {
   CC1101_PROFILE_POWERLEVEL_0,
   CC1101_PROFILE_POWERLEVEL_1,
   CC1101_PROFILE_POWERLEVEL_2,
   CC1101_PROFILE_POWERLEVEL_3,
   CC1101_PROFILE_POWERLEVEL_4,
   CC1101_PROFILE_POWERLEVEL_5,
   CC1101_PROFILE_POWERLEVEL_6,
   CC1101_PROFILE_POWERLEVEL_7,
} cc1101_profile_powerlevel_t;




/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

typedef struct  {
    int fd;
    int ret;
	const char *device;
	uint8_t  bits;
	uint32_t speed;
	uint32_t  mode;
    int      delay;
    int      cs_change;
    uint8_t txbuf[256];
	uint8_t rxbuf[256];
	uint8_t len;
} context_spi_t;

void spidev_abort(const char *s);
void spidev_init(context_spi_t* data);
void spidev_transfer(context_spi_t* data);
void spidev_abort(const char *s);

uint8_t     spidev_cc1101_cmd(context_spi_t* data,uint8_t cmd);
uint8_t     spidev_cc1101_reg_write(context_spi_t* spidev_data,uint8_t reg,uint8_t val);
uint8_t     spidev_cc1101_reg_read(context_spi_t* spidev_data,uint8_t reg);
uint8_t     spidev_cc1101_patable_write(context_spi_t* spidev_data,uint8_t patable[8]);
uint8_t*    spidev_cc1101_patable_read(context_spi_t* spidev_data,uint8_t patable[8]);
uint8_t     spidev_cc1101_txfifo_write(context_spi_t* spidev_data,uint8_t packet[64],uint8_t len);
uint8_t*    spidev_cc1101_rxfifo_read(context_spi_t* spidev_data,uint8_t packet[64]);
void        spidev_cc1101_apply_profile(context_spi_t* spidev_data,cc1101_profile_datarate_t prof_datarate,cc1101_profile_freq_t prof_freq,cc1101_profile_powerlevel_t power_level);
const char* spidev_cc1101_marc_str(int state);
const char* spidev_cc1101_reg_str(int reg);
const char* spidev_cc1101_strobe_str(int reg);

int8_t      spidev_cc1101_convert_rssi_to_dbm(uint8_t rawRssi);
int8_t      spidev_cc1101_read_rssi_in_dbm(context_spi_t* spidev_data);


/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////


#ifdef  __cplusplus
}
#endif

#endif// CC1101_SPI_H
