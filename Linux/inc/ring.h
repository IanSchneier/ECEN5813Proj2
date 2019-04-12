/*========================================================================
** ring.h
** Circular buffer
** ECEN5813
**========================================================================*/
#ifndef RING_H
#define RING_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
	char *Buffer;
	int Length;
	int Ini;
	int Outi;
	int Count;
} ring_t;

ring_t *init( int length );
int insert( ring_t *ring, char data );
int remove_ring( ring_t *ring, char *data );
int entries( ring_t *ring );
int get_count( ring_t *ring );
int resize( ring_t *ring, int length );


#endif
