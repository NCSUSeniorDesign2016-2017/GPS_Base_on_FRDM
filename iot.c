#include <MKL25Z4.H>
#include "UART.h"
#include "LEDs.h"
#include "GPIO_defs.h"
#include "iot.h"

void Init_iot(void){
	// set iot_reset high
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
	PORTC->PCR[0]	&= 	~PORT_PCR_MUX_MASK;
	PORTC->PCR[0]	|=	 PORT_PCR_MUX(1);

	// set iot_STA_MINIAP high
	PORTC->PCR[7]	&= 	~PORT_PCR_MUX_MASK;
	PORTC->PCR[7]	|=	 PORT_PCR_MUX(1);
	
	// set iot_FACTORYA low
	PORTC->PCR[5]	&= 	~PORT_PCR_MUX_MASK;
	PORTC->PCR[5]	|=	 PORT_PCR_MUX(1);
	
	// set iot_WAKEUP high
	PORTC->PCR[6]	&= 	~PORT_PCR_MUX_MASK;
	PORTC->PCR[6]	|=	 PORT_PCR_MUX(1);
	
	PTC->PDDR |= MASK(0) | MASK(7) | MASK(5) | MASK(6);		//set to output

	PTC->PSOR = MASK(0);
	PTC->PSOR = MASK(7);
	PTC->PCOR = MASK(5);
	PTC->PSOR = MASK(6);
	
}

