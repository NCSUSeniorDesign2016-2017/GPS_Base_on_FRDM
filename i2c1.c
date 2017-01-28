#include <MKL25Z4.H>
#include "i2c.h"

int lock_detect1=0;
int i2c_lock1=0;

#define ENABLE_I2C_RESET 1

//init i2c1
void i2c1_init(void)
{
	//clock i2c peripheral and port E
	SIM->SCGC4 |= SIM_SCGC4_I2C1_MASK;
	SIM->SCGC5 |= (SIM_SCGC5_PORTE_MASK);
	
	//set pins to I2C function
	PORTE->PCR[1] |= PORT_PCR_MUX(6);		// SCL
	PORTE->PCR[0] |= PORT_PCR_MUX(6);		// SDA
		
	//set to 100k baud
	//baud = bus freq/(scl_div+mul)
 	//~400k = 24M/(64); icr=0x12 sets scl_div to 64

 	I2C1->F = (I2C_F_ICR(0x00) | I2C_F_MULT(0));
		
	//enable i2c and set to master mode
	I2C1->C1 |= (I2C_C1_IICEN_MASK);
	
	// Select high drive mode
	I2C1->C2 |= (I2C_C2_HDRS_MASK);
}

void i2c1_busy(void){
	// Start Signal
	lock_detect1=0;
	I2C1->C1 &= ~I2C_C1_IICEN_MASK;
	I2C1_TRAN;
	I2C1_M_START;
	I2C1->C1 |=  I2C_C1_IICEN_MASK;
	// Write to clear line
	I2C1->C1 |= I2C_C1_MST_MASK; /* set MASTER mode */  
	I2C1->C1 |= I2C_C1_TX_MASK; /* Set transmit (TX) mode */  
	I2C1->D = 0xFF;
	while ((I2C1->S & I2C_S_IICIF_MASK) == 0U) {
	} /* wait interrupt */  
	I2C1->S |= I2C_S_IICIF_MASK; /* clear interrupt bit */  
		
		
							/* Clear arbitration error flag*/  
	I2C1->S |= I2C_S_ARBL_MASK;
		
		
							/* Send start */  
	I2C1->C1 &= ~I2C_C1_IICEN_MASK;
	I2C1->C1 |= I2C_C1_TX_MASK; /* Set transmit (TX) mode */  
	I2C1->C1 |= I2C_C1_MST_MASK; /* START signal generated */  
		
	I2C1->C1 |= I2C_C1_IICEN_MASK;
							/*Wait until start is send*/  

							/* Send stop */  
	I2C1->C1 &= ~I2C_C1_IICEN_MASK;
	I2C1->C1 |= I2C_C1_MST_MASK;
	I2C1->C1 &= ~I2C_C1_MST_MASK; /* set SLAVE mode */  
	I2C1->C1 &= ~I2C_C1_TX_MASK; /* Set Rx */  
	I2C1->C1 |= I2C_C1_IICEN_MASK;
		
	
								/* wait */  
							/* Clear arbitration error & interrupt flag*/  
	I2C1->S |= I2C_S_IICIF_MASK;
	I2C1->S |= I2C_S_ARBL_MASK;
	lock_detect1=0;
	i2c_lock1=1;
}


#ifdef ENABLE_I2C_RESET
#pragma no_inline 
void i2c1_wait(void) {
	while(((I2C1->S & I2C_S_IICIF_MASK)==0) & (lock_detect1 < 200)) {
		lock_detect1++;
	} 
	if (lock_detect1 >= 200)
		i2c1_busy();
	
  I2C1->S |= I2C_S_IICIF_MASK;
}
#else
#pragma no_inline 
void i2c1_wait(void) {
	while((I2C1->S & I2C_S_IICIF_MASK)==0) {
		} 
  I2C1->S |= I2C_S_IICIF_MASK;
}
#endif

//send start sequence
void i2c1_start()
{
	I2C1_TRAN;							/*set to transmit mode */
	I2C1_M_START;					/*send start	*/
}

//send device and register addresses
#pragma no_inline 
void i2c1_read_setup(uint8_t dev, uint8_t address)
{
	I2C1->D = dev;			  /*send dev address	*/
	I2C1_WAIT							/*wait for completion */
	
	I2C1->D =  address;		/*send read address	*/
	I2C1_WAIT							/*wait for completion */
		
	I2C1_M_RSTART;				   /*repeated start */
	I2C1->D = (dev|0x1);	 /*send dev address (read)	*/
	I2C1_WAIT							 /*wait for completion */
	
	I2C1_REC;						   /*set to receive mode */

}

//read a byte and ack/nack as appropriate
// #pragma no_inline 
uint8_t i2c1_repeated_read(uint8_t isLastRead)
{
	uint8_t data;
	
	lock_detect1 = 0;
	
	if(isLastRead)	{
		NACK_1;								/*set NACK after last read	*/
	} else	{
		ACK_1;								/*ACK after read	*/
	}
	
	data = I2C1->D;				/*dummy read	*/
	I2C1_WAIT							/*wait for completion */
	
	if(isLastRead)	{
		I2C1_M_STOP;					/*send stop	*/
	}
	data = I2C1->D;				/*read data	*/

	return  data;					
}



//////////funcs for reading and writing a single byte
//using 7bit addressing reads a byte from dev:address
// #pragma no_inline 
uint8_t i2c1_read_byte(uint8_t dev, uint8_t address)
{
	uint8_t data;
	
	I2C1_TRAN;							/*set to transmit mode */
	I2C1_M_START;					/*send start	*/
	I2C1->D = dev;			  /*send dev address	*/
	I2C1_WAIT							/*wait for completion */
	
	I2C1->D =  address;		/*send read address	*/
	I2C1_WAIT							/*wait for completion */
		
	I2C1_M_RSTART;				   /*repeated start */
	I2C1->D = (dev|0x1);	 /*send dev address (read)	*/
	I2C1_WAIT							 /*wait for completion */
	
	I2C1_REC;						   /*set to recieve mode */
	NACK_1;									 /*set NACK after read	*/
	
	data = I2C1->D;					/*dummy read	*/
	I2C1_WAIT								/*wait for completion */
	
	I2C1_M_STOP;							/*send stop	*/
	data = I2C1->D;					/*read data	*/

	return data;
}



//using 7bit addressing writes a byte data to dev:address
#pragma no_inline 
void i2c1_write_byte(uint8_t dev, uint8_t address, uint8_t data)
{
	
	I2C1_TRAN;							/*set to transmit mode */
	I2C1_M_START;					/*send start	*/
	I2C1->D = dev;			  /*send dev address	*/
	I2C1_WAIT						  /*wait for ack */
	
	I2C1->D =  address;		/*send write address	*/
	I2C1_WAIT
		
	I2C1->D = data;				/*send data	*/
	I2C1_WAIT
	I2C1_M_STOP;
	
}
