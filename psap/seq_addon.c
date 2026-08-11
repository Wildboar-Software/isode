/* seq_addon.c - add a member to the end of a sequence (efficiency hack) */

#include <stdio.h>
#include "psap.h"

int seq_addon (PE pe, PE last, PE new) {
	if (pe == NULLPE)
		return NOTOK;
	if (last == NULLPE)
		return seq_add (pe, new, -1);
	new -> pe_offset = pe -> pe_cardinal++;
	last -> pe_next = new;
	return OK;
}
