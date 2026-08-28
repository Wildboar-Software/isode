/* str2taddr.c - string value to TSAPaddr */

#include <stdio.h>
#include "general.h"
#include "manifest.h"
#include "isoaddrs.h"

struct TSAPaddr *
str2taddr (const char *str) {
	struct PSAPaddr *pa;

	if (pa = str2paddr (str))
		return (&pa -> pa_addr.sa_addr);

	return NULLTA;
}
