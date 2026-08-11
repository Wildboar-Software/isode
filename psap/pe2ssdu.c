/* pe2ssdu.c - write a PE to a SSDU */







#include <stdio.h>
#include "psap.h"
#include "tailor.h"

int pe2ssdu (PE pe, char **base, int *len) {
	int plen, ret;

	*len = 0;
	plen  = ps_get_abs (pe);
	Qcp = (char *)malloc((unsigned)plen);
	*base = Qcp;

	if (Qcp == NULLCP)
		return NOTOK;

	Len = 0;
	Ecp = Qcp + plen;
	if ((ret = pe2qb_f(pe)) != plen) {
		printf("pe2ssdu: bad length returned %d should be %d\n",
			   ret, plen);
		return NOTOK;
	}
	*len = plen;

#ifdef	DEBUG
	if (psap_log -> ll_events & LLOG_PDUS)
		pe2text (psap_log, pe, 0, *len);
#endif

	return OK;
}
