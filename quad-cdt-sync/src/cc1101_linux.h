

#ifndef CC1101_LINUX_H
#define CC1101_LINUX_H

#ifdef __cplusplus
extern "C" {
#endif

//
#include <stdint.h>

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

void        spidev_cc1101_worker(char* ip);


/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////


#ifdef  __cplusplus
}
#endif

#endif// CC1101_SPI_H
