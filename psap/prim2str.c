/* prim2str.c - presentation element to octet string */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "psap.h"

/* Similar to pe_pullup.  Returns a newly allocated string, composed of
   of any sub-elements in pe, whereas pe_pullup always reverts "pe" to
   a primitive.  The string is null-terminated, though pe_len specifically
   does NOT reflect this. */

char *prim2str (PE pe, int *len) {
	int    i,
		   k;
	int     j;
	char  *dp,
		  *ep,
		  *fp;
	PElementClass class;
	PElementID id;
	PE	    p;

	*len = 0;
	switch (pe -> pe_form) {
	case PE_FORM_PRIM: {
		int nbytes;

		i = pe -> pe_len;
		nbytes = i;
		if (add_int_to_int (&nbytes, 1) != 0
				|| (dp = malloc_int (nbytes)) == NULLCP)
			return pe_seterr (pe, PE_ERR_NMEM, NULLCP);
		if (bcopy_int (pe -> pe_prim, dp, i) != 0) {
			free (dp);
			return pe_seterr (pe, PE_ERR_NMEM, NULLCP);
		}
		break;
	}

	case PE_FORM_CONS:
		if ((p = pe -> pe_cons) == NULLPE) {
			if ((dp = malloc_int (1)) == NULLCP)
				return pe_seterr (pe, PE_ERR_NMEM, NULLCP);
			break;
		}
		dp = NULLCP, i = 0;
		class = p -> pe_class, id = p -> pe_id;
		for (p = pe -> pe_cons; p; p = p -> pe_next) {
			if ((p -> pe_class != class || p -> pe_id != id)
					&& (p -> pe_class != PE_CLASS_UNIV
						|| p -> pe_id != PE_PRIM_OCTS)) {
				if (dp)
					free (dp);
				return pe_seterr (pe, PE_ERR_TYPE, NULLCP);
			}

			if ((ep = prim2str (p, &j)) == NULLCP) {
				if (dp)
					free (dp);
				return pe_seterr (pe, PE_ERR_NMEM, NULLCP);
			}
			if (dp) {
				int nbytes;

				k = i;
				if (add_int_to_int (&k, j) != 0) {
					free (dp);
					free (ep);
					return pe_seterr (pe, PE_ERR_NMEM, NULLCP);
				}
				nbytes = k;
				if (add_int_to_int (&nbytes, 1) != 0
						|| (fp = realloc_int (dp, nbytes))
						== NULLCP) {
					free (dp);
					free (ep);
					return pe_seterr (pe, PE_ERR_NMEM, NULLCP);
				}
				if (bcopy_int (ep, fp + i, j) != 0) {
					free (fp);
					free (ep);
					return pe_seterr (pe, PE_ERR_NMEM, NULLCP);
				}
				dp = fp, i = k;
			} else {
				dp = ep;
				if (add_int_to_int (&i, j) != 0) {
					free (dp);
					return pe_seterr (pe, PE_ERR_NMEM, NULLCP);
				}
			}
		}
		break;
	}

	if (dp)
		dp[*len = i] = 0;

	return dp;
}
