/* str2qb.c - string to qbuf */

#include <stdio.h>
#include "psap.h"

struct qbuf *str2qb (char *s, int len, int head) {
	struct qbuf *qb,
			   *pb;

	if ((pb = (struct qbuf *) malloc ((unsigned) (sizeof *pb + len))) == NULL)
		return NULL;

	if (head) {
		if ((qb = (struct qbuf *) malloc (sizeof *qb)) == NULL) {
			free ((char *) pb);
			return NULL;
		}
		qb -> qb_forw = qb -> qb_back = qb;
		qb -> qb_data = NULL, qb -> qb_len = len;
		insque (pb, qb);
	} else {
		pb -> qb_forw = pb -> qb_back = pb;
		qb = pb;
	}

	pb -> qb_data = pb -> qb_base;
	if ((pb -> qb_len = len) > 0 && s)
		bcopy (s, pb -> qb_data, len);

	return qb;
}
