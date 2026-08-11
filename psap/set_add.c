/* set_add.c - add member to set */







#include <stdio.h>
#include "psap.h"

int set_add (PE pe, PE r) {
	int     pe_id;
	PE	    *p,
	 q;

	if (r == NULLPE)
		return pe_seterr (pe, PE_ERR_NMEM, NOTOK);

	pe_id = PE_ID (r -> pe_class, r -> pe_id);
	for (p = &pe -> pe_cons; q = *p; p = &q -> pe_next)
		if (PE_ID (q -> pe_class, q -> pe_id) == pe_id)
			return pe_seterr (pe, PE_ERR_DUPLICATE, NOTOK);

	*p = r;
	return OK;
}
