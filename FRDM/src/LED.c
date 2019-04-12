/*
 * LED.c
 *
 *  Created on: Apr 9, 2019
 *      Author: iansc
 */
#include "LED.h"

/* This code is based on the youtube tutorial in the following link:
 * https://www.youtube.com/watch?v=HflcEy0BJTM	*/

void LED_Init(void)
{
	// Enable modificaiton to PORTB
	SIM->SCGC5 |= SIM_SCGC5_PORTB(1);
	// Multiplex the pin to red LED
	PORTB->PCR[18] |= PORT_PCR_MUX(1);
	// Enable the red LED
	GPIOB->PDDR |= (1 << 18);
}

void LED_toggle(void)
{
	GPIOB->PTOR |= (1<<18);
}
