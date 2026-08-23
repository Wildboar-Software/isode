/* oid_cpy.c - copy an object identifier */

#include <stdio.h>
#include <stdlib.h>
#include "psap.h"

OID	oid_cpy (OID q) {
	int    i;
	unsigned int   *ip,
			 *jp;
	OID	oid;
	size_t n,
		bytes;

	if (q == NULLOID)
		return NULLOID;
	if ((i = q -> oid_nelem) < 1)
		return NULLOID;
	if (int2sizet (i, &n) != 0)
		return NULLOID;
	if (n > (SIZE_MAX / sizeof *ip) - 1)
		return NULLOID;
	bytes = (n + 1) * sizeof *ip;
	if ((oid = (OID) malloc (sizeof *oid)) == NULLOID)
		return NULLOID;

	if ((ip = (unsigned int *) malloc (bytes)) == NULL) {
		free ((char *) oid);
		return NULLOID;
	}

	oid -> oid_elements = ip, oid -> oid_nelem = i;

	for (i = 0, jp = q -> oid_elements; i < oid -> oid_nelem; i++, jp++)
		*ip++ = *jp;

	return oid;
}
