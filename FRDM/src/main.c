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
#if 1
#define BLOCKING
#endif
/* TODO: insert other definitions and declarations here. */

//ringbuffer global
ring_t *r;

void UART_Init_Polled()
{
	uint16_t sbr = 0;
	uint32_t baudDiff = 0;

	// Disable UART
	UART0->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK);
	//	/* Write the sbr value to the BDH and BDL registers*/

	/* Calculate the baud rate modulo divisor, sbr */
	sbr = 48000000 / (BAUD * 16);

	/* Calculate the baud rate based on the temporary SBR values */
	baudDiff = (48000000 / (sbr * 16)) - BAUD;

	/* Select the better value between sbr and (sbr + 1) */
	if (baudDiff > (BAUD - (48000000 / (16 * (sbr + 1)))))
	{
		baudDiff = BAUD - (48000000 / (16 * (sbr + 1)));
		sbr++;
	}
	UART0->BDH = (UART0->BDH & ~UART_BDH_SBR_MASK) | (uint8_t)(sbr >> 8);
	UART0->BDL = (uint8_t)sbr;

	//PTA1 at pin 27 - UART0_RX, ALT2
	//PTA2 at pin 28 - UART0_TX, ALT2
	PORTA->PCR[1]=PORT_PCR_MUX(2);
	PORTA->PCR[2]=PORT_PCR_MUX(2);


	//Set clock source of UART0 to be MCGFLLCLK
	SIM->SOPT2 |= 0x4000000;
//	SIM_SOPT2 &= SIM_SOPT2_UART0SRC_MASK;
//	SIM_SOPT2 |= SIM_SOPT2_UART0SRC(1);
	//System Clock Gating Control Register 4
	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK; //0x400


	//Baud rate register high
	//#ifdef POLL
	//	UART0->BDH = 0b00000000;
	//#endif
	//	UART0->BDL = 0;
	//oversampling ratio of 16x
	UART0->C4 = 0x0f;
	//Set to 8-bit no parity
	UART0->C1 = 0;
	// Enable UART TX and RX.
	UART0->C2 = (UART_C2_TE_MASK | UART_C2_RE_MASK );//| UART_C2_TIE_MASK | UART_C2_RIE_MASK);

	//what?
	//UART0->C2 |= UART0_C2_RIE_MASK;
#ifndef BLOCKING
	//__enable_irq();
	NVIC_EnableIRQ(UART0_IRQn);
#endif
}

#ifndef BLOCKING
void UART0_IRQ(void)
{
	__disable_irq();
//	if ((UART0->C2 & UART0_C2_TIE_MASK) == UART0_C2_TIE_MASK)
//	{
//		//disable TX interrupt
//		UART0->C2 &= ~UART0_C2_TIE_MASK;
//
//		//while(!(UART0->S1 & UART_S1_TDRE_MASK));
//		//remove_ring(r, UART0->D);
//
//		UART0->C2 |= UART0_C2_RIE_MASK;		//enabling RX interrupt
//	}
//	if(UART0->S1 & UART_S1_TDRE_MASK)
//	{
//		//initiate Rx if the Receive Data Register Full Flag is high
//		//while(!(UART0->S1 & UART_S1_RDRF_MASK));
//		insert(r, UART0->D);
//		//TODO: insert full buffer check
//	}
	NVIC_EnableIRQ(UART0_IRQn);
	__enable_irq();

}
#endif


void UART_Tx(uint8_t data)
{
	//Wait until transmit buffer is empty
	while(!(UART0->S1 & UART_S1_TDRE_MASK));
	//put data into transmit buffer
	UART0->D = data;
}
//// Fuction to check if Tx is ready to send stuff
//uint8_t Tx_Ready(void)
//{
//	return !(UART0->S1 & UART_S1_TDRE_MASK);
//}
//
// Fuction to check if Tx is ready to send stuff

uint8_t Rx_Ready(void)
{
	return (UART0->S1 & UART_S1_RDRF_MASK);
}

uint8_t UART_Rx()
{
	//wait for received data buffer to be full before continuing
	while(!(UART0->S1 & UART_S1_RDRF_MASK));
	uint8_t data = UART0->D;
	return data;
}
// UART implementation of printf
void printUART( const char* format, ... ) {
	char str[0xff];
    va_list args;
    va_start( args, format );
    snprintf(str,0xff, format, args);
    va_end( args );

    for(size_t i=0; i< strlen(str);i++){
    	UART_Tx(str[i]);
    }
}

/*
 * @brief   Application entry point.
 */
int main(void) {


	r = init(256);
//	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitBootPeripherals();
//	/* Init FSL debug console. */
	BOARD_InitDebugConsole();


	UART_Init_Polled();
	//printUART("Hello World\n");

	/* Force the counter to be placed into memory. */
//	char i = 0;
	/* Enter an infinite loop, just incrementing a counter. */
	while(1) {

		UART_Tx(UART_Rx());
//		remove_ring(r,&i);
//		if(i!='\0')
//			UART_Tx(i);


	}
	return 0 ;
}
