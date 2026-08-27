/* str2prim.c - octet string to presentation element */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "psap.h"

PE
str2prim (char *s, const int len, const PElementClass class, const PElementID id) {
	PE	    pe;

	if ((pe = pe_alloc (class, PE_FORM_PRIM, id)) == NULLPE)
		return NULLPE;

	if (len && (pe -> pe_prim = PEDalloc (pe -> pe_len = len)) == NULLPED) {
		pe_free (pe);
		return NULLPE;
	}

	if (s)
		PEDcpy (s, pe -> pe_prim, len);

	return pe;
}
