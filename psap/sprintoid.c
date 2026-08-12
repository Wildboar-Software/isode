/* sprintoid.c - object identifier to string */

#include <stdio.h>
#include <string.h>
#include "psap.h"

char *sprintoid (OID oid)
{
	int    i;
	unsigned int  *ip;
	char  *bp, *cp;
	static char buffer[BUFSIZ];

	if (oid == NULLOID || oid -> oid_nelem < 1)
		return "";
	bp = buffer;
	for (ip = oid -> oid_elements, i = oid -> oid_nelem, cp = "";
			i-- > 0;
			ip++, cp = ".") {
		sprintf (bp, "%s%u", cp, *ip);
		bp += strlen (bp);
	}
	return buffer;
}
