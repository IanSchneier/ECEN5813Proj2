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
 * @file    main.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include <stdint.h>

#include "MKL25Z4.h"
#include "LED.h"
#include "ring.h"
#include "UART0.h"
#include "report.h"

/*
 * @brief   Application entry point.
 */
int main(void)
{
#ifdef APP
	// Fibonacci fn iterator
	uint32_t iter = 0;
	// Fibonacci fn output reset counter
	uint32_t cycle = 0;
	// Value outputted for Fibonacci report
	uint32_t num = 0;
	// Value of the two numbers to made the fibonacci number
	uint32_t f0 = 0;
	uint32_t f1 = 1;
#endif
#ifndef BLOCKING
	//reciving character
	uint8_t rx_chr;
#endif

	UART0_Init();
	LED_Init();
	LED_toggle();

	//Initialize ring buffers
	ring_Rx = init(256);
	ring_Tx = init(256);

#ifdef APP
	report_init();
#endif
	/* Enter an infinite loop */
	while(1)
	{
#ifdef BLOCKING
		UART_Tx(UART_Rx());
#else
//Run standard application
#ifdef APP
		if(ring_Rx->Count>0)
		{
			LED_toggle();
			while(remove_ring(ring_Rx, &rx_chr)==2);
			count(rx_chr);
			report(num,cycle);
			LED_toggle();
		}
		else
		{
			//Conduct test
			fib(&f0, &f1, &iter, &num);
			//reset values to prevent junk values being implemented into fn
			if(iter == 47)
			{
				f0 = 0;
				f1 = 1;
				iter = 0;
				//increment count of fibonacci cycle occurances
				if(cycle < UINT32_MAX) cycle++;
			}
		}

#else
		LED_toggle();
		if(ring_Rx->Count>0)
		{
			while(remove_ring(ring_Rx, &rx_chr)==2);
			UART_Tx(rx_chr);
		}
#endif

#endif
	}
	return 0;
}
