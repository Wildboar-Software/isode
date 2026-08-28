/* malloc.c - Quipu DSA specific memory management */

#include <fcntl.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <stdio.h>
#ifdef	__linux__
#include <values.h>
#define	_MALLOC_H	1	/* not to include the standard <malloc.h> */
#endif
#include "manifest.h"
#include "quipu/util.h"
#ifndef MALLOC_TEST
#include "quipu/malloc.h"
#endif

#if (defined (SYS5) || defined (__STDC__) || defined (_AIX)) && !defined (HPUX) && !defined (masscomp)
#define MALLOC_RETURN void *
#define FREE_RETURN void
#else
#define MALLOC_RETURN char *
#define FREE_RETURN int
#endif

#ifndef MALLOC_TEST
static void write_string (char *p);
static void write_addr (char *addr);
static void write_uval (unsigned x);
static void log_realloc (size_t oldlen, size_t newlen, size_t bsize, char *addr);
static void print_free_list (size_t heap);
static void write_stack (char * x);
static struct freelist *new_freelist (void);
static char *big_malloc (const size_t realsize);
#endif

static int malloc_file = 0;

#ifdef MALLOC_TEST
#define QUIPU_MALLOC
#undef LLOG
#define LLOG(a,b,c)
#define attempt_restart()
int mem_heap = 0;
#endif

#ifdef QUIPU_MALLOC

#ifndef MALLOC_TEST
extern LLog * log_dsap;
extern void attempt_restart(int sig);
#endif

#ifdef MALLOCDEBUG

#ifdef sun3
#define MALLOCSTACK
#include <frame.h>
#endif

#ifdef sun4
#define MALLOCSTACK
#include <machine/frame.h>
#endif

#endif /* MALLOCDEBUG */

#ifdef MALLOCSTACK
#include "sys.file.h"
off_t lseek(int fd, off_t offset, int whence);

#ifndef MALLOCTRACE
#define MALLOCTRACE
#endif

#else   /* MALLOCSTACK */

#define write_stack(x)

#endif	/* MALLOCSTACK */

#define MAXHEAP		100		/* Number of heaps */
#ifndef	BSD42
#define PAGESIZE	0x2000		/* The systems memory page size */
#else
#define	PAGESIZE	pagesize
#endif

#define ALIGN(x)	(((x) + (sizeof(char *) - 1)) & ~(sizeof(char *) - 1))
#define PAGEALIGN(x)	(((x) + pageminusone) & pagemask)
#define SMALLMAX	smallmax  /* largest block a short can reference */

struct header {
	union {
		struct {
			unsigned short 	 control;
			unsigned short   size;
		} small;
		size_t big;
	} un;
};

#define bigsize		un.big
#define smallsize	un.small.size
#define use		un.small.control

#define INUSE           0x1000u
#define USED(x)         (x->use & INUSE)

/* sizes chosen for anticipated QUIPU behaviour */

#define BUCKETS 8
static size_t sizes [BUCKETS] = { 0, 12, 24, 68, 512, 1028, 8204, MAXINT};

struct freelist {
	struct header * block;
	size_t size;
	struct freelist * next;
	struct freelist * prev;
};

struct freehead {
	struct header	head;
	struct freelist * flist;
};

static void big_free (struct header *ptr);
static void add_free (struct header *x);
static struct header *next_free_block (const struct header *ptr);
static void *quipu_sbrk (size_t n);
static int remove_free_ent (struct freelist *a);
static int use_block (struct header *ptr, const size_t size);
static int set_use_heap (struct header *h, const unsigned heap);

static struct freelist  heaps[MAXHEAP][BUCKETS];
static struct freelist *heapptr[MAXHEAP];
static struct freelist  bigmem = { 0,0,&bigmem, &bigmem};
static struct freelist  *bigfree = &bigmem;
static struct freelist  freemem = { 0,0,&freemem, &freemem};
static struct freelist  *listfree = &freemem;

