/* pa2str.c - pretty-print PSAPaddr */

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "general.h"
#include "manifest.h"
#include "isoaddrs.h"

/*    Presentation Address to String */

char *pa2str (struct PSAPaddr *px) {
	char *bp;
	struct PSAPaddr pas;
	struct PSAPaddr *pa = &pas;
	struct TSAPaddr *ta = &pa -> pa_addr.sa_addr;
	static char buffer[BUFSIZ];

	bp = buffer;

	*pa = *px;	/* struct copy */
	if (ta -> ta_selectlen > 0
			&& ta -> ta_selectlen < sizeof ta -> ta_selector) {
		char *dp,
			 *ep;
		struct TSAPaddr *tz;
		int n, m;

		/* does this look like an encoded TSEL? */
		m = ta -> ta_selectlen;
		n = ta -> ta_selector[0];
		if (m > 4 &&
				ta -> ta_selector[0] == ta -> ta_selector[1] &&
				n > 2 && n <= m - 2) {
			/* encoded! */
			tz = &px -> pa_addr.sa_addr;
			bzero ((char *)ta, sizeof *ta);
			if ((ta -> ta_selectlen = m - n - 2) > 0)
				bcopy (&tz -> ta_selector[n+2], ta -> ta_selector,
					   ta -> ta_selectlen);
			if (norm2na (&tz -> ta_selector[2], n, ta -> ta_addrs) != OK) {
				*pa = *px;
				goto normal;
			}
			ta -> ta_naddr = 1;
			goto bridge;
		}
		for (ep = (dp = ta -> ta_selector) + ta -> ta_selectlen, *ep = 0;
				dp < ep;
				dp++)
			if (!isprint ((uint8_t) *dp) && *dp != ' ')
				break;
		if (dp >= ep && (tz = str2taddr (ta -> ta_selector))) {
			pa -> pa_addr.sa_addr = *tz;	    /* struct copy */
bridge:
			sprintf (bp, "%s through TS bridge at ",
					 paddr2str (pa, NULLNA));
			bp += strlen (bp);

			bzero ((char *) pa, sizeof *pa);
			*ta = px -> pa_addr.sa_addr;    /* struct copy */
			ta -> ta_selectlen = 0;
		}
	}
normal:
	strcpy (bp, paddr2str (pa, NULLNA));

	return buffer;
}
