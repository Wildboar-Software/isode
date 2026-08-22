/* pe_pullup.c - "pullup" a presentation element */
#include <stdlib.h>
#include <strings.h>
#include "psap.h"

static PElementData pe_pullup_aux (PE pe, int *len);

int pe_pullup (PE pe) {
	PElementLen	    len;
	PElementData dp;
	PE	    p;

	if (pe -> pe_form != PE_FORM_CONS)
		return OK;
	if ((dp = pe_pullup_aux (pe, &len)) == NULLPED)
		return NOTOK;
	for (p = pe -> pe_cons; p; p = p -> pe_next)
		pe_free (p);
	pe -> pe_form = PE_FORM_PRIM;
	pe -> pe_len = len;
	pe -> pe_prim = dp;
	return OK;
}

static PElementData pe_pullup_aux (PE pe, int *len) {
	int    i,
		   k;
	int     j;
	PElementClass class;
	PElementID id;
	PElementData dp,
				 ep,
				 fp;
	PE	    p;

	switch (pe -> pe_form) {
	case PE_FORM_PRIM:
		if ((dp = PEDalloc (i = pe -> pe_len)) == NULLPED)
			return NULLPED;
		PEDcpy (pe -> pe_prim, dp, i);
		break;
	case PE_FORM_CONS:
		dp = NULLPED, i = 0;
		class = pe -> pe_class, id = pe -> pe_id;
		for (p = pe -> pe_cons; p; p = p -> pe_next) {
			if (p -> pe_class != class
					|| p -> pe_id != id
					|| (ep = pe_pullup_aux (p, &j)) == NULLPED) {
				if (dp)
					PEDfree (dp);
				return NULLPED;
			}
			if (dp) {
				if ((fp = PEDrealloc (dp, k = i + j)) == NULLPED) {
					PEDfree (dp);
					return NULLPED;
				}
				PEDcpy (ep, fp + i, j);
				dp = fp, i = k;
			} else
				dp = ep, i += j;
		}
		break;
	case PE_FORM_ICONS:
		return NULLPED;
	}
	*len = i;
	return (dp);
}
