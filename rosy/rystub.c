/* rystub.c - ROSY: stubs */

#include <signal.h>
#include <sys/types.h>
#include "compat.h"
#include "rosy.h"

#ifdef __STDC__
#define missingP(p) \
{ \
    if (p == NULL) \
        return rosaplose (roi, ROS_PARAMETER, NULLCP, \
                            "mandatory parameter \"%s\" missing", #p); \
}
#else
#define	missingP(p) \
{ \
    if (p == NULL) \
	return rosaplose (roi, ROS_PARAMETER, NULLCP, \
			    "mandatory parameter \"%s\" missing", "p"); \
}
#endif

static int interrupted;
#if defined(SVR4) || defined(LINUX)
static void	intrser (int sig);
#else
static SFD	intrser (int sig);
#endif

/* stub */

int	RyStub (
	int sd,
	struct RyOperation *ryo,
	int op,
	int id,
	int *linked,
	caddr_t in,
	int (*rfx)(int sd, int id, int dummy, caddr_t result, struct RoSAPindication *roi),
	void (*efx)(int sd, int id, int error, caddr_t parameter, struct RoSAPindication *roi),
	int class,
	struct RoSAPindication *roi
) {
	int     firstime,
			opclass,
			result;
#ifdef LINUX
	__sighandler_t istat;
#else
	SFP	    istat;
#endif

#ifdef	notdef			/* let RyOpInvoke check these as necessary */
	missingP (ryo);
	missingP (in);
	missingP (rfx);
	missingP (efx);
#endif
	missingP (roi);
	if ((opclass = class) == ROS_INTR) {
		interrupted = 0;
		istat = signal (SIGINT, (__sighandler_t)intrser);
		opclass = ROS_ASYNC;
	}
	result = RyOpInvoke (sd, ryo, op, in, (caddr_t *) NULL, rfx, efx,
						 opclass, id, linked, ROS_NOPRIO, roi);
	firstime = 1;
again:
	;
	switch (result) {
	case NOTOK:
		break;

	case OK:
		switch (class) {
		case ROS_ASYNC:
			break;

		case ROS_INTR:
			if (firstime) {
				for (;;) {
					if (!interrupted) {
						int	nfds;
						fd_set	rfds;

						nfds = 0;
						FD_ZERO (&rfds);

						/* interrupt causes EINTR */
						if (RoSelectMask (sd, &rfds, &nfds, roi) == OK)
							xselect (nfds, &rfds, NULLFD,
									 NULLFD, NOTOK);
					}
					if (interrupted) {
						result = rosaplose (roi, ROS_INTERRUPTED,
											NULLCP, NULLCP);
						break;
					}
					if ((result = RyWait (sd, &id, (caddr_t *) NULL,
										  OK, roi)) != NOTOK
							|| roi -> roi_preject.rop_reason
							!= ROS_TIMER) {
						firstime = 0;
						goto again;
					}
				}
				break;
			}
		/* else fall */

		default:
			switch (roi -> roi_type) {
			case ROI_RESULT:
			case ROI_ERROR:
			case ROI_UREJECT:
				result = OK;
				break;

			default:
				result = rosaplose (roi, ROS_PROTOCOL, NULLCP,
									"unknown indication type=%d",
									roi -> roi_type);
				break;
			}
			break;
		}

	case DONE:
		break;

	default:
		result = rosaplose (roi, ROS_PROTOCOL, NULLCP,
							"unknown return from RyInvoke=%d", result);
		break;
	}

	if (class == ROS_INTR)
		signal (SIGINT, (__sighandler_t)istat);
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
