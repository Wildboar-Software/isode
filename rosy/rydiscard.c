/* rydiscard.c - ROSY: discard invocation in progress */

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

static int	do_response (int sd, int id, int dummy, caddr_t value, struct RoSAPindication *roi);

/* DISCARD */

int	RyDiscard (int sd, int id, struct RoSAPindication *roi)
{
	struct opsblk *opb;
	missingP (roi);
	if ((opb = findopblk (sd, id, OPB_INITIATOR)) == NULLOPB)
		return rosaplose (roi, ROS_PARAMETER, NULLCP,
						  "invocation %d not in progress on association %d",
						  id, sd);
	opb -> opb_resfnx = opb -> opb_errfnx = do_response;
	return OK;
}

static int do_response (int sd, int id, int dummy, caddr_t value, struct RoSAPindication *roi) {
	return OK;
}
