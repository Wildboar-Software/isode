/* qbuf2ps.c - qbuf-backed abstractions for PStreams */

#include <stdio.h>
#include <stdlib.h>
#include <search.h>
#include <strings.h>
#include "psap.h"
static int qbuf_read (PS ps, PElementData data, PElementLen n, int in_line);
static int qbuf_close (PS ps);


static int qbuf_read (PS ps, PElementData data, PElementLen n, int in_line) {
	int cc,
		i;
	struct qbuf *qb,
			   *qp;

	if ((qb = (struct qbuf *) ps -> ps_addr) == NULL)
		return 0;

	for (qp = NULL, cc = 0; n > 0; data += i, cc += i, n -= i) {
		if (qp == NULL && (qp = qb -> qb_forw) == qb)
			return cc;

		i = min (qp -> qb_len, n);
		if (bcopy_int (qp -> qb_data, data, i) != 0)
			return cc;

		qp -> qb_data += i, qp -> qb_len -= i;
		if (qp -> qb_len <= 0) {
			remque (qp);

			free ((char *) qp);
			qp = NULL;
		}
	}

	return cc;
}

static int qbuf_close (PS ps) {
	struct qbuf *qb;
	if ((qb = (struct qbuf *) ps -> ps_addr) == NULL)
		return OK;
	QBFREE (qb);
	return OK;
}

int qbuf_open (PS ps) {
	ps -> ps_readP = qbuf_read;
	ps -> ps_closeP = qbuf_close;
	return OK;
}
