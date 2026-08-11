/* seq_del.c - delete a member from a sequence */







#include <stdio.h>
#include "psap.h"

int seq_del (PE pe, int i) {
	int	    offset;
	PE	   *p,
	 q;

	for (p = &pe -> pe_cons, offset = 0;
			q = *p;
			p = &q -> pe_next, offset = q -> pe_offset)
		if (q -> pe_offset == i) {
			if (((*p) = q -> pe_next) == NULLPE)
				pe -> pe_cardinal = offset + 1;
			pe_free (q);
			return OK;
		} else if (q -> pe_offset > i)
			break;

	return pe_seterr (pe, PE_ERR_MBER, NOTOK);
}
