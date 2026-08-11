/* bit2prim.c - bit string to presentation element */







#include <stdio.h>
#include "psap.h"

static PE	bit2prim_aux ();

PE
bit2prim (PE pe) {
	if (pe == NULLPE)
		return NULLPE;

	switch (pe -> pe_form) {
	case PE_FORM_PRIM:
		if (pe -> pe_prim == NULLPED) {
			if ((pe -> pe_prim = PEDalloc (1)) == NULLPED)
				return NULLPE;
			pe -> pe_len = 1;
			pe -> pe_nbits = 0;
		}
	/* and fall */

	case PE_FORM_CONS:
		if (bit2prim_aux (pe) == NULLPE)
			return NULLPE;
		break;
	}

	return pe;
}

static PE
bit2prim_aux (PE pe) {
	int	    i;
	PE	    p;

	if (pe == NULLPE)
		return NULLPE;

	switch (pe -> pe_form) {
	case PE_FORM_PRIM:
		if (pe -> pe_prim && pe -> pe_len) {
			if ((i = (((pe -> pe_len - 1) * 8) - pe -> pe_nbits)) > 7)
				return pe_seterr (pe, PE_ERR_BITS, NULLPE);
			pe -> pe_prim[0] = i & 0xff;

		}
		break;

	case PE_FORM_CONS:
		for (p = pe -> pe_cons; p; p = p -> pe_next)
			if (bit2prim (p) == NULLPE)
				return NULLPE;
		break;
	}

	return pe;
}
