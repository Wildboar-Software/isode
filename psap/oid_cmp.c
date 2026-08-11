/* oid_cmp.c - compare two object identifiers */

#include <stdio.h>
#include "psap.h"

int	oid_cmp (OID p, OID q) {
	if (p == NULLOID)
		return (q ? -1 : 0);

	return elem_cmp (p -> oid_elements, p -> oid_nelem,
					 q -> oid_elements, q -> oid_nelem);
}

int elem_cmp (unsigned int *ip, int i, unsigned int *jp, int j) {
	while (i > 0) {
		if (j == 0)
			return 1;
		if (*ip > *jp)
			return 1;
		else if (*ip < *jp)
			return (-1);

		ip++, i--;
		jp++, j--;
	}
	return (j == 0 ? 0 : -1);
}
