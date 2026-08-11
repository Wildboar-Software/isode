/* qb2str.c - qbuf to string */

#include <stdio.h>
#include "psap.h"

char *qb2str (struct qbuf *q) {
	int    len;
	char  *b,
		  *d;
	struct qbuf   *p;

	p = q -> qb_forw, len = 0;
	do {
		len += p -> qb_len;

		p = p -> qb_forw;
	} while (p != q);
	q -> qb_len = len;

	if ((b = d = malloc ((unsigned) (len + 1))) == NULL)
		return NULLCP;

	p = q -> qb_forw;
	do {
		bcopy (p -> qb_data, d, p -> qb_len);
		d += p -> qb_len;

		p = p -> qb_forw;
	} while (p != q);
	*d = 0;

	return b;
}
