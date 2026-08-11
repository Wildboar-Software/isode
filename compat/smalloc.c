/* smalloc.c - error checking malloc */







#include <unistd.h>
#include "general.h"
#include "manifest.h"

static void
default_smalloc_handler (void) {
	abort ();
}

static VFP smalloc_handler = default_smalloc_handler;

VFP
set_smalloc_handler (VFP fnx) {
	VFP savefnx = smalloc_handler;

	if (fnx)
		smalloc_handler = fnx;
	else	smalloc_handler = default_smalloc_handler;
	return savefnx;
}

char *smalloc (int size) {
	char *ptr;

	if ((ptr = malloc((unsigned) size)) == (char *)0) {
		(*smalloc_handler) ();
		_exit(1);	/* just in case */
	}

	return(ptr);
}
