/* flag2prim.c - boolean to presentation element */

#include <stdio.h>
#include <stdlib.h>
#include "psap.h"

PE
flag2prim (int b, int class, int id) {
	PE	    pe;
	PElementClass cl;
	PElementID pid;

	if (int2u8 (class, &cl) != 0 || int2u16 (id, &pid) != 0)
		return NULLPE;

	if ((pe = pe_alloc (cl, PE_FORM_PRIM, pid)) == NULLPE)
		return NULLPE;

	if ((pe -> pe_prim = PEDalloc (pe -> pe_len = 1)) == NULLPED) {
		pe_free (pe);
		return NULLPE;
	}

	if (int2u8 (b ? 0xff : 0x00, pe -> pe_prim) != 0) {
		pe_free (pe);
		return NULLPE;
	}

	return pe;
}
