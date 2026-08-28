/* time2prim.c - time string to presentation element */

#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "psap.h"

PE
time2prim (UTC u, const int generalized, const PElementClass class, const PElementID id) {
	int    len;
	char  *bp;
	PE	    pe;

	if ((bp = time2str (u, generalized)) == NULLCP)
		return NULLPE;

	if ((pe = pe_alloc (class, PE_FORM_PRIM, id)) == NULLPE)
		return NULLPE;

	if (strlen2int (bp, &len) != 0
			|| (pe -> pe_prim = PEDalloc (len)) == NULLPED) {
		pe_free (pe);
		return NULLPE;
	}
	PEDcpy (bp, pe -> pe_prim, pe -> pe_len = len);

	return pe;
}
