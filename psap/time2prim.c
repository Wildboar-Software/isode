/* time2prim.c - time string to presentation element */







#include <string.h>
#include "psap.h"

PE
time2prim (UTC u, int generalized, PElementClass class, PElementID id) {
	int    len;
	char  *bp;
	PE	    pe;

	if ((bp = time2str (u, generalized)) == NULLCP)
		return NULLPE;

	if ((pe = pe_alloc (class, PE_FORM_PRIM, id)) == NULLPE)
		return NULLPE;

	if ((pe -> pe_prim = PEDalloc (len = strlen (bp))) == NULLPED) {
		pe_free (pe);
		return NULLPE;
	}
	PEDcpy (bp, pe -> pe_prim, pe -> pe_len = len);

	return pe;
}
