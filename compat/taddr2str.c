/* taddr2str.c - TSAPaddr to string value */

#include <stdio.h>
#include <strings.h>
#include "general.h"
#include "manifest.h"
#include "isoaddrs.h"

char *taddr2str (const struct TSAPaddr *ta) {
	struct PSAPaddr pas;
	struct PSAPaddr *pa = &pas;

	if (!ta)
		return NULL;
	bzero ((char *) pa, sizeof *pa);
	pa -> pa_addr.sa_addr = *ta;	/* struct copy */

	return paddr2str (pa, NULLNA);
}
