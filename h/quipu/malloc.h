/* malloc.h - Quipu specific mallocing */

#ifndef _QUIPUMALLOC_
#define _QUIPUMALLOC_

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

#define SET_HEAP(x)	{if (mem_heap == 1) mem_heap = 2 + set_heap (x);}

void start_malloc_trace (char *f);
void stop_malloc_trace (void);
#endif
