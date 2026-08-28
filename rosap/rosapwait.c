/* rosapwait.c - ROPM: wait for an indication */
/*
 * Based on an TCP-based implementation by George Michaelson of University
 * College London.
 */

#include <stdio.h>
#include <signal.h>
#include "ropkt.h"

/*    RO-WAIT.REQUEST (pseudo) */

int RoWaitRequest (int sd, const int secs, struct RoSAPindication *roi) {
	SBV	    smask;
	int     result;
	struct assocblk   *acb;

	missingP (roi);

	smask = sigioblock ();

	rosapXsig (acb, sd);

	result =  (*acb -> acb_rowaitrequest) (acb, NULL, secs, roi);

	sigiomask (smask);

	return result;
}
