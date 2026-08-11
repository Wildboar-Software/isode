/* pe_expunge.c - expunge a PE */







#include <stdio.h>
#include "psap.h"

PE
pe_expunge (PE pe, PE r) {
	if (r) {
		if (pe == r)
			return r;

		if (pe_extract (pe, r))
			if (pe -> pe_realbase && !r -> pe_realbase) {
				r -> pe_realbase = pe -> pe_realbase;
				pe -> pe_realbase = NULL;
			}

		r -> pe_refcnt++;
	}

	pe_free (pe);

	return r;
}
