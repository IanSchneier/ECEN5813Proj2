/*
 * ring.c
 *
 *  Created on: Apr 3, 2019
 *      Author: iansc
 */


/*========================================================================

 ** Circular buffer

 ** ECEN5813

 **========================================================================*/



#include "ring.h"

ring_t *init( int length )
{
	ring_t *ring;

	if((BUFF_MAX_SIZE >= length) && (length > 0))
	{
		ring = malloc(sizeof(ring_t));
		ring->Length = length;
		ring->Buffer = (uint8_t*)malloc(length * sizeof(uint8_t));
		ring->Ini = 0;
		ring->Outi = 0;
		ring->Count = 0;
		ring->Busy = 0;
	}
	else
	{
#ifdef WARN
		printf("Error: Invalid buffer length.\n");
#endif
		ring = NULL;
	}
	return ring;
}



uint8_t insert( ring_t *ring, uint8_t data )
{

#ifdef SAFE
	if(ring == NULL)
	{
		return 3;
	}
	if(ring->Busy)
	{
		return 2;
	}
	else if((ring->Buffer[ring->Ini] != 0) && (ring->Count >= ring->Length))
	{
#ifdef WARN
		if(ring->Count >= ring->Length) printf("Error: Buffer is full.\n");
		else printf("Error: Overwriting existing element.\n");
#endif
		return 1;
	}
#endif
	ring->Busy = 1;
	ring->Buffer[ring->Ini] = data;
	ring->Ini = ( (ring->Ini + 1) == ring->Length ) ? 0 : (ring->Ini + 1);
	ring->Count++;
	ring->Busy = 0;
	return 0;
}


uint8_t remove_ring( ring_t *ring, uint8_t *data )
{
#ifdef SAFE
	if(ring == NULL)
	{
		return 3;
	}
	if(ring->Busy)
	{
		return 2;
	}
	else if(ring->Count <= 0)
	{
#ifdef WARN
		printf("Error: Buffer is empty.\n");
#endif
		return 1;
	}
#endif
	ring->Busy = 1;
	//place element in pointer
	*data = ring->Buffer[ring->Outi];

	//remove element from buffer
	ring->Buffer[ring->Outi] = 0;
	ring->Outi = ((ring->Outi + 1) == ring->Length) ? 0 : (ring->Outi + 1);
	ring->Count--;
	ring->Busy = 0;
	return 0;
}

uint32_t get_count( ring_t *ring )
{
	return (ring != NULL) ? ring->Count : 0;
}


uint32_t resize( ring_t *ring, uint32_t length )
{
	if((BUFF_MAX_SIZE >= length) && (length > 0) && (ring->Count == 0 ))
	{
		ring->Buffer = (uint8_t*)realloc(ring->Buffer, length * sizeof(uint8_t));
		ring->Length = length;
		ring->Ini = 0;
		ring->Outi = 0;
		ring->Count = 0;

		return 0;
	}
	else
	{
#ifdef WARN
		printf("Error: Invalid buffer length.\n");
#endif
		return 1;
	}
}
