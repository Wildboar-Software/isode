/* is2saddr.c - old-style S-ADDR lookup */

#include <string.h>
#include <strings.h>
#include "psap.h"
#include "isoaddrs.h"
#include "isoservent.h"

struct SSAPaddr *
is2saddr (char *host, char *service, const struct isoservent *is) {
	AEI	    aei;
	struct PSAPaddr *pa;
	struct SSAPaddr *sa;

	if ((aei = str2aei (host, service)) == NULLAEI || (pa = aei2addr (aei)) == NULLPA)
		return NULLSA;

	sa = &pa -> pa_addr;
	if (is && strcmp (is -> is_provider, "ssap") == 0) {
		if (is -> is_selectlen > SSSIZE)	/* XXX */
			return NULLSA;

		if (bcopy_int (is -> is_selector, sa -> sa_selector,
					   is -> is_selectlen) != 0)
			return NULLSA;
		sa -> sa_selectlen = is -> is_selectlen;
	}

	return sa;
}
