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
#include "iot.h"

extern Q_T RxQ, TxQ;
volatile extern uint8_t message_received;
volatile extern uint8_t GPS_message_received;
volatile extern uint8_t iot_message_received;
uint8_t input_message[Q_SIZE] ; 
uint8_t terminal_message[Q_SIZE];
uint8_t iot_message[Q_SIZE];
extern struct GPS_Message GPS_Message;
int done = FALSE;

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main(void) { 
	int i, k, index= ZERO, terminal_index = ZERO, iot_index = ZERO;
	uint8_t copy_message = FALSE;
	Init_UART0(BAUD_RATE);							//communication to terminal (backdoor through open SDA)
	//Init_UART1(BAUD_RATE);						//communication to IoT device
	Init_UART2(GPS_BAUD_RATE);					//communication with GPS
	Init_RGB_LEDs();
	//i2c0_init();											// Port E 24 (SCL) and 25 (SDA)		//accelerometer
	//i2c1_init();
	//init_compass();
	RxQ.Head++; 
	printf("United States Air Force\r\n");		
	NVIC_ClearPendingIRQ(UART0_IRQn);
	NVIC_EnableIRQ(UART0_IRQn); 
	
	// init GPIO
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	PORTB->PCR[RIGHT_ANTENNA_POS] &= ~PORT_PCR_MUX_MASK;		// left antenna 
	PORTB->PCR[RIGHT_ANTENNA_POS] |= PORT_PCR_MUX(1);
	PORTB->PCR[LEFT_ANTENNA_POS]	&= ~PORT_PCR_MUX_MASK;		// right antenna
	PORTB->PCR[LEFT_ANTENNA_POS] |= PORT_PCR_MUX(1);
	PTB->PDDR |= MASK(RIGHT_ANTENNA_POS) | MASK(LEFT_ANTENNA_POS);					// set to outputs
	
	PORTB->PCR[8] &=	~PORT_PCR_MUX_MASK;
	PORTB->PCR[8] |=	PORT_PCR_MUX(1);
	PORTB_PCR8 	|= 	0x03;															// pull up resistor
	PTB->PDDR &= ~MASK(8);
	
	PTB->PCOR	=	MASK(RIGHT_ANTENNA_POS);	// right antenna on
	//Init_iot();
	Control_RGB_LEDs(1,1,1);
while (TRUE) {
		if(message_received) {
			for(i=ZERO; i<Get_Num_Rx_Chars_Available(); i++){
				terminal_message[terminal_index] = RxQ.Data[i]; 	//copy que to array
				terminal_index++;
				RxQ.Data[i] = ZERO;			//empty que
			} 
			//Send_String(terminal_message);					// debugging function
			printf(terminal_message);
		  
			for(k=ZERO; k<i; k++){
				terminal_message[k] = ZERO;
			}	
			Q_Init(&RxQ);
			message_received = FALSE;
			terminal_index=ZERO;
		}
//		if(iot_message_received) {
//			for(i=ZERO; i<Get_Num_Rx_Chars_Available(); i++){
//				iot_message[iot_index] = RxQ.Data[i]; 	//copy que to array
//				iot_index++;
//				RxQ.Data[i] = ZERO;			//empty que
//			} 
//			//Send_String(terminal_message);					// debugging function
//			printf(iot_message);
//			for(k=ZERO; k<i; k++){
//				iot_message[k] = ZERO;
//			}	
//			Q_Init(&RxQ);
//			iot_message_received = FALSE;
//			iot_index=ZERO;
//		}
		if(GPS_message_received){	
		 for(i=ZERO; i<Get_Num_Rx_Chars_Available(); i++){
			 if(RxQ.Data[i] == ASCII_$){
				 copy_message = TRUE;
			 }
			 if(copy_message){
				input_message[index] = RxQ.Data[i]; 	//copy que to array
				index++;
			 }
			 RxQ.Data[i] = ZERO;			//empty que
		 } //Send_String(input_message);					// debugging function
		 Q_Init(&RxQ);
		 GPS_message_received = FALSE;
		 copy_message = FALSE;
		 index=ZERO;
		 Task_NMEA_Decode();
	 }
	 if(GPS_Message.new){
		 if(GPS_Message.Track_angle_true >=0 && GPS_Message.Track_angle_true <= 180){
				Control_RGB_LEDs(0,1,0);	// GREEN
				PTB->PSOR = MASK(RIGHT_ANTENNA_POS);		// right_antenna off
				PTB->PCOR	=	MASK(LEFT_ANTENNA_POS);			// LEFT_ANTENNA on
				printf("LEFT\n");
		 }
		 else if(GPS_Message.Track_angle_true > 180 && GPS_Message.Track_angle_true <= 360){
			 Control_RGB_LEDs(1,0,0);		// RED
			 PTB->PSOR	=	MASK(LEFT_ANTENNA_POS);		// LEFT_ANTENNA off
			 PTB->PCOR	=	MASK(RIGHT_ANTENNA_POS);	// right antenna on
			 printf("RIGHT\n");
		 }
		 else {Control_RGB_LEDs(0,0,0);}		// no antenna selected
	 }
	 Task_Report_Drift();		//report latitude and longitude
	 //read_full_compass();
 }
} 
// *******************************ARM University Program Copyright © ARM Ltd 2013************************************* 
