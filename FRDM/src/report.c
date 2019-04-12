/*
 * report.c
 *
 *  Created on: Apr 11, 2019
 *      Author: iansc
 */
#include "report.h"

//Array that tracks the number of character occurances in UART0 Rx
uint32_t occr[256];

void report_init(void)
{
	size_t i;
	for(i=0; i<256;i++){
		occr[i]=0;
	}
}

void count(uint8_t chr)
{
	++occr[chr];
}

void report(uint32_t n, uint32_t cyc)
{
	size_t i;
	printUART("\r\nApplication Report\r\n");
	printUART("Fibonacci Computation Update.\r\n");
	printUART("Current Numerical output: %d\r\n", n);
	printUART("Number of Cycles: %d\r\n", cyc);
	printUART("Character Count Update.\r\n");
	for(i=0; i<256; i++)
	{
		if(occr[i]>0)
		{
			//doesn't print out the counter for enter properly
			//it does show its count but not its char or ascii value
			//when i press del on my computer
			printUART("%d: %c - %d\r\n", i, i, occr[i]);
		}
	}
}

// recursive fibonacci function based on code from this site:
// https://www.programmingsimplified.com/c-program-generate-fibonacci-series
void fib(uint32_t *first, uint32_t *second, uint32_t *c, uint32_t *next)
{
	if (*c <= 1)
		*next = *c;
	else
	{
		*next = *first + *second;
		*first = *second;
		*second = *next;
	}
	(*c)++;
}

