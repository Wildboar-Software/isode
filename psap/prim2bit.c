/* prim2bit.c - presentation element to bit string */







#include <stdio.h>
#include "psap.h"

PE
prim2bit (PE pe) {
	int	    i;
	PElementData bp;
	PElementLen len;
	PE	    p;

	switch (pe -> pe_form) {
	case PE_FORM_PRIM:	/* very paranoid... */
		if ((bp = pe -> pe_prim) && (len = pe -> pe_len)) {
			if ((i = *bp & 0xff) > 7)
				return pe_seterr (pe, PE_ERR_BITS, NULLPE);
			pe -> pe_nbits = ((len - 1) * 8) - i;
		} else
			pe -> pe_nbits = 0;
		break;

	case PE_FORM_CONS:
		pe -> pe_nbits = 0;
		for (p = pe -> pe_cons; p; p = p -> pe_next) {
			if (prim2bit (p) == NULLPE)
				return NULLPE;
			pe -> pe_nbits += p -> pe_nbits;
		}
		break;
	}

	return pe;
}
