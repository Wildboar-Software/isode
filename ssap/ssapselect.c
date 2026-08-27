/* ssapselect.c - SPM: map descriptors */

#include <stdio.h>
#include <signal.h>
#include "spkt.h"

/*    map session descriptors for select() */

int SSelectMask (const int sd, const fd_set *mask, const int *nfds, struct SSAPindication *si) {
	SBV	    smask;
	int     result;
	struct ssapblk *sb;
	struct TSAPdisconnect   tds;
	struct TSAPdisconnect *td = &tds;

	missingP (mask);
	missingP (nfds);

	smask = sigioblock ();

	if ((sb = findsblk (sd)) == NULL) {
		sigiomask (smask);
		return ssaplose (si, SC_PARAMETER, NULLCP,
						 "invalid session descriptor");
	}

	if ((result = TSelectMask (sb -> sb_fd, mask, nfds, td)) == NOTOK)
		if (td -> td_reason == DR_WAITING)
			ssaplose (si, SC_WAITING, NULLCP, NULLCP);
		else
			ts2sslose (si, "TSelectMask", td);

	sigiomask (smask);

	return result;
}
