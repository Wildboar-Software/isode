/* rosapuerror.c - ROPM: error */

/*
 * 
 *
 * Based on an TCP-based implementation by George Michaelson of University
 * College London.
 *
 *
 * 
 *
 *
 *
 */

#include <stdio.h>
#include <signal.h>
#include "ropkt.h"

static int RoErrorRequestAux ();

/* RO-ERROR.REQUEST */

int RoErrorRequest (int sd, int invokeID, int error, PE params, int priority, struct RoSAPindication *roi) {
	SBV	    smask;
	int     result;
	struct assocblk   *acb;

	missingP (roi);

	smask = sigioblock ();

	rosapPsig (acb, sd);

	result = RoErrorRequestAux (acb, invokeID, error, params, priority, roi);

	sigiomask (smask);

	return result;
}

static int RoErrorRequestAux (struct assocblk *acb, int invokeID, int error, PE params, int priority, struct RoSAPindication *roi) {
	PE	pe,
	 p;

	if ((acb -> acb_flags & ACB_INIT) && (acb -> acb_flags & ACB_ROS))
		return rosaplose (roi, ROS_OPERATION, NULLCP, "not responder");
	if (!(acb -> acb_flags & ACB_ACS)) {
		missingP (params);
	}

	if (acb -> acb_ready
			&& !(acb -> acb_flags & ACB_TURN)
			&& (*acb -> acb_ready) (acb, priority, roi) == NOTOK)
		return NOTOK;

	/* begin Error APDU */
	if ((pe = pe_alloc (PE_CLASS_CONT, PE_FORM_CONS, APDU_ERROR)) == NULLPE
			|| ((acb -> acb_flags & ACB_ACS)
				? (p = pe, 0)
				: set_add (pe, p = pe_alloc (PE_CLASS_UNIV, PE_FORM_CONS,
								   PE_CONS_SEQ)) == NOTOK)
			|| seq_add (p, int2prim (invokeID), -1) == NOTOK
			|| seq_add (p, int2prim (error), -1) == NOTOK
			|| (params && seq_add (p, params, -1) == NOTOK)) {
		if (pe) {
			if (params)
				pe_extract (pe, params);
			pe_free (pe);
		}
		freeacblk (acb);
		return rosaplose (roi, ROS_CONGEST, NULLCP, "out of memory");
	}
	/* end Error APDU */

	return (*acb -> acb_putosdu) (acb, pe, params, priority, roi);
}
