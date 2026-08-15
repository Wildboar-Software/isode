/* addr2ref.c - manage encoded session addresses */

#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdint.h>
#include "psap.h"
#include "ssap.h"

static int  stuff ();

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

	if ((pe = t61s2prim (addr, strlen (addr))) == NULLPE)
		return NULL;
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

	bcopy (base, dbase, (int) (*dlen = len));
	free (base);

	return OK;
}
