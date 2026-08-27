/* ro2ssthorn.c - ROPM: interface for THORN */

#include <stdio.h>
#include <signal.h>
#include "ropkt.h"

static PE
qb2Rpe (struct qbuf *qb, int len, int *result) {
	return qb2pe (qb, len, 2, result);
}

/*    modify underling service */

int RoSetThorn (int sd, struct RoSAPindication *roi) {
	SBV	    smask;
	int	    result;
	struct assocblk   *acb;

	missingP (roi);

	smask = sigioblock ();

	if ((acb = findacblk (sd)) == NULL) {
		sigiomask (smask);
		return rosaplose (roi, ROS_PARAMETER, NULLCP,
						  "invalid association descriptor");
	}

	if (acb -> acb_flags & ACB_ROS) {
		acb -> acb_getosdu = qb2Rpe;
		result = OK;
	} else
		result = rosaplose (roi, ROS_OPERATION, NULLCP,
							"not an association descriptor for ROS");

	sigiomask (smask);

	return result;
}
