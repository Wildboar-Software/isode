/* num2prim.c - integer to presentation element */

#include <stdio.h>
#include <stdlib.h>
#include "psap.h"

PE
num2prim (integer i, const int class, const int id) {
	static const integer MASK = 0x1ff;
	integer mask, sign, n;
	int len;
	PElementClass cl;
	PElementID pid;
	PElementData dp;
	PE	    pe;

	if (int2u8 (class, &cl) != 0 || int2u16 (id, &pid) != 0)
		return NULLPE;

	if ((pe = pe_alloc (cl, PE_FORM_PRIM, pid)) == NULLPE)
		return NULLPE;

	sign = i >= 0 ? i : i ^ (-1);
	{
		int nbytes;

		if (sizet2int (sizeof i, &nbytes) != 0) {
			pe_free (pe);
			return NULLPE;
		}
		n = nbytes;
	}
	mask = MASK << ((n - 1) * 8 - 1);
	while (n > 1 && (sign & mask) == 0)
		mask >>= 8, n--;

	if (int32_to_int (n, &len) != 0) {
		pe_free (pe);
		return NULLPE;
	}
	if ((pe -> pe_prim = PEDalloc (len)) == NULLPED) {
		pe_free (pe);
		return NULLPE;
	}

	pe -> pe_len = len;
	for (dp = pe -> pe_prim + pe -> pe_len; n-- > 0; i >>= 8) {
		int octet;

		if (int32_to_int (i & 0xff, &octet) != 0
				|| int2u8 (octet, --dp) != 0) {
			pe_free (pe);
			return NULLPE;
		}
	}

	return pe;
}
