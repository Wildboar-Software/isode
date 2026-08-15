/* sprintref.c - manage encoded session addresses */

#include <stdio.h>
#include <string.h>
#include "psap.h"
#include "ssap.h"

char *sprintref (struct SSAPref *sr) {
	char  *cp;
	static char buffer[BUFSIZ];

	cp = buffer;
	*cp++ = '<';

	if (sr -> sr_ulen) {
		if (sr -> sr_ulen > 1 && *(sr -> sr_udata + 1) + 2 == sr -> sr_ulen)
			sprintf (cp, "%*.*s", sr -> sr_ulen - 2, sr -> sr_ulen - 2,
					 sr -> sr_udata + 2);
		else
			sprintf (cp, "%*.*s", sr -> sr_ulen, sr -> sr_ulen,
					 sr -> sr_udata);
		cp += strlen (cp);
	}
	*cp++ = ',';

	if (sr -> sr_clen) {
		if (sr -> sr_clen > 1 && *(sr -> sr_cdata + 1) + 2 == sr -> sr_clen)
			sprintf (cp, "%*.*s", sr -> sr_clen - 2, sr -> sr_clen - 2,
					 sr -> sr_cdata + 2);
		else
			sprintf (cp, "%*.*s", sr -> sr_clen, sr -> sr_clen,
					 sr -> sr_cdata);
		cp += strlen (cp);
	}
	*cp++ = ',';

	if (sr -> sr_alen) {
		if (sr -> sr_alen > 1 && *(sr -> sr_adata + 1) + 2 == sr -> sr_alen)
			sprintf (cp, "%*.*s", sr -> sr_alen - 2, sr -> sr_alen - 2,
					 sr -> sr_adata + 2);
		else
			sprintf (cp, "%*.*s", sr -> sr_alen, sr -> sr_alen,
					 sr -> sr_adata);
		cp += strlen (cp);
	}
	*cp++ = ',';

	if (sr -> sr_vlen) {
		if (sr -> sr_vlen > 1 && *(sr -> sr_vdata + 1) + 2 == sr -> sr_vlen)
			sprintf (cp, "%*.*s", sr -> sr_vlen - 2, sr -> sr_vlen - 2,
					 sr -> sr_vdata + 2);
		else
			sprintf (cp, "%*.*s", sr -> sr_vlen, sr -> sr_vlen,
					 sr -> sr_vdata);
		cp += strlen (cp);
	}
	*cp++ = '>';

	*cp = 0;

	return buffer;
}
