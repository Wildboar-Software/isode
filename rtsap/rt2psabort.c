/* rt2psabort.c - RTPM: user abort */

#include <stdio.h>
#include <signal.h>
#include "RTS-types.h"
#include "rtpkt.h"
#include "tailor.h"
#include "logger.h"

static int  RtUAbortRequestAux ();

/* RT-U-ABORT.REQUEST */

int RtUAbortRequest (int sd, PE data, struct RtSAPindication *rti) {
	SBV	    smask;
	int	    result;
	struct assocblk *acb;

	missingP (rti);

	smask = sigioblock ();

	rtsapPsig (acb, sd);

	result = RtUAbortRequestAux (acb, data, rti);

	sigiomask (smask);

	return result;
}

static int
RtUAbortRequestAux (struct assocblk *acb, PE data, struct RtSAPindication *rti) {
	int	    result;
	PE	    pe,
	 p;
	struct AcSAPindication acis;
	struct AcSAPindication *aci = &acis;
	struct AcSAPabort *aca = &aci -> aci_abort;

	if (!(acb -> acb_flags & ACB_ACS))
		return rtsaplose (rti, RTS_OPERATION, NULLCP,
						  "not an association descriptor for RTS");
	/* begin RTAB APDU */
	if ((pe = pe_alloc (PE_CLASS_CONT, PE_FORM_CONS, 22)) == NULLPE
			|| set_add (pe, num2prim ((integer) ABORT_USER, PE_CLASS_CONT,
									  RTAB_REASON)) == NOTOK
			|| (data
				&& (set_add (pe, p = pe_alloc (PE_CLASS_CONT, PE_FORM_CONS,
									 RTAB_USERDATA)) == NOTOK
					|| set_add (p, data) == NOTOK))) {
		result = rtsaplose (rti, RTS_CONGEST, NULLCP, "out of memory");
		AcUAbortRequest (acb -> acb_fd, NULLPEP, 0, aci);
		goto out;
	}
	pe -> pe_context = acb -> acb_rtsid;
	/* end RTAB APDU */

	PLOGP (rtsap_log,RTS_RTSE__apdus, pe, "RTABapdu", 0);

	if ((result = AcUAbortRequest (acb -> acb_fd, &pe, 1, aci)) == NOTOK)
		acs2rtslose (acb, rti, "AcUAbortRequest", aca);
	else
		result = OK;

out:
	;
	if (pe) {
		if (data)
			pe_extract (pe, data);
		pe_free (pe);
	}

	return result;
}
