/* is2saddr.c - old-style S-ADDR lookup */







#include <string.h>
#include "psap.h"
#include "isoaddrs.h"
#include "isoservent.h"

struct SSAPaddr *
is2saddr (char *host, char *service, struct isoservent *is) {
	AEI	    aei;
	struct PSAPaddr *pa;
	struct SSAPaddr *sa;

	if ((aei = str2aei (host, service)) == NULLAEI || (pa = aei2addr (aei)) == NULLPA)
		return NULLSA;

	sa = &pa -> pa_addr;
	if (is && strcmp (is -> is_provider, "ssap") == 0) {
		if (is -> is_selectlen > SSSIZE)	/* XXX */
			return NULLSA;

		bcopy (is -> is_selector, sa -> sa_selector,  sa -> sa_selectlen = is -> is_selectlen);
	}

	return sa;
}
