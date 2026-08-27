/* rosapintr.c - ROPM: invoke (interruptable) */

#include <stdio.h>
#include <signal.h>
#include "rosap.h"
#include "compat.h"

static int interrupted;
#if defined(SVR4) || defined(LINUX)
static void	intrser (int sig);
#else
static SFD	intrser (int sig);
#endif

/*    RO-INVOKE.REQUEST (interruptable) */

int RoIntrRequest (const int sd, const int op, PE args, const int invokeID, const int *linkedID, const int priority, struct RoSAPindication *roi) {
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

#if defined(SVR4) || defined(LINUX)
static void
#else
static SFD
#endif
intrser (int sig) {
#ifndef	BSDSIGS
	signal (SIGINT, intrser);
#endif

	interrupted++;
}
