/* rosapintr.c - ROPM: invoke (interruptable) */

#include <stdio.h>
#include <signal.h>
#include "rosap.h"
#include "compat.h"

static int interrupted;
static SFD	intrser ();

/*    RO-INVOKE.REQUEST (interruptable) */

int RoIntrRequest (int sd, int op, PE args, int invokeID, int *linkedID, int priority, struct RoSAPindication *roi) {
	int	    nfds,
			result;
	fd_set  rfds;
#ifdef LINUX
	__sighandler_t istat;
#else
	SFP    istat;
#endif

	if (RoInvokeRequest (sd, op, ROS_ASYNC, args, invokeID, linkedID, priority,
						 roi) == NOTOK)
		return NOTOK;

	interrupted = 0;
	istat = signal (SIGINT, (__sighandler_t)intrser);

	for (;;) {
		nfds = 0;
		FD_ZERO (&rfds);

		/* interrupt causes EINTR */
		if (RoSelectMask (sd, &rfds, &nfds, roi) == OK)
			xselect (nfds, &rfds, NULLFD, NULLFD, NOTOK);

		if (interrupted) {
			result = rosaplose (roi, ROS_INTERRUPTED, NULLCP, NULLCP);
			break;
		}

		if ((result = RoWaitRequest (sd, OK, roi)) != NOTOK
				|| roi -> roi_preject.rop_reason != ROS_TIMER)
			break;
	}

	signal (SIGINT, istat);

	return result;
}

static SFD
intrser (int sig) {
#ifndef	BSDSIGS
	signal (SIGINT, intrser);
#endif

	interrupted++;
}
