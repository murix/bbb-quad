/*
 * i2c_bitbang.h
 *
 *  Created on: 18/02/2015
 *      Author: murix
 */

#ifndef I2C_BITBANG_H_
#define I2C_BITBANG_H_



// Hardware-specific support functions that MUST be customized:
#define I2CSPEED 100
void I2C_delay(void);// { volatile int v; int i; for (i=0; i < I2CSPEED/2; i++) v; }
bool read_SCL(void); // Set SCL as input and return current level of line, 0 or 1
bool read_SDA(void); // Set SDA as input and return current level of line, 0 or 1
void clear_SCL(void); // Actively drive SCL signal low
void clear_SDA(void); // Actively drive SDA signal low
void arbitration_lost(void);


void i2c_start_cond(void);

void i2c_stop_cond(void);

// Write a bit to I2C bus
void i2c_write_bit(bool bit);

// Read a bit from I2C bus
bool i2c_read_bit(void);

// Write a byte to I2C bus. Return 0 if ack by the slave.
bool i2c_write_byte(bool send_start, bool send_stop, unsigned char byte);

// Read a byte from I2C bus
unsigned char i2c_read_byte(bool nack, bool send_stop);




#endif /* I2C_BITBANG_H_ */
