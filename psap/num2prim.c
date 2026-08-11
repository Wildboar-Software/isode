/* num2prim.c - integer to presentation element */

#include <stdio.h>
#include "psap.h"

PE
num2prim (integer i, int class, int id) {
	static const integer MASK = 0x1ff;
	integer mask, sign, n;
	PElementData dp;
	PE	    pe;

	if ((pe = pe_alloc (class, PE_FORM_PRIM, id)) == NULLPE)
		return NULLPE;

	sign = i >= 0 ? i : i ^ (-1);
	mask = MASK << (((n = sizeof i) - 1) * 8 - 1);
	while (n > 1 && (sign & mask) == 0)
		mask >>= 8, n--;

	if ((pe -> pe_prim = PEDalloc (n)) == NULLPED) {
		pe_free (pe);
		return NULLPE;
	}

	for (dp = pe -> pe_prim + (pe -> pe_len = n); n-- > 0; i >>= 8)
		*--dp = i & 0xff;

	return pe;
}
