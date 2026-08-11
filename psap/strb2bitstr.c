/* strb2bitstr.c - string of bits to bit string */

#include <stdio.h>
#include "psap.h"

PE
strb2bitstr (char *cp, int len, PElementClass class, PElementID id) {
	int    i,
		   j,
		   bit,
		   mask;
	PE	    p;

	if ((p = pe_alloc (class, PE_FORM_PRIM, id)) == NULLPE)
		return NULLPE;

	p = prim2bit (p);
	if (len > 0 && bit_off (p, len - 1) == NOTOK) {
no_mem:
		;
		pe_free (p);
		return NULLPE;
	}

	for (bit = (*cp & 0xff), i = 0, mask = 1 << (j = 7); i < len; i++) {
		if ((bit & mask) && bit_on (p, i) == NOTOK)
			goto no_mem;
		if (j-- == 0)
			bit = *++cp & 0xff, mask = 1 << (j = 7);
		else
			mask >>= 1;
	}

	return p;
}
