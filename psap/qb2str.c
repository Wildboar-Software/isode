/* qb2str.c - qbuf to string */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "psap.h"

char *qb2str (struct qbuf *q) {
	int    len;
	int    nbytes;
	char  *b,
		  *d;
	struct qbuf   *p;

	p = q -> qb_forw, len = 0;
	do {
		if (add_int_to_int (&len, p -> qb_len) != 0)
			return NULLCP;

		p = p -> qb_forw;
	} while (p != q);
	q -> qb_len = len;

	nbytes = len;
	if (add_int_to_int (&nbytes, 1) != 0
			|| (b = d = malloc_int (nbytes)) == NULL)
		return NULLCP;

	p = q -> qb_forw;
	do {
		if (bcopy_int (p -> qb_data, d, p -> qb_len) != 0) {
			free (b);
			return NULLCP;
		}
		d += p -> qb_len;

		p = p -> qb_forw;
	} while (p != q);
	*d = 0;

	return b;
}
