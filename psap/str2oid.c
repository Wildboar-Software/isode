/* str2oid.c - string to object identifier */

#include <stdio.h>
#include "psap.h"

OID	str2oid (s)
char    *s;
{
	int	    i;
	static struct OIDentifier   oids;
	static unsigned int elements[NELEM + 1];

	if ((i = str2elem (s, elements)) < 1
			|| elements[0] > 2
			|| (i > 1 && elements[0] < 2 && elements[1] > 39))
		return NULLOID;

	oids.oid_elements = elements, oids.oid_nelem = i;

	return (&oids);
}
