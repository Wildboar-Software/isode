/* ps_free.c - free a presentation stream */

#include <stdio.h>
#include "psap.h"

void ps_free (PS ps) {
	if (ps -> ps_closeP)
		(*ps -> ps_closeP) (ps);

	free ((char *) ps);
}