static int first_malloc = 1;
static char * top_mem;

#ifdef	BSD42
static size_t pagesize = 0x2000;
#endif

/* all calculated later - initialise for safety */
static size_t pagemask = 0xE000;
static size_t pageminusone = 0x1FFF;
static unsigned short smallmax = 0xDFFF;

#ifdef MALLOCTRACE
static int malloc_started = 0;
static char * malloc_fname = (char *)0;
#endif

#ifndef MALLOCTRACE
#endif

#endif	/* QUIPU_MALLOC */

void start_malloc_trace (const char *f) {
#ifdef MALLOCTRACE
	char * env, *getenv(const char *name);

	if (((env = getenv ("TRACE_MEMORY")) == (char *)0) || (*env == 0))
		return;
	if (! malloc_started) {
		if (f == (char *)0)
			malloc_fname = "memory.out";
		else
			malloc_fname = f;
		malloc_file = creat (malloc_fname,0644);
		malloc_started = 1;
	} else {
		malloc_file = open (malloc_fname,1);
		lseek (malloc_file,0l,2);
	}
#else
	malloc_file = 0;
#endif
}

void stop_malloc_trace (void) {
#ifdef MALLOCTRACE
	if (malloc_file)
		close (malloc_file);
#endif
	malloc_file = 0;
}

#ifdef QUIPU_MALLOC
#ifdef MALLOCTRACE

static write_string (char *p) {
	char *q;

	if (!malloc_file)
		return;
	q = p;
	while (*q++)
		;
	write(malloc_file, p, q-p-1);
}

static write_addr (char *addr) {
	char buf[20];
	static char hex[] = "0123456789abcdef";
	char *ptr;
	int x;

	if (!malloc_file)
		return;
	x = (int) addr;
	if (x == 0) {
		write(malloc_file, "0 ",2);
		return;
	}
	ptr = buf;
	while (x > 0)
		*ptr++ = hex[x % 16], x /= 16;
	*ptr = 0;
	while (ptr != buf)
		write(malloc_file, --ptr,1);
	write (malloc_file," ",1);
}

static void write_uval (unsigned x) {
	char buf[20];
	static char dec[] = "0123456789";
	char *ptr;

	if (!malloc_file)
		return;
	if (x == 0) {
		write(malloc_file, "0 ",2);
		return;
	}
	ptr = buf;
	while (x > 0)
		*ptr++ = dec[x % 10], x /= 10;
	while (ptr != buf)
		write(malloc_file, --ptr,1);
	write (malloc_file," ",1);
}

static log_realloc (size_t oldlen, size_t newlen, size_t bsize, char *addr) {
	write_string ("realloc of ");
	write_uval (oldlen);
	write_string ("at ");
	write_addr (addr);
	write_string ("\n");
	write_stack("x");
	write_string ("realloc-to of ");
	write_uval (newlen);
	write_string ("gets ");
	write_uval (bsize);
	write_string ("at ");
	write_addr (addr);
	write_string ("\n");
	write_stack("x");
}

static print_free_list (size_t heap) {
	int i;
	struct freelist * top;
	struct freelist * ptr;

	write_string ("free list for heap ");
	write_int(heap);
	write_string(":\n");
	for (i=0; i<BUCKETS; i++) {
		top = &heaps[heap][i];
		write_uval (sizes[i]);
		write_string (": ");
		for (ptr = top->next ; ptr != top; ptr=ptr->next)
			write_uval (ptr->size);
		write_string ("\n");
	}
}

#ifdef MALLOCSTACK

#ifdef sun4
#endif

static write_stack (char * x)
{
	struct frame *fp;

	if (!malloc_file)
		return;
#ifdef sun3
	for (fp = ((struct frame*)(&x-2))->next ;
			fp;
			fp = fp->fr_savfp)
#endif
#ifdef sun4
		for ( fp = (struct frame *) (&fp+1);
				fp->fr_savfp;
				fp = fp->fr_savfp)
#endif
		{
			write_string ("C ");
			write_addr ((char *)fp->fr_savpc);
			write_string ("\n");
		}
	write_string ("\n");
}

