/* rosapselect.c - ROPM: map descriptors */



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

/*    map association descriptors for select() */

int RoSelectMask (int sd, fd_set *mask, int *nfds, struct RoSAPindication *roi) {
	SBV	    smask;
	int     result;
	struct assocblk   *acb;

	missingP (mask);
	missingP (nfds);
	missingP (roi);

	smask = sigioblock ();

	rosapPsig (acb, sd);

	if (acb -> acb_apdu || (acb -> acb_flags & ACB_CLOSING)) {
		sigiomask (smask);
		return rosaplose (roi, ROS_WAITING, NULLCP, NULLCP);
	}

	result = (*acb -> acb_roselectmask) (acb, mask, nfds, roi);

	sigiomask (smask);

	return result;
}
