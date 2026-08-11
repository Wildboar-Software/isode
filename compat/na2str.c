/* na2str.c - pretty-print NSAPaddr */







#include <stdio.h>
#include "general.h"
#include "manifest.h"
#include "isoaddrs.h"

/*    Network Address to String */

char *na2str (struct NSAPaddr *na) {
	switch (na -> na_stack) {
	case NA_TCP:
		return na -> na_domain;

	case NA_X25:
		return na -> na_dte;

	case NA_NSAP:
	default:
		return sel2str (na -> na_address, na -> na_addrlen, 0);
	}
}
