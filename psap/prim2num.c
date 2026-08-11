/* prim2num.c - presentation element to integer */







#include <stdio.h>
#include "psap.h"

integer
prim2num (PE pe) {
	integer    i;
	PElementData dp,
				 ep;

	if (pe -> pe_form != PE_FORM_PRIM || pe -> pe_prim == NULLPED)
		return pe_seterr (pe, PE_ERR_PRIM, NOTOK);
	if (pe -> pe_len > sizeof (i))
		return pe_seterr (pe, PE_ERR_OVER, NOTOK);

	pe -> pe_errno = PE_ERR_NONE;/* in case integer is NOTOK-valued */
	dp = pe -> pe_prim;
	if (pe -> pe_len > 1 && ((*dp == 0 && ((*(dp+1)) & 0x80) == 0) ||
							 (*dp == 0xff && ((*(dp + 1)) & 0x80) == 0x80)))
		return pe_seterr (pe, PE_ERR_SYNTAX, NOTOK);
	i = ((*dp) & 0x80) ? (-1) : 0;
	for (ep = dp + pe -> pe_len; dp < ep;)
		i = (i << 8) | (*dp++ & 0xff);

	return i;
}
