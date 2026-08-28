/* real2prim.c - real to presentation element */
/*
 * Contributed by Julian Onions, Nottingham University.
 */
#include <stdlib.h>
#include "psap.h"

PE
real2prim (const double d, const PElementClass class, const PElementID id) {
	PE pe;
	double	mant, nm;
	int	exponent;
	int	expsign;
	int	parts[sizeof (double)];
	int	sign, i, maxi, mask;
	int	n, explen;
	PElementData	dp;

	if ((pe = pe_alloc (class, PE_FORM_PRIM, id)) == NULLPE)
		return NULLPE;

	if (d == 0.0)
		return pe;

	mant = frexp (d, &exponent);

	if (mant < 0.0) {
		sign = -1;
		mant = -mant;
	} else	sign = 1;

	nm = mant;
	{
		int nd;

		if (sizet2int (sizeof (double), &nd) != 0) {
			pe_free (pe);
			return NULLPE;
		}
		for (i = 0; i < nd; i++) {
			int intnm;
			nm *= (1<<8);
			if (double2int (nm, &intnm) != 0) {
				pe_free (pe);
				return NULLPE;
			}
			intnm &= 0xff;
			nm -= intnm;
			if (intnm)
				maxi = i + 1;
			parts[i] = intnm;
		}
	}

	exponent -= 8 * maxi;

	expsign = exponent >= 0 ? exponent : exponent ^ (-1);
	{
		int nbytes;

		if (sizet2int (sizeof exponent, &nbytes) != 0) {
			pe_free (pe);
			return NULLPE;
		}
		n = nbytes;
	}
	mask = 0x1ff << ((n - 1) * 8 - 1);
	while (n > 1 && (expsign & mask) == 0)
		mask >>= 8, n--;

	explen = n;
	if (n > 3)
		n ++;

	if ((pe -> pe_prim = PEDalloc (n + maxi + 1)) == NULLPED) {
		pe_free (pe);
		return NULLPE;
	}

	dp = pe -> pe_prim + (pe -> pe_len = n + maxi + 1);

	for (; maxi > 0; maxi --)
		if (int2u8 (parts[maxi - 1], --dp) != 0) {
			pe_free (pe);
			return NULLPE;
		}
	for (n = explen; n-- > 0; exponent >>= 8)
		if (int2u8 (exponent & 0xff, --dp) != 0) {
			pe_free (pe);
			return NULLPE;
		}
	if (explen > 3)
		if (int2u8 (explen & 0xff, --dp) != 0) {
			pe_free (pe);
			return NULLPE;
		}

	switch (explen) {
	case 1:
		explen = PE_REAL_B_EF1;
		break;
	case 2:
		explen = PE_REAL_B_EF2;
		break;
	case 3:
		explen = PE_REAL_B_EF3;
		break;
	default:
		explen = PE_REAL_B_EF3;
		break;
	}
	if (int2u8 (PE_REAL_BINENC
			| PE_REAL_B_B2
			| (sign == -1 ? PE_REAL_B_S : 0)
			| explen, --dp) != 0) {
		pe_free (pe);
		return NULLPE;
	}
	return pe;
}
