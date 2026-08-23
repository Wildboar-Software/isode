/* qb_pullup.c - "pullup" a list of qbufs */

#include <stdlib.h>
#include <search.h>
#include <strings.h>
#include "psap.h"

int qb_pullup (struct qbuf *qb) {
	int len = 0;
	int extra;
	char *d;
	struct qbuf *p, *qp, *qpp;
#ifdef	notdef		/* want null-termination... */
	if ((p = qb -> qb_forw) -> qb_forw == qb)
		return OK;
#endif
	for (p = qb -> qb_forw; p != qb; p = p -> qb_forw)
		if (add_int_to_int (&len, p -> qb_len) != 0)
			return NOTOK;
	extra = len;
	if (add_int_to_int (&extra, 1) != 0)
		return NOTOK;
	if ((p = (struct qbuf *) malloc_plus_int (sizeof *p, extra))
			== NULL)
		return NOTOK;
	d = p -> qb_data = p -> qb_base;
	p -> qb_len = len;
	for (qp = qb -> qb_forw; qp != qb; qp = qpp) {
		qpp = qp -> qb_forw;
		remque (qp);
		if (bcopy_int (qp -> qb_data, d, qp -> qb_len) != 0) {
			free ((char *) qp);
			free ((char *) p);
			return NOTOK;
		}
		d += qp -> qb_len;
		free ((char *) qp);
	}
	*d = 0;
	insque (p, qb);
	return OK;
}
