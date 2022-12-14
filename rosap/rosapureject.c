/* rosapureject.c - ROPM: user reject */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/rosap/RCS/rosapureject.c,v 9.0 1992/06/16 12:37:02 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/rosap/RCS/rosapureject.c,v 9.0 1992/06/16 12:37:02 isode Rel $
 *
 * Based on an TCP-based implementation by George Michaelson of University
 * College London.
 *
 *
 * $Log: rosapureject.c,v $
 * Revision 9.0  1992/06/16  12:37:02  isode
 * Release 8.0
 *
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */


/* LINTLIBRARY */

#include <stdio.h>
#include <signal.h>
#include "ropkt.h"

/*    RO-U-REJECT.REQUEST */

int
RoURejectRequestAux (struct assocblk *acb, int *invokeID, int reason, PElementID id, int priority, struct RoSAPindication *roi);

int
RoURejectRequest (int sd, int *invokeID, int reason, int priority, struct RoSAPindication *roi) {
	SBV	    smask;
	int     result;
	PElementID id;
	struct assocblk   *acb;

	switch (reason) {
	case ROS_IP_DUP: 	/* Invoke Problem */
	case ROS_IP_UNRECOG:
	case ROS_IP_MISTYPED:
	case ROS_IP_LIMIT:
		id = REJECT_INVOKE;
		reason -= REJECT_INVOKE_BASE;
		break;

	case ROS_IP_RELEASE:
	case ROS_IP_UNLINKED:
	case ROS_IP_LINKED:
	case ROS_IP_CHILD:
		id = REJECT_COMPLETE;
		reason -= REJECT_INVOKE_BASE;
		break;

	case ROS_RRP_UNRECOG: 	/* Return Result Problem */
	case ROS_RRP_UNEXP:
	case ROS_RRP_MISTYPED:
		id = REJECT_RESULT;
		reason -= REJECT_RESULT_BASE;
		break;

	case ROS_REP_UNRECOG: 	/* Return Error Problem */
	case ROS_REP_UNEXP:
	case ROS_REP_RECERR:
	case ROS_REP_UNEXPERR:
	case ROS_REP_MISTYPED:
		id = REJECT_ERROR;
		reason -= REJECT_ERROR_BASE;
		break;

	default:
		return rosaplose (roi, ROS_PARAMETER, NULLCP,
						  "bad value for reason parameter");
	}
	missingP (roi);

	smask = sigioblock ();

	rosapPsig (acb, sd);

	result = RoURejectRequestAux (acb, invokeID, reason, id, priority, roi);

	sigiomask (smask);

	return result;
}

/*  */

int
RoURejectRequestAux (struct assocblk *acb, int *invokeID, int reason, PElementID id, int priority, struct RoSAPindication *roi) {
	PE pe,
	p;

	if (id == REJECT_COMPLETE)
		if (acb -> acb_flags & ACB_ACS)
			id = REJECT_INVOKE;
		else
			return rosaplose (roi, ROS_PARAMETER, NULLCP,
							  "bad value for reason parameter");

	if (acb -> acb_ready
			&& !(acb -> acb_flags & ACB_TURN)
			&& (*acb -> acb_ready) (acb, priority, roi) == NOTOK)
		return NOTOK;

	/* begin Reject APDU */
	if ((pe = pe_alloc (PE_CLASS_CONT, PE_FORM_CONS, APDU_REJECT)) == NULLPE
			|| ((acb -> acb_flags & ACB_ACS)
				? (p = pe, 0)
				: set_add (pe, p = pe_alloc (PE_CLASS_UNIV, PE_FORM_CONS,
								   PE_CONS_SEQ)) == NOTOK)
			|| seq_add (p, invokeID ? int2prim (*invokeID)
						: pe_alloc (PE_CLASS_UNIV, PE_FORM_PRIM,
									PE_PRIM_NULL), -1) == NOTOK
			|| seq_add (p, num2prim ((integer) reason, PE_CLASS_CONT, id), -1)
			== NOTOK) {
		if (pe)
			pe_free (pe);
		freeacblk (acb);
		return rosaplose (roi, ROS_CONGEST, NULLCP, "out of memory");
	}
	/* end Reject APDU */

	return (*acb -> acb_putosdu) (acb, pe, NULLPE, priority, roi);
}
