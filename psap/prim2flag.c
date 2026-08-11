/* prim2flag.c - presentation element to boolean */

#include <stdio.h>
#include "psap.h"

int prim2flag (PE pe) {
	if (pe -> pe_form != PE_FORM_PRIM
			|| pe -> pe_prim == NULLPED
			|| pe -> pe_len == 0)
		return pe_seterr (pe, PE_ERR_PRIM, NOTOK);

	return (*pe -> pe_prim != 0x00);
}
