/* qb_pullup.c - "pullup" a list of qbufs */

#include <stdlib.h>
#include <search.h>
#include <strings.h>
#include "psap.h"

int qb_pullup (struct qbuf *qb) {
	int len = 0;
	char *d;
	struct qbuf *p, *qp, *qpp;
#ifdef	notdef		/* want null-termination... */
	if ((p = qb -> qb_forw) -> qb_forw == qb)
		return OK;
#endif
	for (p = qb -> qb_forw; p != qb; p = p -> qb_forw)
		len += p -> qb_len;
	if ((p = (struct qbuf *) malloc ((unsigned) (sizeof *p + len + 1)))
			== NULL)
		return NOTOK;
	d = p -> qb_data = p -> qb_base;
	p -> qb_len = len;
	for (qp = qb -> qb_forw; qp != qb; qp = qpp) {
		qpp = qp -> qb_forw;
		remque (qp);
		bcopy (qp -> qb_data, d, qp -> qb_len);
		d += qp -> qb_len;
		free ((char *) qp);
	}
	*d = 0;
	insque (p, qb);
	return OK;
}
