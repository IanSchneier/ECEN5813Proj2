/*
 * ring.h
 *
 *  Created on: Apr 3, 2019
 *      Author: iansc
 */

#ifndef RING_H_
#define RING_H_

/*========================================================================
** ring.h
** Circular buffer
** ECEN5813
**========================================================================*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

//Maximum buffer size
#define BUFF_MAX_SIZE (32768)

#if 0
#define WARN
#endif
// Compile option for more secure saftety checks in exchange for performance
#if 1
#define SAFE
#endif

typedef struct
{
	uint8_t *Buffer;
	uint32_t Length;
	uint32_t Ini;
	uint32_t Outi;
	uint32_t Count;
	uint8_t Busy;
} ring_t;

ring_t *init( int length );
uint8_t insert( ring_t *ring, uint8_t data );
uint8_t remove_ring( ring_t *ring, uint8_t *data );
uint32_t get_count( ring_t *ring );
uint32_t resize( ring_t *ring, uint32_t length );

#endif /* RING_H_ */
