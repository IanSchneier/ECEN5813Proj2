/*
 * UART0.c
 *
 *  Created on: Apr 9, 2019
 *      Author: iansc
 */

#include "UART0.h"

#ifndef BLOCKING
// Ringbuffer for Receiving data from UART
ring_t *ring_Rx;
// Ringbuffer for sending data to UART
ring_t *ring_Tx;

void UART0_IRQHandler(void)
{
	uint8_t rx_chr;

	__disable_irq();
	if (Rx_ready())
	{
		rx_chr = UART0->D;
		//insert element into buffer when it becomes available
		while(insert(ring_Rx, rx_chr)==2);
	}
	else if(Tx_ready())
	{
		Tx_continue();
		//turn off tx interrupt
		UART0->C2 &= ~UART0_C2_TIE_MASK;
	}
	__enable_irq();

}

// Function to wait for Rx buffer to be empty
void Tx_continue(void)
{
	uint8_t tx_chr=0;

	while((UART0->S1 & UART_S1_TC_MASK) && (ring_Tx->Count > 0))
	{
		while(remove_ring(ring_Tx, &tx_chr)==2);
		Tx(tx_chr);
	}
}
#endif

void UART0_Init()
{
	uint16_t sbr = 0;
	uint32_t baudDiff = 0;

	//set clock div to 0
	SIM->CLKDIV1 = (SIM->CLKDIV1 & ~SIM_CLKDIV1_OUTDIV1_MASK) | SIM_CLKDIV1_OUTDIV1(0);
	MCG->C1 = (MCG->C1 & ~MCG_C1_CLKS_MASK) | MCG_C1_CLKS(0);
	MCG->C1 = (MCG->C1 & ~MCG_C1_IREFS_MASK) | MCG_C1_IREFS(1);

	MCG->C4 = (MCG->C4 & ~MCG_C4_DMX32_MASK) | MCG_C4_DMX32(1);
	MCG->C4 = (MCG->C4 & ~MCG_C4_DRST_DRS_MASK) | MCG_C4_DRST_DRS(1);


	//Set a clock to UART0
	SIM->SCGC4 = (SIM->SCGC4 & ~SIM_SCGC4_UART0_MASK) | SIM_SCGC4_UART0(1);
	// enable and select the 48 Mhz clock
	SIM->SOPT2 = (SIM->SOPT2 & ~SIM_SOPT2_UART0SRC_MASK) | SIM_SOPT2_UART0SRC(1);
	// enable clock at port A
	SIM->SCGC5 = (SIM->SCGC5 & ~SIM_SCGC5_PORTA_MASK) | SIM_SCGC5_PORTA(1);

	//Set PortA located at pin 27 to alternative2 or UART0_RX mode
	PORTA->PCR[1] |= PORT_PCR_MUX(2);
	//Set PortA located at pin 28 to alternative2 or UART0_TX mode
	PORTA->PCR[2] |= PORT_PCR_MUX(2);

	// set 16x oversampling rate
	UART0->C4 |= UART0_C4_OSR(0xf);

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
#else

	// enable UART operation via interrupts
	UART0->C2 |= (UART0_C2_RE_MASK | UART0_C2_TE_MASK | UART_C2_RIE_MASK);
	NVIC_EnableIRQ(UART0_IRQn);
#endif
}

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

uint8_t UART_Rx()
{
#ifdef BLOCKING
	while(!Rx_ready());
#endif
	return UART0->D;
}

void UART_Tx(uint8_t data)
{
#ifdef BLOCKING
	while(!Tx_ready);
	//put data into transmit buffer
	Tx(data);
#else
	Tx(data);
	//Set Tx Interrupt Enable flag in C2 to high to send signal via ISR
	UART0->C2 |= UART_C2_TIE_MASK;
#endif

}

// UART implementation of printf
void printUART( const char* format, ... )
{
	char str[0xff];
	uint8_t len;

	va_list args;
	va_start( args, format );
	vsnprintf(str,0xff, format, args);
	va_end( args );

	len = strlen(str);

#ifdef BLOCKING
	for(size_t i=0; i<strlen(str); i++)
	{
		UART_Tx(str[i]);
	}
#else
	for(size_t i=0; i<len; i++)
	{
		while(insert(ring_Tx,str[i])==2);
	}
	while(ring_Tx->Count > 0) UART0->C2 |= UART_C2_TIE_MASK;
#endif
}

void Tx(uint8_t data)
{
	UART0->D = data;
}
