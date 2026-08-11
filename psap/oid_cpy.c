/* oid_cpy.c - copy an object identifier */

#include <stdio.h>
#include "psap.h"

OID	oid_cpy (q)
OID q;
{
	unsigned int   i,
			 *ip,
			 *jp;
	OID	oid;

	if (q == NULLOID)
		return NULLOID;
	if ((i = q -> oid_nelem) < 1)
		return NULLOID;
	if ((oid = (OID) malloc (sizeof *oid)) == NULLOID)
		return NULLOID;

	if ((ip = (unsigned int *) malloc ((unsigned) (i + 1) * sizeof *ip))
			== NULL) {
		free ((char *) oid);
		return NULLOID;
	}

	oid -> oid_elements = ip, oid -> oid_nelem = i;

	for (i = 0, jp = q -> oid_elements; i < oid -> oid_nelem; i++, jp++)
		*ip++ = *jp;

	return oid;
}
