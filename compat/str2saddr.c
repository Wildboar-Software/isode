/* str2saddr.c - string value to SSAPaddr */

#include <stdio.h>
#include "general.h"
#include "manifest.h"
#include "isoaddrs.h"

struct SSAPaddr *
str2saddr (char *str) {
	struct PSAPaddr *pa;

	if (pa = str2paddr (str))
		return (&pa -> pa_addr);

	return NULLSA;
}
