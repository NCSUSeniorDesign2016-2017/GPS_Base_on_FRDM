#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <MKL25Z4.H>
#include "queue.h"

#define USE_UART_INTERRUPTS (1)

#define UART_OVERSAMPLE (16)
#define BUS_CLOCK 			(24e6)

#define TRUE (1)
#define FALSE (0) 
#define ZERO (0)
#define BAUD_RATE (115200)
#define GPS_BAUD_RATE (9600)

void Init_UART0(uint32_t baud_rate);
void UART0_Transmit_Poll(uint8_t);
uint8_t UART0_Receive_Poll(void);

void Init_UART1(uint32_t baud_rate);
void UART1_Transmit_Poll(uint8_t);
uint8_t UART1_Receive_Poll(void);

void Init_UART2(uint32_t baud_rate);
void UART2_Transmit_Poll(uint8_t data);
uint8_t UART2_Receive_Poll(void);

void Send_String(uint8_t * str);
void Send_to_IOT(uint8_t * str);
uint32_t Get_Num_Rx_Chars_Available(void);
uint8_t	Get_Char(void);

extern Q_T Tx_Data, Rx_Data;

#endif
// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************
