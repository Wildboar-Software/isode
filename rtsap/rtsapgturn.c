/* rtsapgturn.c - RTPM: give turn */

#include <stdio.h>
#include <signal.h>
#include "rtpkt.h"

/* RT-TURN-GIVE.REQUEST */

int RtGTurnRequest (int sd, struct RtSAPindication *rti) {
	SBV	    smask;
	int     result;
	struct assocblk   *acb;

	missingP (rti);

	smask = sigioblock ();

	rtsapPsig (acb, sd);

	result = (*acb -> acb_gturnrequest) (acb, rti);

	sigiomask (smask);

	return result;
}
