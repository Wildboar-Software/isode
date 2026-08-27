/* ro2ssreleas2.c - respond to release */
/*
 * Based on an TCP-based implementation by George Michaelson of University
 * College London.
 */

#include <stdio.h>
#include <signal.h>
#include "ropkt.h"

static int  RoEndResponseAux (struct assocblk *acb, const struct RoSAPindication *roi);

/* RO-END.RESPONSE */

int RoEndResponse (const int sd, struct RoSAPindication *roi) {
	SBV	    smask;
	int     result;
	struct assocblk   *acb;

	missingP (roi);

	smask = sigioblock ();

	rosapFsig (acb, sd);

	result = RoEndResponseAux (acb, roi);

	sigiomask (smask);

	return result;

}

static int RoEndResponseAux (struct assocblk *acb, const struct RoSAPindication *roi) {
	int     result;
	struct SSAPindication   sis;
	struct SSAPindication *si = &sis;
	struct SSAPabort *sa = &si -> si_abort;

	if (SRelResponse (acb -> acb_fd, SC_ACCEPT, NULLCP, 0, si) == NOTOK)
		result = ss2roslose (acb, roi, "SRelResponse", sa);
	else {
		acb -> acb_fd = NOTOK;
		result = OK;
	}

	freeacblk (acb);

	return result;
}
