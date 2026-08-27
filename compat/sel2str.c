/* sel2str.c - selector to string */

#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include "general.h"
#include "manifest.h"
#include "isoaddrs.h"

char *sel2str (char *sel, int len, const int quoted) {
	char *cp,
		 *dp,
		 *ep;
	static int    i = 0;
	static char buf1[NASIZE * 2 + 1],
		   buf2[NASIZE * 2 + 1],
		   buf3[NASIZE * 2 + 1],
		   buf4[NASIZE * 2 + 1];
	static char *bufs[] = { buf1, buf2, buf3, buf4 };

	cp = bufs[i++];
	i = i % 4;

	if (quoted) {
#ifndef	NOGOSIP
		if (len == 2) {
			if (quoted < 0)
				goto ugly;
			sprintf (cp, "#%d",
					 (sel[0] & 0xff) << 8 | (sel[1] & 0xff));
			goto out;
		}
#endif

		for (ep = (dp = sel) + len; dp < ep; dp++)
			if (!isprint ((uint8_t) *dp))
				goto ugly;

		if (len > NASIZE * 2)
			len = NASIZE * 2;

		sprintf (cp, len ? "\"%*.*s\"" : "\"\"", len, len, sel);
	} else {
ugly:
		;
		if (len > NASIZE)	/* XXX */
			len = NASIZE;

		cp[explode (cp, (uint8_t *) sel, len)] = 0;
	}
#ifndef	NOGOSIP
out:
	;
#endif

	return cp;
}
