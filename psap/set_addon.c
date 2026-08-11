/* set_addon.c - add member to end of a set */







#include <stdio.h>
#include "psap.h"

int set_addon (PE pe, PE last, PE new) {
	if (pe == NULLPE)
		return NOTOK;
	if (last == NULLPE)
		return set_add (pe, new);
	new -> pe_offset = pe -> pe_cardinal++;
	last -> pe_next = new;

	return OK;
}
