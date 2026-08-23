/* is2taddr.c - old-style T-ADDR lookup */

#include <string.h>
#include <strings.h>
#include "psap.h"
#include "isoaddrs.h"
#include "isoservent.h"

struct TSAPaddr *
is2taddr (char *host, char *service, struct isoservent *is) {
	AEI	    aei;
	struct PSAPaddr *pa;
	struct TSAPaddr *ta;

	if ((aei = str2aei (host, service)) == NULLAEI || (pa = aei2addr (aei)) == NULLPA)
		return NULLTA;

	ta = &pa -> pa_addr.sa_addr;
	if (is && strcmp (is -> is_provider, "tsap") == 0) {
		if (is -> is_selectlen > TSSIZE)	/* XXX */
			return NULLTA;

		if (bcopy_int (is -> is_selector, ta -> ta_selector,
					   is -> is_selectlen) != 0)
			return NULLTA;
		ta -> ta_selectlen = is -> is_selectlen;
	}

	return ta;
}
