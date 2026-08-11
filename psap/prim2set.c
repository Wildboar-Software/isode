/* prim2flag.c - presentation element to set */

#include <stdio.h>
#include "psap.h"

PE
prim2set (PE pe) {
	int    i;
	PE	    p;

	if (pe -> pe_form != PE_FORM_CONS)
		return pe_seterr (pe, PE_ERR_CONS, NULLPE);

	for (i = 0, p = pe -> pe_cons; p; p = p -> pe_next)
		p -> pe_offset = i++;

	pe -> pe_cardinal = i;

	return pe;
}
