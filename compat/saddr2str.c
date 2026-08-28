/* saddr2str.c - SSAPaddr to string value */

#include <stdio.h>
#include <strings.h>
#include "general.h"
#include "manifest.h"
#include "isoaddrs.h"

char *saddr2str (const struct SSAPaddr *sa) {
	struct PSAPaddr pas;
	struct PSAPaddr *pa = &pas;

	if (!sa)
		return NULL;
	bzero ((char *) pa, sizeof *pa);
	pa -> pa_addr = *sa;		/* struct copy */

	return paddr2str (pa, NULLNA);
}
