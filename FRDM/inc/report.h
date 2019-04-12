/*
 * report.h
 *
 *  Created on: Apr 11, 2019
 *      Author: iansc
 */

#ifndef REPORT_H_
#define REPORT_H_

#include "UART0.h"

// compile coniditional to enable application functionality if true
// (or bit banging if false)
#if 0 & !BLOCKING
#define APP
#endif

void report_init(void);
void count(uint8_t chr);
void report(uint32_t n, uint32_t cyc);
void fib(uint32_t *first, uint32_t *second, uint32_t *c, uint32_t *next);


#endif /* REPORT_H_ */
