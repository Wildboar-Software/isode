/* ryoperation.c - ROSY: operations */
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

int	RyOperation (
	int sd,
	struct RyOperation *ryo,
	int op,
	caddr_t in,
	caddr_t *out,
	int *response,
	struct RoSAPindication *roi
) {
	int     result;

#ifdef	notdef			/* let RyOpInvoke check these as necessary */
	missingP (ryo);
	missingP (in);
#endif
	missingP (out);
	missingP (response);
	missingP (roi);
	switch (result = RyOpInvoke (sd, ryo, op, in, out, NULL, NULL,
								 ROS_SYNC, RyGenID (sd),
								 NULL, ROS_NOPRIO, roi)) {
	case NOTOK:
		return NOTOK;
	case OK:
		switch (roi -> roi_type) {
		case ROI_RESULT:
			*response = RY_RESULT;
			return OK;
		case ROI_ERROR: {	/* XXX: hope roe -> roe_error != NOTOK */
			struct RoSAPerror  *roe = &roi -> roi_error;
			*response = roe -> roe_error;
			return OK;
		}
		case ROI_UREJECT: {
			struct RoSAPureject *rou = &roi -> roi_ureject;
			return rosaplose (roi, rou -> rou_reason, NULLCP,
							  NULLCP);
		}
		default:
			return rosaplose (roi, ROS_PROTOCOL, NULLCP,
							  "unknown indication type=%d", roi -> roi_type);
		}
	case DONE:
		return DONE;
	default:
		return rosaplose (roi, ROS_PROTOCOL, NULLCP,
						  "unknown return from RyInvoke=%d", result);
	}
}
