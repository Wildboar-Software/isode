/* nsapcmp.c - compare NSAPs */

#include <string.h>
#include <strings.h>
#include "general.h"
#include "manifest.h"
#include "isoaddrs.h"
static int nsap_addr_cmp (struct NSAPaddr *na1, struct NSAPaddr *na2);
static int ssap_addr_cmp (struct SSAPaddr *sa, struct SSAPaddr *sb);
static int psap_addr_cmp (struct PSAPaddr *pa, struct PSAPaddr *pb);


static int nsap_addr_cmp (struct NSAPaddr *na1, struct NSAPaddr *na2) {
	if (na1 -> na_stack != na2 -> na_stack)
		return NOTOK;

	switch (na1 -> na_stack) {
	case NA_NSAP:
		if (na1 -> na_addrlen == 0 ||
				(na1 -> na_addrlen == na2 -> na_addrlen &&
				 bcmp_int (na1 -> na_address, na2 -> na_address,
					   (int) na1 -> na_addrlen) == 0))
			return OK;
		break;

	case NA_TCP:
		if (na1 -> na_port == na2 -> na_port &&
				strcmp (na1 -> na_domain, na2 -> na_domain) == 0)
			return OK;
		break;

	case NA_X25:
		if (na1 -> na_dtelen == na2 -> na_dtelen &&
				bcmp_int (na1 -> na_dte, na2 -> na_dte,
					  (int) na1 -> na_dtelen) == 0 &&
				na1 -> na_pidlen == na2 -> na_pidlen &&
				bcmp_int (na1 -> na_pid, na2 -> na_pid,
					  (int) na1 -> na_pidlen) == 0)
			return OK;
		break;
	}
	return NOTOK;
}

int tsap_addr_cmp (struct TSAPaddr *ta, struct TSAPaddr *tb) {
	struct NSAPaddr *na1, *na2;
	int n1, n2;

	if (ta -> ta_selectlen != tb -> ta_selectlen ||
			bcmp_int (ta -> ta_selector, tb -> ta_selector, ta -> ta_selectlen) != 0)
		return NOTOK;
	if (ta -> ta_naddr == 0 && tb -> ta_naddr == 0)
		return OK;
	for (na1 = ta -> ta_addrs, n1 = ta -> ta_naddr; n1-- > 0; na1++) {
		for (na2 = tb -> ta_addrs, n2 = tb -> ta_naddr; n2 -- > 0; na2 ++)
			if (nsap_addr_cmp (na1, na2) == OK)
				return OK;
	}
	return NOTOK;
}

static int ssap_addr_cmp (struct SSAPaddr *sa, struct SSAPaddr *sb) {
	if (sa -> sa_selectlen != sb -> sa_selectlen ||
			bcmp_int (sa -> sa_selector, sb -> sa_selector, sa -> sa_selectlen) != 0)
		return NOTOK;
	return tsap_addr_cmp (&sa -> sa_addr, &sb -> sa_addr);
}

static int psap_addr_cmp (struct PSAPaddr *pa, struct PSAPaddr *pb) {
	if (pa -> pa_selectlen != pb -> pa_selectlen ||
			bcmp_int (pa -> pa_selector, pb -> pa_selector, pa -> pa_selectlen) != 0)
		return NOTOK;
	return ssap_addr_cmp (&pa -> pa_addr, &pb -> pa_addr);
}
