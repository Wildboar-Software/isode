/* obj2prim.c - object identifier to presentation element */

#include <stdio.h>
#include <stdlib.h>
#include "psap.h"

PE	obj2prim (OID o, const PElementClass class, const PElementID id) {
	int    i,
		   m,
		   n,
		   *mp,
		   *np;
	unsigned int j,
			 *ip;
	PElementData dp,
				 ep;
	PE	    pe;

	if (o == NULLOID
			|| o -> oid_nelem <= 1
			|| o -> oid_elements[0] > 2
			|| (o -> oid_elements[0] < 2 && o -> oid_elements[1] > 39))
		return NULLPE;

	if ((pe = pe_alloc (class, PE_FORM_PRIM, id)) == NULLPE)
		return NULLPE;

	if ((np = (int *) malloc_nmemb (o -> oid_nelem, sizeof *np))
			== NULL) {
		pe_free (pe);
		return NULLPE;
	}

	for (i = n = 0, ip = o -> oid_elements, mp = np;
			i < o -> oid_nelem;
			i++, ip++) {
		if (ip == o -> oid_elements)
			j = *ip++ * 40, i++, j+= *ip;
		else
			j = *ip;
		m = 0;
		do {
			m++;
			j >>= 7;
		} while (j);
		*mp++ = m;
		if (add_int_to_int (&n, m) != 0) {
			free ((char *) np);
			pe_free (pe);
			return NULLPE;
		}
	}

	if ((pe -> pe_prim = PEDalloc (pe -> pe_len = n)) == NULLPED) {
		free ((char *) np);
		pe_free (pe);
		return NULLPE;
	}

	dp = pe -> pe_prim;
	for (i = 0, ip = o -> oid_elements, mp = np;
			i < o -> oid_nelem;
			i++, ip++) {
		if (ip == o -> oid_elements)
			j = *ip++ * 40, i++, j += *ip;
		else
			j = *ip;

		ep = dp + (m = *mp++) - 1;
		for (dp = ep; m-- > 0; j >>= 7) {
			if (int2u8 ((int) ((j & 0x7f) | 0x80), dp) != 0) {
				free ((char *) np);
				pe_free (pe);
				return NULLPE;
			}
			dp--;
		}
		*ep = u8_bic (*ep, 0x80);
		dp = ep + 1;
	}

	free ((char *) np);

	return pe;
}
