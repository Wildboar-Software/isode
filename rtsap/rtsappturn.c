/* rtsappturn.c - RTPM: turn please */

#include <stdio.h>
#include <signal.h>
#include "rtpkt.h"

/* RT-TURN-PLEASE.REQUEST */

int RtPTurnRequest (int sd, int priority, struct RtSAPindication *rti) {
	SBV	    smask;
	int     result;
	struct assocblk   *acb;

	missingP (rti);

	smask = sigioblock ();

	rtsapPsig (acb, sd);

	result = (*acb -> acb_pturnrequest) (acb, priority, rti);

	sigiomask (smask);

	return result;
}
