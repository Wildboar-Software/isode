/* pe_cmp.c - compare two presentation elements */

#include <stdio.h>
#include "psap.h"

int pe_cmp (PE p, PE q) {
	int    i;

	if (p == NULLPE)
		return (q ? 1 : 0);
	if (q == NULLPE
			|| p -> pe_class != q -> pe_class
			|| p -> pe_form != q -> pe_form
			|| p -> pe_id != q -> pe_id)
		return 1;

	/* XXX: perhaps compare pe_context ??? */

	switch (p -> pe_form) {
	case PE_FORM_ICONS:
		if (p -> pe_ilen != q -> pe_ilen)
			return 1;
	/* else fall */
	case PE_FORM_PRIM:
		if (i = p -> pe_len) {
			if (i != q -> pe_len || PEDcmp (p -> pe_prim, q -> pe_prim, i))
				return 1;
		} else if (q -> pe_len)
			return 1;
		return 0;

	case PE_FORM_CONS:
		for (p = p -> pe_cons, q = q -> pe_cons;
				p;
				p = p -> pe_next, q = q -> pe_next)
			if (pe_cmp (p, q))
				return 1;
		return (q ? 1 : 0);

	default:		/* XXX */
		return 1;
	}
}
