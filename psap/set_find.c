/* set_find.c - find member of a set */







#include <stdio.h>
#include "psap.h"

PE
set_find (PE pe, PElementClass class, PElementID id) {
	int    pe_id;
	PE	    p;
	PE pfound = NULLPE;

	pe_id = PE_ID (class, id);
	for (p = pe -> pe_cons; p; p = p -> pe_next)
		if (PE_ID (p -> pe_class, p -> pe_id) == pe_id) {
			if (pfound == NULLPE)
				pfound = p;
			else
				return pe_seterr (pe, PE_ERR_DUPLICATE, NULLPE);
		}
	return pfound;
}