#endif /* MALLOCSTACK */
#endif /* MALLOCTRACE */

#define return_freelist(z) { \
	z->next = listfree->next; \
	z->prev = listfree; \
	listfree->next->prev = z; \
	listfree->next = z; }

static void *
quipu_sbrk (size_t n) {
	int incr;

	if (sizet2int (n, &incr) != 0)
		return (void *)-1;
	return sbrk (incr);
}

static struct freelist *
new_freelist (void) {
	struct freelist * flist;
	size_t i;
	struct freelist * next;

	if ((flist = (struct freelist *) quipu_sbrk ((size_t)PAGESIZE)) == (struct freelist *)-1) {
		/* there are 100s of places where Quipu would choke on a naff malloc */
		attempt_restart (-2);
		return ((struct freelist *)0);
	}
	top_mem = (char *)flist + PAGESIZE;
	next = (struct freelist *)flist;
	next++;
	for (i=sizeof (struct freelist); i< (size_t)PAGESIZE ; i+=sizeof (struct freelist)) {
		return_freelist (next);
		next++;
	}
	return (flist);
}

/* used for mallocs of > MAXSMALL */
static char *big_malloc (const size_t realsize) {
	size_t blocksize;
	struct freelist * flist;
	struct header * head = (struct header *)0;
	char * mem;
	for (flist = bigfree->next; flist != bigfree; flist=flist->next) {
		if (flist->size >= realsize) {
			head = flist->block;
			flist->prev->next = flist->next;
			flist->next->prev = flist->prev;
			return_freelist (flist);
			break;
		}
	}
	if (head == (struct header *)0) {
		/* go and get one then !!! */
		blocksize = PAGEALIGN(realsize);
		if ((head = (struct header *) quipu_sbrk (blocksize)) == (struct header *)-1) {
			/* there are 100s of places where Quipu would choke on a naff malloc */
			attempt_restart (-2);
			return ((char *)0);
		}
		top_mem = (char *)head + blocksize;
		head->bigsize = blocksize | 0x01;
	} else
		head->bigsize |= 0x01;
	mem = (char *) head + ALIGN(sizeof (struct header));
#ifdef MALLOCTRACE
	write_string ("gets ");
	write_uval (head->bigsize & ~(size_t)1 );
	write_string ("at ");
	write_addr (mem);
	write_string ("\n");
	write_stack("x");
#endif
	return (mem);
}

static void big_free (struct header *ptr) {
	struct freelist *next;
	struct freehead *x;
	if (listfree->next == listfree) {
		if ((next = new_freelist ()) == (struct freelist *)0)
			return;
	} else {
		next = listfree->next;
		next->prev->next = next->next;
		next->next->prev = next->prev;
	}
	ptr->bigsize &= ~(size_t)1;
	next->size = ptr->bigsize;
	next->block = ptr;
	next->next = bigfree->next;
	next->prev = bigfree;
	bigfree->next->prev = next;
	bigfree->next = next;
	x = (struct freehead *) ptr;
	x->flist = next;
}

static void add_free (struct header *x) {
	struct freelist *next, *c;
	size_t * p = sizes;
	int bucket;

	if (ushort_bic (&x->use, INUSE) != 0)
		return;
	if ((c = heapptr[x->use]) == (struct freelist *) 0)
		c = heapptr[x->use] = heaps[x->use];
	while ( x->smallsize > *p++ )
		;
	if (ptrdiff2int ((p - 1) - sizes, &bucket) != 0)
		return;
	c = &c[bucket];
	if (listfree->next == listfree) {
		if ((next = new_freelist ()) == (struct freelist *)0)
			return;
	} else {
		next = listfree->next;
		next->prev->next = next->next;
		next->next->prev = next->prev;
	}
	next->size = x->smallsize;
	next->block = x;
	next->next = c->next;
	next->prev = c;
	c->next->prev = next;
	c->next = next;
	((struct freehead *) x)->flist = next;
}

