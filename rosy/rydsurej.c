/* rydsureject.c - ROSY: reject invocation */







#include <stdio.h>
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

/* U-REJECT */

int	RyDsUReject (sd, id, reason, priority, roi)
int	sd;
int	id,
	reason,
	priority;
struct RoSAPindication *roi;
{
	int     result;
	struct opsblk *opb;

	missingP (roi);

	if ((opb = findopblk (sd, id, OPB_RESPONDER)) == NULLOPB)
		return rosaplose (roi, ROS_PARAMETER, NULLCP,
						  "invocation %d not in progress on association %d",
						  id, sd);

	if ((result = RoURejectRequest (sd, &id, reason, priority, roi)) != NOTOK)
		freeopblk (opb);

	return result;
}
