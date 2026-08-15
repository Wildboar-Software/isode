/* rtsapdtrans.c - RTPM: set downtrans upcall */

#include <stdio.h>
#include <signal.h>
#include "rtpkt.h"

/*    set downtrans upcall */

int RtSetDownTrans (
	int sd,
	int (*fnx)(int sd, char **base, int *len, int size, long ssn, long ack, struct RtSAPindication *rti),
	struct RtSAPindication *rti
) {
	int	    smask;
	struct assocblk   *acb;

	missingP (rti);

	smask = sigioblock ();

	rtsapPsig (acb, sd);

	acb -> acb_downtrans = fnx;

	sigiomask (smask);

	return OK;
}
