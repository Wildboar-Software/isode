/* smalloc.c - error checking malloc */

#include <stdlib.h>
#include <unistd.h>
#include "general.h"
#include "manifest.h"
static void default_smalloc_handler (void);


static void default_smalloc_handler (void) {
	abort ();
}

static void (*smalloc_handler)(void) = default_smalloc_handler;

void (*set_smalloc_handler (void (*fnx)(void)))(void) {
	void (*savefnx)(void) = smalloc_handler;

	if (fnx)
		smalloc_handler = fnx;
	else	smalloc_handler = default_smalloc_handler;
	return savefnx;
}

char *smalloc (int size) {
	char *ptr;
	size_t n;

	if (int2sizet (size, &n) != 0) {
		(*smalloc_handler) ();
		_exit(1);
	}
	if ((ptr = malloc(n)) == (char *)0) {
		(*smalloc_handler) ();
		_exit(1);	/* just in case */
	}

	return(ptr);
}
