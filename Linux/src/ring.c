/*========================================================================
** Circular buffer
** ECEN5813
**========================================================================*/

#include "ring.h"
#ifdef TEST

#define MAX (0xffffffff)

#endif
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
	if(ring == NULL)
	{
		return 3;
	}
	else if((ring->Buffer[ring->Ini] != 0) || (ring->Count >= ring->Length))
	{
#ifdef WARN
		if(ring->Count >= ring->Length) printf("Error: Buffer is full.\n");
		else printf("Error: Overwriting existing element.\n");
#endif
		return 1;
	}
#endif
	ring->Buffer[ring->Ini] = data;
	ring->Ini = ( (ring->Ini + 1) == ring->Length ) ? 0 : (ring->Ini + 1);
	ring->Count++;
	return 0;
}

int remove_ring( ring_t *ring, char *data )
{
#ifdef SAFE
	if(ring == NULL)
	{
		return 3;
	}
	else if(ring->Count <= 0)
	{
#ifdef WARN
		printf("Error: Buffer is empty.\n");
#endif
		return 1;
	}
#endif
	//place element in pointer
	*data = ring->Buffer[ring->Outi];
	//remove element from buffer
	ring->Buffer[ring->Outi] = 0;
	ring->Outi = ((ring->Outi + 1) == ring->Length) ? 0 : (ring->Outi + 1);
	ring->Count--;
	return 0;
}

int entries( ring_t *ring )
{
	for(int i = 0; i < ring->Length; i++)
	{
		printf("Element %d: %c.\n", i, ring->Buffer[i]);
	}

	return 0;
}

int get_count( ring_t *ring )
{
	return (ring != NULL) ring->Count : 0;
}

int resize( ring_t *ring, int length )
{
	if((BUFF_MAX_SIZE >= length) && (length > 0) && (ring->Count ==0))
	{
		ring->Length = length;
		ring->Buffer = (char*)realloc(ring->Buffer, length * sizeof(char));
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