static int
remove_free_ent (struct freelist *a)
{
	if (ushort_bis (&a->block->use, INUSE) != 0)
		return -1;
	a->prev->next = a->next;
	a->next->prev = a->prev;
	return_freelist(a);
	return 0;
}

static int
set_use_heap (struct header *h, const unsigned heap)
{
	unsigned short u;

	if (uint2ushort (heap, &u) != 0 || ushort_bis (&u, INUSE) != 0)
		return -1;
	h->use = u;
	return 0;
}

static int
use_block (struct header *ptr, const size_t size)
{
	struct header *unext;
	unsigned short sz, leftover;

	if (sizet2ushort (size, &sz) != 0)
		return -1;
	if (ptr->smallsize == sz)
		return 0;
	if ((size_t) ptr->smallsize < size + sizeof (struct freehead))
		return 0;
	unext = (struct header *) ((char *) ptr + size);
	if (uint2ushort ((unsigned) ptr->smallsize - (unsigned) sz, &leftover) != 0)
		return -1;
	unext->smallsize = leftover;
	unext->use = ptr->use;
	if (ushort_bic (&unext->use, INUSE) != 0)
		return -1;
	ptr->smallsize = sz;
	if (ushort_bis (&ptr->use, INUSE) != 0)
		return -1;
	add_free (unext);
	return 0;
}

static struct header *next_free_block (const struct header *ptr) {
	struct header * next;
	next = (struct header *)((char *)ptr + ptr->smallsize);
	if (((size_t)(next - 1) & pagemask) != ((size_t)next & pagemask))
		return (struct header *)0;
	if (((char *)next < top_mem) && (next->use == (ptr->use & ~INUSE)))
		return (next);
	return (struct header *)0;
}

