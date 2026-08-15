/* rtsapselect.c - RTPM: map descriptors */

#include <stdio.h>
#include <signal.h>
#include "rtpkt.h"

/*    map association descriptors for select() */

int RtSelectMask (int sd, fd_set *mask, int *nfds, struct RtSAPindication *rti) {
	int	    smask;
	int     result;
	struct assocblk   *acb;

	missingP (mask);
	missingP (nfds);
	missingP (rti);

	smask = sigioblock ();

	rtsapPsig (acb, sd);

	if (acb -> acb_flags & ACB_PLEASE) {
		sigiomask (smask);

		return rtsaplose (rti, RTS_WAITING, NULLCP, NULLCP);
	}

	result = (*acb -> acb_rtselectmask) (acb, mask, nfds, rti);

	sigiomask (smask);

	return result;
}
