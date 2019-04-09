/*
 * Copyright 2016-2018 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file    Polled_UART.c
 * @brief   Application entry point.
 */
#include <stdarg.h>

#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
//#include "fsl_debug_console.h"
//#include "fsl_clock.h"
/* TODO: insert other include files here. */
#include "ring.h"

#define BAUD (115200)
//Frequency of Clock
#define CLK (48000000)

/* setting to configure if UART will be polled or interrupt-based */
#if 0
#define BLOCKING
#endif
/* TODO: insert other definitions and declarations here. */

#ifndef BLOCKING
// Ringbuffer for sending data to UART
ring_t *r_Tx;
// Ringbuffer for Receiving data from UART
ring_t *r_Rx;
#endif

// Global variable to pass functions into the UART Tx buffer
volatile uint8_t chr_Tx;


uint8_t Rx_ready(void);
uint8_t Tx_ready(void);

void UART_Init_Polled()
{
	uint16_t sbr = 0;
	uint32_t baudDiff = 0;
	//Disable UART to edit it
	//	UART0->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK);


	//Set a clock to UART0
	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
	// enable and select the 48 Mhz clock
	SIM->SOPT2 &= ~SIM_SOPT2_UART0SRC_MASK;
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1);
	// enable clock at port A
	SIM->SCGC5 = SIM_SCGC5_PORTA_MASK;

	//Set PortA located at pin 27 to alternative2 or UART0_RX mode
	PORTA->PCR[1] = PORT_PCR_MUX(2);
	//Set PortA located at pin 28 to alternative2 or UART0_TX mode
	PORTA->PCR[2] = PORT_PCR_MUX(2);

	// set 16x oversampling rate
	UART0->C4 = UART0_C4_OSR(0xf);

//	UART0->C1 = 0;

	/* Calculate the baud rate modulo divisor, sbr */
	sbr = CLK / (BAUD * 16);

	/* Calculate the baud rate based on the temporary SBR values */
	baudDiff = (CLK / (sbr * 16)) - BAUD;

	/* Select the better value between sbr and (sbr + 1) */
	if (baudDiff > (BAUD - (CLK / (16 * (sbr + 1)))))
	{
		baudDiff = BAUD - (CLK / (16 * (sbr + 1)));
		sbr++;
	}
	UART0->BDH = (UART0->BDH & ~UART_BDH_SBR_MASK) | (uint8_t)(sbr >> 8);
	UART0->BDL = (uint8_t)sbr;



#ifdef BLOCKING
	// enable UART operation
	UART0->C2 = (UART0_C2_RE_MASK | UART0_C2_TE_MASK );
	__enable_irq();
#else
	// enable UART operation via interrupts
	UART0->C2 |= (UART0_C2_RE_MASK | UART0_C2_TE_MASK | UART_C2_RIE_MASK);
	NVIC_EnableIRQ(UART0_IRQn);
#endif
}

#ifndef BLOCKING
void UART0_IRQHandler(void)
{
	__disable_irq();
	//Rx Interrupt
	if (Rx_ready())
	{
//		chr_Tx = UART0->D;
		//re-enable RX interrupt after succesful ringbuffer insertion
		if(insert(r_Rx, UART0->D))
			UART0->C2 &= ~UART0_C2_RIE_MASK; //Disable Rx interrupt enable if insertion fails
	}
	else if(UART0->S1 & UART_S1_TDRE_MASK)
	{
		UART0->D = chr_Tx;
		UART0->C2 &= ~UART0_C2_TIE_MASK;
	}
	__enable_irq();

}
#endif

#ifdef BLOCKING
// Function to wait for Rx buffer to be empty
void Rx_wait(void)
{
	//wait for received data buffer to be full before continuing
	while(!(UART0->S1 & UART_S1_RDRF_MASK));
}

// Function to block PC until Tx is ready to send stuff
void Tx_Wait(void)
{
	while(!(UART0->S1 & UART_S1_TDRE_MASK));
}
#else
//Function that ISR calls to see if it can receive stuff
uint8_t Rx_ready(void)
{
	return (UART0->S1 & UART_S1_RDRF_MASK);
}
//Function that ISR calls to see if it can send stuff
uint8_t Tx_ready(void)
{
	return (UART0->S1 & UART_S1_TDRE_MASK);
}
#endif

uint8_t UART_Rx()
{
#ifdef BLOCKING
	Rx_wait();
#endif
	return UART0->D;
}

void UART_Tx(uint8_t data)
{
#ifdef BLOCKING
	Tx_Wait();
	//put data into transmit buffer
		UART0->D = data;
#else
		chr_Tx = data;
		UART0->C2 |= UART_C2_TIE_MASK;
#endif

}

// UART implementation of printf
void printUART( const char* format, ... )
{
	char str[0xff];
	va_list args;
	va_start( args, format );
	snprintf(str,0xff, format, args);
	va_end( args );

	for(size_t i=0; i< strlen(str);i++)
	{
		UART_Tx(str[i]);
	}
}

/*
 * @brief   Application entry point.
 */
int main(void) {
	//xfer pointer
	char rx_to_tx;
#ifndef BLOCKING
	r_Rx = init(256);
	r_Tx = init(256);
#endif
	//TODO: replace this with clock function
	BOARD_InitBootClocks();
	UART_Init_Polled();

	/* Enter an infinite loop */
	while(1) {
#ifdef BLOCKING
		UART_Tx(UART_Rx());
#endif
		if(r_Rx->Count>0)
		{
			remove_ring(r_Rx, &rx_to_tx);
//			insert(r_Tx, rx_to_tx);
			UART_Tx(rx_to_tx);
		}

	}
	return 0 ;
}