MALLOC_RETURN
#ifdef lint
x_malloc (size_t size)
#else
malloc (const size_t size)
#endif
{
	char * mem;
	struct header *head;
	size_t realsize, blocksize;
	struct freelist * top;
	struct freelist * ptr;
	int i;
	size_t * p = sizes;

	if (first_malloc) {
#ifdef	BSD42
		{
			const int pagei = getpagesize();

			if (int2sizet (pagei, &pagesize) != 0) {
				attempt_restart (-2);
				return ((char *)0);
			}
		}
#endif
		pageminusone = (size_t)PAGESIZE - 1;
		pagemask = ~pageminusone;
		{
			unsigned short page;
			int pagei;

			if (sizet2int ((size_t)PAGESIZE, &pagei) != 0
					|| int2u16 (pagei, &page) != 0) {
				attempt_restart (-2);
				return ((char *)0);
			}
			smallmax = (unsigned short) (UINT16_MAX - page);
		}
	}
	if (mem_heap >= MAXHEAP)
		mem_heap = MAXHEAP - 1;
	if (size < sizeof (struct freelist *))	/* memory will be used when freed for freelist !!! */
		realsize = ALIGN (sizeof (struct freehead));
	else
		realsize = ALIGN (size) + ALIGN (sizeof (struct header));
	if (realsize >= SMALLMAX) {
#ifdef MALLOCTRACE
		write_string ("malloc of ");
		write_uval (size);
#endif
		return (big_malloc (realsize));
	}
	if (first_malloc) {
		/* set up freelist */
		size_t x;
		int j;
		for (i = 0; i < MAXHEAP; i++) {
			heapptr[i] = (struct freelist *) 0;
			for (j = 0 ; j<BUCKETS; j++) {
				heaps[i][j].prev = &heaps[i][j];
				heaps[i][j].next = &heaps[i][j];
				heaps[i][j].size = 0;
			}
		}
		/* align first sbrk to page boundary */
		x = (size_t)sbrk(0);
		x = PAGEALIGN (x) - x;
		blocksize = PAGEALIGN(realsize) + x;
		if ((head = (struct header *) quipu_sbrk (blocksize)) == (struct header *)-1) {
			/* there are 100s of places where Quipu would choke on a naff malloc */
			attempt_restart (-2);
			return ((char *)0);
		}
		if (sizet2ushort (blocksize, &head->smallsize) != 0
				|| set_use_heap (head, mem_heap) != 0) {
			attempt_restart (-2);
			return ((char *)0);
		}
		top_mem = (char *)head + blocksize;
		first_malloc = 0;
	} else {
		if ((top = heapptr[mem_heap]) == (struct freelist *)0)
			goto allocate_more;
		while ( size > *p++ )
			;
		if (ptrdiff2int ((p - 1) - sizes, &i) != 0) {
			attempt_restart (-2);
			return ((char *)0);
		}
		top = &top[i];
		for (; i < BUCKETS ; i++,top++ ) {
			for (ptr = top->next ; ptr != top; ptr=ptr->next) {
				if (ptr->size >= realsize) {
					if (remove_free_ent (ptr) != 0) {
						attempt_restart (-2);
						return ((char *)0);
					}
					head = ptr->block;
					goto return_memory;
				}
			}
		}
allocate_more:
		;
		blocksize = PAGEALIGN(realsize);
		if ((head = (struct header *) quipu_sbrk (blocksize)) == (struct header *)-1) {
			/* there are 100s of places where Quipu would choke on a naff malloc */
			attempt_restart (-2);
			return ((char *)0);
		}
		if (sizet2ushort (blocksize, &head->smallsize) != 0
				|| set_use_heap (head, mem_heap) != 0) {
			attempt_restart (-2);
			return ((char *)0);
		}
		top_mem = (char *)head + blocksize;
	}
return_memory:
	;
	if (use_block (head, realsize) != 0) {
		attempt_restart (-2);
		return ((char *)0);
	}
	mem = (char *) head + ALIGN(sizeof (struct header));
#ifdef MALLOCTRACE
	write_string ("malloc of ");
	write_uval (size);
	write_string ("gets ");
	write_uval (head->smallsize);
	write_string ("at ");
	write_addr (mem);
	write_string ("heap ");
	write_uval (mem_heap);
	write_string ("\n");
	write_stack("x");
#endif
	return (mem);
}

FREE_RETURN
#ifdef lint
x_free(s1)
#else
free(s1)
#endif
#ifndef LINUX
char *s1;
#else
void *s1;
#endif
{
	struct header * ptr;
	struct header * next;
	char* s = (char*) s1;
	if (s == NULL)
		return;
	ptr = (struct header *) (s - ALIGN (sizeof (struct header)));
	if (ptr->smallsize & 1) {
#ifdef MALLOCTRACE
		write_string ("free of ");
		write_uval (ptr->bigsize);
		write_string ("at ");
		write_addr (s);
		write_string ("heap (big)\n");
		write_stack("x");
#endif
		big_free (ptr);
		return;
	}
#ifdef MALLOCTRACE
	write_string ("free of ");
	write_uval (ptr->smallsize);
	write_string ("at ");
	write_addr (s);
	write_string ("heap ");
	write_uval (ptr->use & ~INUSE);
	write_string ("\n");
	write_stack("x");
#endif
	if (! USED(ptr)) {
		LLOG (log_dsap,LLOG_EXCEPTIONS,("freeing problem"));
		return;		/* already freed !!! */
	}
	/* join forward free block in loop to catch previous back blocks ! */
	while ((next = next_free_block(ptr)) != (struct header *) 0) {
		if (ushort_add (&ptr->smallsize, next->smallsize) != 0) {
			LLOG (log_dsap,LLOG_EXCEPTIONS,("free size overflow"));
			attempt_restart (-2);
			return;
		}
		if (remove_free_ent (((struct freehead *)next)->flist) != 0) {
			attempt_restart (-2);
			return;
		}
	}
	add_free (ptr);
	return;
}

