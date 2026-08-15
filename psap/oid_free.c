/* oid_free.c - free an object identifier */

#include <stdio.h>
#include <stdlib.h>
#include "psap.h"

void oid_free (OID oid) {
	if (oid == NULLOID)
		return;

	if (oid -> oid_elements)
		free ((char *) oid -> oid_elements);

	free ((char *) oid);
}
