/* set_del.c - remove member from set */







#include <stdio.h>
#include "psap.h"

int set_del (PE pe, PElementClass class, PElementID id) {
	int    pe_id;
	PE	   *p,
	 q;

	pe_id = PE_ID (class, id);
	for (p = &pe -> pe_cons; q = *p; p = &q -> pe_next)
		if (PE_ID (q -> pe_class, q -> pe_id) == pe_id) {
			(*p) = q -> pe_next;
			pe_free (q);
			return OK;
		}

	return pe_seterr (pe, PE_ERR_MBER, NOTOK);
}
