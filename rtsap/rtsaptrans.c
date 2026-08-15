/* rtsaptrans.c - RTPM: transfer */

#include <stdio.h>
#include <signal.h>
#include "rtpkt.h"

/* RT-TRANSFER.REQUEST */

int RtTransferRequest (int sd, PE data, int secs, struct RtSAPindication *rti) {
	int	    smask;
	int     result;
	struct assocblk   *acb;

	missingP (rti);

	smask = sigioblock ();

	rtsapPsig (acb, sd);

	if (data == NULLPE && acb -> acb_downtrans == NULLIFP) {
		sigiomask (smask);
		return rtsaplose (rti, RTS_PARAMETER, NULLCP,
						  "mandatory parameter \"data\" missing");
	}

	result = (*acb -> acb_transferequest)  (acb, data, secs, rti);

	sigiomask (smask);

	return result;
}
