/* dse-c.c - DSE wrapper for pepsy */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "psap.h"
#include "isoaddrs.h"
#include "tailor.h"
#include "DSE-types.h"
#include "pepsycodec.h"
#include "acsap.h"

#define advise	PY_advise

static struct type_DSE_PSAPaddr *psap2dse (const struct PSAPaddr *pa);

static int  dse2psap (const struct type_DSE_PSAPaddr *dse, struct PSAPaddr *pa);
static int  gstring (char *buf, const int buflen, struct qbuf *qb, const char *w);

int build_DSE_PSAPaddr (PE *pe, int explicit, int len, const char *buffer, const char *parm) {
	int	    result;
	struct PSAPaddr *pa = (struct PSAPaddr *) parm;
	struct type_DSE_PSAPaddr *dse;

	if ((dse = psap2dse (pa)) == NULL)
		return NOTOK;
	result = encode_DSE_PSAPaddr (pe, explicit, len, buffer, dse);
	free_DSE_PSAPaddr (dse);
	return result;
}

static struct type_DSE_PSAPaddr *
psap2dse (const struct PSAPaddr *pa) {
	int    n;
	struct type_DSE_PSAPaddr *dse;
	struct SSAPaddr *sa = &pa -> pa_addr;
	struct TSAPaddr *ta = &sa -> sa_addr;
	struct NSAPaddr *na;
	struct member_DSE_0  *nDSE,
			   **oDSE;

	if ((dse = (struct type_DSE_PSAPaddr *) calloc (1, sizeof *dse)) == NULL) {
		advise (NULLCP, "psap2dse: out of memory");
		return NULL;
	}

	if (pa -> pa_selectlen > 0)
		dse -> pSelector = str2qb (pa -> pa_selector, pa -> pa_selectlen, 1);

	if (sa -> sa_selectlen > 0)
		dse -> sSelector = str2qb (sa -> sa_selector, sa -> sa_selectlen, 1);

	if (ta -> ta_selectlen > 0)
		dse -> tSelector = str2qb (ta -> ta_selector, ta -> ta_selectlen, 1);

	oDSE = &dse -> nAddress;
	for (na = ta -> ta_addrs, n = ta -> ta_naddr; n > 0; na++, n--) {
		struct NSAPaddr *ca;

		if ((nDSE = (struct member_DSE_0 *) calloc (1, sizeof *nDSE))
				== NULL) {
			advise (NULLCP, "psap2dse: out of memory");
losing:
			;
			free_DSE_PSAPaddr (dse);
			return NULL;
		}
		*oDSE = nDSE;
		oDSE = &nDSE -> next;

		if ((ca = na2norm (na)) == NULLNA) {
			advise (NULLCP, "unable to normalize address");
			goto losing;
		}

		nDSE -> member_DSE_1 = str2qb (ca -> na_address, ca -> na_addrlen, 1);
	}

	return dse;
}

int parse_DSE_PSAPaddr (PE pe, int explicit, const int *len, char **buffer, const char *parm) {
	int	    result;
	struct PSAPaddr *pa = (struct PSAPaddr *) parm;
	struct type_DSE_PSAPaddr *dse;

	if (decode_DSE_PSAPaddr (pe, explicit, len, buffer, &dse) == NOTOK)
		return NOTOK;
	result = dse2psap (dse, pa);
	free_DSE_PSAPaddr (dse);
	return result;
}

static int dse2psap (const struct type_DSE_PSAPaddr *dse, struct PSAPaddr *pa) {
	struct SSAPaddr *sa = &pa -> pa_addr;
	struct TSAPaddr *ta = &sa -> sa_addr;
	struct member_DSE_0 *nDSE;

	bzero ((char *) pa, sizeof *pa);

	pa -> pa_selectlen = gstring (pa -> pa_selector,
								  sizeof pa -> pa_selector,
								  dse -> pSelector, "psap selector");

	sa -> sa_selectlen = gstring (sa -> sa_selector, sizeof sa -> sa_selector,
								  dse -> sSelector, "ssap selector");

	ta -> ta_selectlen = gstring (ta -> ta_selector, sizeof ta -> ta_selector,
								  dse -> tSelector, "tsap selector");

	for (nDSE  = dse -> nAddress; nDSE; nDSE = nDSE -> next) {
		char   *p;

		if (ta -> ta_naddr >= NTADDR) {
			advise (NULLCP, "too many network addresses");
			return NOTOK;
		}

		p = qb2str (nDSE->member_DSE_1);
		if (norm2na (p, nDSE -> member_DSE_1 -> qb_len,
					 &ta -> ta_addrs[ta -> ta_naddr++]) == NOTOK)
			return NOTOK;
		free (p);
	}

	return OK;
}

static int gstring (char *buf, const int buflen, struct qbuf *qb, const char *w) {
	char   *p;

	if (qb == NULL || qb -> qb_len <= 0)
		return 0;

	if (qb -> qb_len > buflen) {
		advise (NULLCP, "%s too long", w);
		return 0;
	}

	p = qb2str (qb);
	if (bcopy_int (p, buf, qb -> qb_len) != 0) {
		free (p);
		return 0;
	}
	free (p);

	return qb -> qb_len;
}
