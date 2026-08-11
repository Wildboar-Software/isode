/* qbuf2pe.c - read a PE from a SSDU */

#include <stdio.h>
#include "psap.h"
#undef	qbuf2pe
#include "tailor.h"

#ifndef	DEBUG
#endif

PE	qbuf2pe (qb, len, result)
struct qbuf *qb;
int	len;
int    *result;
{
#ifdef	notdef
	struct qbuf *qp;
#endif
	PE	    pe;
	PS	    ps;

#ifdef	notdef		/* "inline" nonsense too difficult to handle */
	if ((qp = qb -> qb_forw) != qb && qp -> qb_forw == qb) {
		remque (qp);

		return ssdu2pe (qp -> qb_data, qp -> qb_len, (char *) qp, result);
	}
#endif

	if ((ps = ps_alloc (qbuf_open)) == NULLPS) {
		*result = PS_ERR_NMEM;
		return NULLPE;
	}
	if (qbuf_setup (ps, qb) == NOTOK || (pe = ps2pe (ps)) == NULLPE) {
		if (ps -> ps_errno == PS_ERR_NONE)
			ps -> ps_errno = PS_ERR_EOF;
		*result = ps -> ps_errno;
		ps_free (ps);
		return NULLPE;
	}

	*result = PS_ERR_NONE;
	ps_free (ps);

#ifdef	DEBUG
	if (psap_log -> ll_events & LLOG_PDUS)
		pe2text (psap_log, pe, 1, len);
#endif

	return pe;
}
