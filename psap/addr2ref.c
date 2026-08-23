/* addr2ref.c - manage encoded session addresses */

#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdint.h>
#include "psap.h"
#include "ssap.h"
#include "psap2.h"

static int stuff (PE pe, char *dbase, uint8_t *dlen);

struct SSAPref *
addr2ref (char *addr) {
	int     result;
	long    clock;
	PE pe;
	struct tm *tm = NULL;
	struct UTCtime  uts;
	struct UTCtime *ut = &uts;
	static struct SSAPref   srs;
	struct SSAPref *sr = &srs;

	bzero ((char *) sr, sizeof *sr);

	{
		int alen;

		if (sizet2int (strlen (addr), &alen) != 0)
			return NULL;
		if ((pe = t61s2prim (addr, alen)) == NULLPE)
			return NULL;
	}
	result = stuff (pe, sr -> sr_udata, &sr -> sr_ulen);
	pe_free (pe);
	if (result == NOTOK)
		return NULL;

	if (time (&clock) == NOTOK || (tm = gmtime (&clock)) == NULL)
		return NULL;
	tm2ut (tm, ut);

	if ((pe = utct2prim (ut)) == NULLPE)
		return NULL;
	result = stuff (pe, sr -> sr_cdata, &sr -> sr_clen);
	pe_free (pe);
	if (result == NOTOK)
		return NULL;

	return sr;
}

static int stuff (PE pe, char *dbase, uint8_t *dlen) {
	int     len;
	char   *base;

	if (pe2ssdu (pe, &base, &len) == NOTOK)
		return NOTOK;

	if (int2u8 (len, dlen) != 0
			|| bcopy_int (base, dbase, len) != 0) {
		free (base);
		return NOTOK;
	}
	free (base);

	return OK;
}
