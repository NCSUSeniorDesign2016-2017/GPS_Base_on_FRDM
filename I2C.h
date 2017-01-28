#include <stdint.h>

#define I2C_M_START 	I2C0->C1 |= I2C_C1_MST_MASK
#define I2C_M_STOP  	I2C0->C1 &= ~I2C_C1_MST_MASK
#define I2C_M_RSTART 	I2C0->C1 |= I2C_C1_RSTA_MASK

#define I2C_TRAN			I2C0->C1 |= I2C_C1_TX_MASK
#define I2C_REC				I2C0->C1 &= ~I2C_C1_TX_MASK

#define BUSY_ACK 	    while(I2C0->S & 0x01)
#define TRANS_COMP		while(!(I2C0->S & 0x80))
// #define I2C_WAIT			while((I2C0->S & I2C_S_IICIF_MASK)==0) {} \
                                 I2C0->S |= I2C_S_IICIF_MASK;
// new below
#define I2C1_M_START 	I2C1->C1 |= I2C_C1_MST_MASK
#define I2C1_M_STOP  	I2C1->C1 &= ~I2C_C1_MST_MASK
#define I2C1_M_RSTART 	I2C1->C1 |= I2C_C1_RSTA_MASK

#define I2C1_TRAN			I2C1->C1 |= I2C_C1_TX_MASK
#define I2C1_REC				I2C1->C1 &= ~I2C_C1_TX_MASK

#define BUSY1_ACK 	    while(I2C1->S & 0x01)
#define TRANS1_COMP		while(!(I2C1->S & 0x80))
// new above

#define I2C_WAIT 			i2c_wait();
#define I2C1_WAIT			i2c1_wait();

#define NACK 	        I2C0->C1 |= I2C_C1_TXAK_MASK
#define ACK           I2C0->C1 &= ~I2C_C1_TXAK_MASK

#define NACK_1 	        I2C1->C1 |= I2C_C1_TXAK_MASK
#define ACK_1           I2C1->C1 &= ~I2C_C1_TXAK_MASK

void i2c0_init(void);
void i2c1_init(void);

void i2c_start(void);
void i2c1_start(void);
void i2c_read_setup(uint8_t dev, uint8_t address);
void i2c1_read_setup(uint8_t dev, uint8_t address);
uint8_t i2c_repeated_read(uint8_t);
uint8_t i2c1_repeated_read(uint8_t);
	
uint8_t i2c_read_byte(uint8_t dev, uint8_t address);
void i2c_write_byte(uint8_t dev, uint8_t address, uint8_t data);

uint8_t i2c1_read_byte(uint8_t dev, uint8_t address);
void i2c1_write_byte(uint8_t dev, uint8_t address, uint8_t data);
