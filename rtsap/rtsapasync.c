/* rtsapasync.c - RTPM: set asynchronous events */







#include <stdio.h>
#include <signal.h>
#include "rtpkt.h"

/*    define vectors for INDICATION events */

int RtSetIndications (int sd, IFP indication, struct RtSAPindication *rti) {
	SBV	    smask;
	int     result;
	struct assocblk   *acb;

	_iosignals_set = 1;

	smask = sigioblock ();

	rtsapPsig (acb, sd);

	if (acb -> acb_flags & ACB_PLEASE) {
		sigiomask (smask);

		return rtsaplose (rti, RTS_WAITING, NULLCP, NULLCP);
	}

	result = (*acb -> acb_rtsetindications) (acb, indication, rti);

	sigiomask (smask);

	return result;
}
