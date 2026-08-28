/* ftambits.c - FPM: encode/decode BIT STRINGs */

#include <stdio.h>
#include "fpkt.h"

PE
bits2fpm (const struct ftamblk *fsb, struct pair pairs[], const int actions, struct FTAMindication *fti) {
	struct pair *pp;
	PE	    fpm;

	if ((fpm = prim2bit (pe_alloc (PE_CLASS_UNIV, PE_FORM_PRIM, PE_PRIM_BITS)))
			== NULLPE) {
no_mem:
		;
		ftamlose (fti, FS_GEN (fsb), 1, NULLCP, "out of memory");
		if (fpm)
			pe_free (fpm);
		return NULLPE;
	}
	for (pp = pairs; pp -> p_mask; pp++)
		if ((actions & pp -> p_mask) && bit_on (fpm, pp -> p_bitno) == NOTOK)
			goto no_mem;
	return fpm;
}

int fpm2bits (const struct ftamblk *fsb, struct pair pairs[], PE fpm, int *actions, struct FTAMindication *fti) {
	int    i;
	int	bits_set=0;
	struct pair *pp;

	i = 0;
	for (pp = pairs; pp -> p_mask; pp++)
		if (bit_test (fpm, pp -> p_bitno) > OK) {
			i |= pp -> p_mask;
			bits_set++;
		}
	*actions = i;
	if (bits_set == 0)
		return NOTOK;
	else
		return (bits_set);
}
