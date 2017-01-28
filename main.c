/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#include <MKL25Z4.H>
#include <stdio.h>
#include "gpio_defs.h"
#include "UART.h"
#include "LEDs.h"
#include "timers.h"		
#include "gps.h"
#include "i2c.h"
//#include "MMA8451.h"			// accelerometer
#include "HMC5883L.h"

extern Q_T RxQ, TxQ;
volatile extern uint8_t message_received;
volatile extern uint8_t GPS_message_received;
uint8_t input_message[Q_SIZE] ; 
extern struct GPS_Message GPS_Message;

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main(void) { 
	int i, index= ZERO;
	uint8_t copy_message = FALSE;
	Init_UART0(BAUD_RATE);			//communication to terminal
	Init_UART2(GPS_BAUD_RATE);			//communication with GPS
	Init_RGB_LEDs();
	//i2c0_init();											// Port E 24 (SCL) and 25 (SDA)		//accelerometer
	i2c1_init();
	init_compass();
	RxQ.Head++; 
	printf("Brandon Wiseman bmwisema\r\n");		
	NVIC_ClearPendingIRQ(UART0_IRQn);
	NVIC_EnableIRQ(UART0_IRQn); 
	
	// init GPIO
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	PORTB->PCR[RIGHT_ANTENNA_POS] &= ~PORT_PCR_MUX_MASK;		// left antenna 
	PORTB->PCR[RIGHT_ANTENNA_POS] |= PORT_PCR_MUX(1);
	PORTB->PCR[LEFT_ANTENNA_POS]	&= ~PORT_PCR_MUX_MASK;		// right antenna
	PORTB->PCR[LEFT_ANTENNA_POS] |= PORT_PCR_MUX(1);
	PTB->PDDR |= MASK(RIGHT_ANTENNA_POS) | MASK(LEFT_ANTENNA_POS);					// set to outputs
	
 while (TRUE) {
		if(GPS_message_received){	
				//Control_RGB_LEDs(0,1,0);			// flash green when GPS received
		 for(i=ZERO; i<Get_Num_Rx_Chars_Available(); i++){
			 if(RxQ.Data[i] == ASCII_$){
				 copy_message = TRUE;
			 }
			 if(copy_message){
				input_message[index] = RxQ.Data[i]; 	//copy que to array
				index++;
			 }
			 RxQ.Data[i] = ZERO;			//empty que
		 }
		 //Send_String(input_message);
		 Q_Init(&RxQ);
		 GPS_message_received = FALSE;
		 copy_message = FALSE;
		 index=ZERO;
		 Task_NMEA_Decode();
		 //Control_RGB_LEDs(0,0,0);		// turn off LED after sending message
	 }
		Task_Report_Drift();		//report latitude and longitude
	 if(GPS_Message.new){
		 if(GPS_Message.Track_angle_true > 225 && GPS_Message.Track_angle_true < 315){
				Control_RGB_LEDs(0,1,0);	// GREEN
				PTB->PSOR = MASK(RIGHT_ANTENNA_POS);		// right_antenna off
				PTB->PCOR	=	MASK(LEFT_ANTENNA_POS);			// LEFT_ANTENNA on
		 }
		 else if(GPS_Message.Track_angle_true > 315 && GPS_Message.Track_angle_true < 360){
			 Control_RGB_LEDs(1,0,0);		// RED
			 PTB->PSOR	=	MASK(LEFT_ANTENNA_POS);		// LEFT_ANTENNA off
			 PTB->PCOR	=	MASK(RIGHT_ANTENNA_POS);	// right antenna on
		 }
		 else {Control_RGB_LEDs(0,0,0);}
		 GPS_Message.new = FALSE;
	 }
	 read_full_compass();
 }
} 
// *******************************ARM University Program Copyright © ARM Ltd 2013************************************* 
