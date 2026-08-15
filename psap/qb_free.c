/* qb_free.c - free a list of qbufs */

#include <stdio.h>
#include <stdlib.h>
#include "psap.h"

void qb_free (struct qbuf *qb) {
	QBFREE (qb);
	free ((char *) qb);
}
