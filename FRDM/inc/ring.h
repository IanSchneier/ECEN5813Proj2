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

#if 1
#define TEST
#endif

typedef struct
{
	char *Buffer;
	uint32_t Length;
	uint32_t Ini;
	uint32_t Outi;
	uint32_t Count;
} ring_t;

ring_t *init( int length );
int insert( ring_t *ring, char data );
int remove_ring( ring_t *ring, char *data );
int entries( ring_t *ring );
int count( ring_t *ring );

#endif /* RING_H_ */
