/* flag2prim.c - boolean to presentation element */

#include <stdio.h>
#include <stdlib.h>
#include "psap.h"

PE
flag2prim (int b, int class, int id) {
	PE	    pe;

	if ((pe = pe_alloc (class, PE_FORM_PRIM, id)) == NULLPE)
		return NULLPE;

	if ((pe -> pe_prim = PEDalloc (pe -> pe_len = 1)) == NULLPED) {
		pe_free (pe);
		return NULLPE;
	}

	*pe -> pe_prim = b ? 0xff : 0x00;

	return pe;
}
