/* ps_flush.c - flush a presentation stream */







#include <stdio.h>
#include "psap.h"

int ps_flush (PS ps) {
	if (ps -> ps_flushP)
		return (*ps -> ps_flushP) (ps);

	return OK;
}
