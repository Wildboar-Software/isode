/* rt2ssreleas2.c - RTPM: respond to release */







#include <stdio.h>
#include <signal.h>
#include "rtpkt.h"

static int  RtEndResponseAux ();

/*    RT-END.RESPONSE (X.410 CLOSE.RESPONSE) */

int RtEndResponse (int sd, struct RtSAPindication *rti) {
	SBV	    smask;
	int     result;
	struct assocblk   *acb;

	missingP (rti);

	smask = sigioblock ();

	rtsapFsig (acb, sd);

	result = RtEndResponseAux (acb, rti);

	sigiomask (smask);

	return result;

}

static int
RtEndResponseAux (struct assocblk *acb, struct RtSAPindication *rti) {
	int     result;
	struct SSAPindication   sis;
	struct SSAPindication *si = &sis;
	struct SSAPabort  *sa = &si -> si_abort;

	if (acb -> acb_flags & ACB_ACS)
		return rtsaplose (rti, RTS_OPERATION, NULLCP,
						  "not an association descriptor for RTS");

	if (SRelResponse (acb -> acb_fd, SC_ACCEPT, NULLCP, 0, si) == NOTOK)
		result = ss2rtslose (acb, rti, "SRelResponse", sa);
	else {
		acb -> acb_fd = NOTOK;
		result = OK;
	}

	acb -> acb_flags &= ~ACB_STICKY;
	freeacblk (acb);

	return result;
}
