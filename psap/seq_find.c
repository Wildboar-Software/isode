/* seq_find.c - find an element in a sequence */







#include <stdio.h>
#include "psap.h"

PE
seq_find (PE pe, int i) {
	PE	    p;

	if (i >= pe -> pe_cardinal)
		return pe_seterr (pe, PE_ERR_MBER, NULLPE);

	for (p = pe -> pe_cons; p; p = p -> pe_next)
		if (p -> pe_offset == i)
			break;

	return p;
}
