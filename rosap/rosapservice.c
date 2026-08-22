/* rosapservice.c - ROPM: hack loader */
/*
 * Based on an TCP-based implementation by George Michaelson of University
 * College London.
 */

#include <stdio.h>
#include <signal.h>
#include "ropkt.h"

/*    bind underlying service */

int RoSetService (int sd, int (*bfunc)(struct assocblk *acb, struct RoSAPindication *roi), struct RoSAPindication *roi) {
	int	    smask;
	int     result;
	struct assocblk   *acb;

	missingP (bfunc);
	missingP (roi);

	smask = sigioblock ();

	if ((acb = findacblk (sd)) == NULL) {
		sigiomask (smask);
		return rosaplose (roi, ROS_PARAMETER, NULLCP,
						  "invalid association descriptor");
	}

	result = (*bfunc) (acb, roi);

	sigiomask (smask);

	return result;
}