MALLOC_RETURN
#ifdef lint
x_realloc(
#else
realloc(
#endif
#ifndef LINUX
char *s1,
#else
void *s1,
#endif
size_t n)
{
	char *mem, *s = (char*) s1;
	size_t realsize;
	struct header * ptr;
	struct header * next;
	size_t copysize;
	ptr = (struct header *) (s - ALIGN (sizeof (struct header)));
	if (ptr->smallsize & 1) {
		DLOG (log_dsap,LLOG_DEBUG,("re-alloc of big block"));
#ifdef MALLOCTRACE
		write_stack ("x");
#endif
		realsize = ALIGN (n) + ALIGN (sizeof (struct header));
		copysize = ptr->bigsize & ~(size_t)1;
		if (copysize >= realsize)
			/* its big enough - carry on */
			return s;
		goto out;
	}
	copysize = ptr->smallsize;
	if (! USED(ptr)) {
		LLOG (log_dsap,LLOG_EXCEPTIONS,("re-alloc problem"));
#ifdef MALLOCTRACE
		write_stack ("x");
#endif
		goto out;
	}
	realsize = ALIGN (n) + ALIGN (sizeof (struct header));
	if (realsize >= SMALLMAX) {
		DLOG (log_dsap,LLOG_DEBUG,("re-alloc in to big block"));
#ifdef MALLOCTRACE
		write_stack ("x");
#endif
		goto out;
	}
	if (ptr->smallsize >= realsize) {
#ifdef MALLOCTRACE
		log_realloc (ptr->smallsize,realsize,ptr->smallsize,s);
#endif
		return (s);
	}
	/* see if next block is free */
	if ((next = next_free_block(ptr)) != (struct header *) 0) {
		struct header * top;
		top = next;
		/* join with other free blocks */
		while ((next = next_free_block(top)) != (struct header *) 0) {
			if (ushort_add (&top->smallsize, next->smallsize) != 0) {
				LLOG (log_dsap,LLOG_EXCEPTIONS,("realloc size overflow"));
				goto out;
			}
			if (remove_free_ent (((struct freehead *)next)->flist) != 0)
				goto out;
		}
		if (remove_free_ent (((struct freehead *)top)->flist) != 0)
			goto out;
		/* is it big enough ? */
		if (ptr->smallsize + top->smallsize >= realsize) {
#ifdef MALLOCTRACE
			size_t savesize;
			savesize = ptr->smallsize;
#endif
			if (ushort_add (&ptr->smallsize, top->smallsize) != 0
					|| use_block (ptr, realsize) != 0)
				goto out;
#ifdef MALLOCTRACE
			log_realloc (savesize,realsize,ptr->smallsize,s);
#endif
			return (s);
		} else
			/* return to free list */
			add_free (top);
	}
out:
	;
	if ((mem = malloc (n)) == (char *)0)
		return ((char *)0);
	copysize -= ALIGN (sizeof (struct header));
	copysize = MIN (copysize, n);
	{
		int ncopy;

		if (sizet2int (copysize, &ncopy) != 0
				|| bcopy_int (s, mem, ncopy) != 0) {
			free (mem);
			return ((char *)0);
		}
	}
	free (s);
	return (mem);
}

MALLOC_RETURN
#ifdef lint
x_calloc(size_t n, size_t size)
#else
calloc(size_t n, const size_t size)
#endif
{
	char * mem;
	size_t x;
	x = n * size;
	if ((mem = malloc (x)) == (char *)0)
		return ((char *)0);
	{
		int nz;

		if (sizet2int (x, &nz) != 0 || bzero_int (mem, nz) != 0) {
			free (mem);
			return ((char *)0);
		}
	}
	return (mem);
}

#ifndef lint
FREE_RETURN cfree(char *mem);
#endif

FREE_RETURN
#ifdef lint
x_cfree(char *mem)
#else
cfree(char *mem)
#endif
{
	free(mem);
}

#endif	/* QUIPUMALLOC */
