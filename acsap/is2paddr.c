/* is2paddr.c - old-style P-ADDR lookup */

#include <string.h>
#include <strings.h>
#include "psap.h"
#include "isoaddrs.h"
#include "isoservent.h"

struct PSAPaddr* is2paddr (char *host, char *service, struct isoservent *is) {
	AEI	    aei;
	struct PSAPaddr *pa;

	if ((aei = str2aei (host, service)) == NULLAEI || (pa = aei2addr (aei)) == NULLPA)
		return NULLPA;

	if (is && strcmp (is -> is_provider, "psap") == 0) {
		if (is -> is_selectlen > PSSIZE)	/* XXX */
			return NULLPA;

		bcopy (is -> is_selector, pa -> pa_selector, pa -> pa_selectlen = is -> is_selectlen);
	}

	return pa;
}
