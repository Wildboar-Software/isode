/* rt2psreleas2.c - RTPM: respond to release */

#include <stdio.h>
#include <signal.h>
#include "rtpkt.h"

static int  RtCloseResponseAux (struct assocblk *acb, int reason, PE data, struct RtSAPindication *rti);

/* RT-CLOSE.RESPONSE */

int RtCloseResponse (int sd, int reason, PE data, struct RtSAPindication *rti) {
	SBV	    smask;
	int	    result;
	struct assocblk *acb;

	missingP (rti);

	smask = sigioblock ();

	rtsapFsig (acb, sd);

	result = RtCloseResponseAux (acb, reason, data, rti);

	sigiomask (smask);

	return result;
}

static int RtCloseResponseAux (struct assocblk *acb, int reason, PE data, struct RtSAPindication *rti) {
	int	    result;
	struct AcSAPindication acis;
	struct AcSAPindication *aci = &acis;
	struct AcSAPabort *aca = &aci -> aci_abort;

	if (!(acb -> acb_flags & ACB_ACS))
		return rtsaplose (rti, RTS_OPERATION, NULLCP,
						  "not an association descriptor for RTS");

	if (data)
		data -> pe_context = acb -> acb_rtsid;

	acb -> acb_flags &= ~ACB_STICKY;
	if (AcRelResponse (acb -> acb_fd, ACS_ACCEPT, reason, &data, data ? 1 : 0,
					   aci) == NOTOK)
		result = acs2rtslose (acb, rti, "AcRelResponse", aca);
	else
		result = OK;

	return result;
}
