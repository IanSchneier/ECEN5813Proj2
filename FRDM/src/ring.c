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
		ring->Buffer = (char*)malloc(length * sizeof(char));
		ring->Ini = 0;
		ring->Outi = 0;
		ring->Count = 0;
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



int insert( ring_t *ring, char data )
{
#ifdef SAFE
//	if((ring->Buffer[ring->Ini] == '\0') && (ring->Count < ring->Length))
	if(ring->Count < ring->Length)
	{
#endif
		ring->Buffer[ring->Ini] = data;

		ring->Ini = ( (ring->Ini + 1) == ring->Length ) ? 0 : (ring->Ini + 1);
		ring->Count++;
		return 0;
#ifdef SAFE
	}
	else
	{
#ifdef WARN
		if(ring->Count >= ring->Length) printf("Error: Buffer is full.\n");
		else printf("Error: Overwriting existing element.\n");
#endif
		return 1;
	}
#endif
}



int remove_ring( ring_t *ring, char *data )
{
#ifdef SAFE
	//if((ring->Outi != ring->Ini) && (ring->Count > 0))
	if(ring->Count > 0)
	{
#endif
		//place element in pointer
		*data = ring->Buffer[ring->Outi];

		//remove element from buffer
		ring->Buffer[ring->Outi] = '\0';

		ring->Outi = ((ring->Outi + 1) == ring->Length) ? 0 : (ring->Outi + 1);

		ring->Count--;
		return 0;
#ifdef SAFE
	}
	else
	{
#ifdef WARN
		printf("Error: Buffer is empty.\n");
#endif
		return 1;
	}
#endif
}



int entries( ring_t *ring )
{
	for(int i = 0; i < ring->Length; i++)
	{
		printf("Element %d: %c.\n", i, ring->Buffer[i]);
	}
	return 0;
}



// TODO: determine if resetting head and tail indexes best way for resize
void resize( ring_t *ring, int length )
{
	if((BUFF_MAX_SIZE >= length) && (length > 0))
	{
		ring->Buffer = (char*)realloc(ring->Buffer, length * sizeof(char));
		ring->Length = length;
		ring->Ini = 0;
		ring->Outi = 0;
		ring->Count = 0;
	}
	else
	{
		printf("Error: Invalid buffer length.\n");
	}
}
