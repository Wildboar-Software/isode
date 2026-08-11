/* pe_extract.c - extract a PE */







#include <stdio.h>
#include "psap.h"

/* assumes that q appears at most once directly under p... */

int pe_extract (PE pe, PE r) {
	PE	   *p,
	 q;

	switch (pe -> pe_form) {
	case PE_FORM_PRIM:
	case PE_FORM_ICONS:
		break;

	case PE_FORM_CONS:
		for (p = &pe -> pe_cons; q = *p; p = &q -> pe_next)
			if (q == r) {
				(*p) = q -> pe_next;
				q -> pe_next = NULLPE;
				if (r->pe_refcnt > 0)
					r->pe_refcnt--;
				return 1;
			} else if (pe_extract (q, r))
				return 1;
		break;
	}

	return 0;
}
