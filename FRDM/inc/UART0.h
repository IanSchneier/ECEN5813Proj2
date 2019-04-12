/*
 * UART0.h
 *
 *  Created on: Apr 9, 2019
 *      Author: iansc
 */

#ifndef UART0_H_
#define UART0_H_

#include <stdint.h>
#include <stdarg.h>
#include "MKL25Z4.h"
#include "ring.h"

#define BAUD (115200)
//Frequency of Clock
#define CLK (48000000)

/* setting to configure if UART will be polled or interrupt-based */
#if 1
#define BLOCKING

#else
// If interrupt based compile these functions
void UART0_IRQHandler(void);
void Tx_continue(void);
#endif

void UART0_Init(void);
void UART_Tx(uint8_t data);
void printUART( const char* format, ... );
void Tx(uint8_t data);

uint8_t UART_Rx(void);
uint8_t Rx_ready(void);
uint8_t Tx_ready(void);

//Go between varaiable to extract value from Tx ringbuffer and pass it
//to the Tx buffer of UART in order to preserve the volatility of UART0->D
extern uint8_t flag_UART0;
// Ringbuffer for sending data to UART
extern ring_t *ring_Tx;
// Ringbuffer for Receiving data from UART
extern ring_t *ring_Rx;

#endif
