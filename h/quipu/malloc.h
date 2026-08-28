/* malloc.h - Quipu specific mallocing */

#ifndef _QUIPUMALLOC_
#define _QUIPUMALLOC_

#include "general.h"

extern unsigned mem_heap;
extern unsigned attr_index;

/*
#define MALLOCSTACK
*/

/* HEAP Selection */
/* Two main heaps - general and database.
 * Database is split into main critical part,
 * and several parts for attribute values.
 */
#define GENERAL_HEAP	mem_heap = 0;	/* general mallocing */
#define DATABASE_HEAP	mem_heap = 1;	/* critical database structures */
#define ATTRIBUTE_HEAP	{if (mem_heap == 1) mem_heap = 2 + attr_index;}
/* non critical database structures */
#define RESTORE_HEAP	{if (mem_heap >= 2) mem_heap = 1;}

#define SET_HEAP(x)	{if (mem_heap == 1) { \
	int _sh = set_heap (x); \
	unsigned int _uh; \
	if (add_int_to_int (&_sh, 2) == 0 && int2uint (_sh, &_uh) == 0) \
		mem_heap = _uh; \
}}

void start_malloc_trace (const char *f);
void stop_malloc_trace (void);
#endif
