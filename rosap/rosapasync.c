/* rosapasync.c - ROPM: set asynchronous events */
/*
 * Based on an TCP-based implementation by George Michaelson of University
 * College London.
 */

#include <stdio.h>
#include <signal.h>
#include "ropkt.h"

/*    define vectors for INDICATION events */

int RoSetIndications (int sd, IFP indication, struct RoSAPindication *roi) {
	int	    smask;
	int     result;
	struct assocblk   *acb;

	_iosignals_set = 1;

	smask = sigioblock ();

	rosapPsig (acb, sd);

	if (acb -> acb_apdu || (acb -> acb_flags & ACB_CLOSING)) {
		sigiomask (smask);
		return rosaplose (roi, ROS_WAITING, NULLCP, NULLCP);
	}

	result = (*acb -> acb_rosetindications) (acb, indication, roi);

	sigiomask (smask);

	return result;
}
