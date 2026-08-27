/* rtsaputrans.c - RTPM: set uptrans upcall */

#include <stdio.h>
#include <signal.h>
#include "rtpkt.h"

/*    set uptrans upcall */

int RtSetUpTrans (
	const int sd,
	int (*fnx)(int sd, int type, caddr_t addr, struct RtSAPindication *rti),
	struct RtSAPindication *rti
) {
	SBV	    smask;
	struct assocblk   *acb;

	missingP (rti);

	smask = sigioblock ();

	rtsapPsig (acb, sd);

	acb -> acb_uptrans = fnx;

	sigiomask (smask);

	return OK;
}
