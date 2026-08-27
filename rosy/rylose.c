/* rylose.c - ROSY: clean-up after association termination */
#include "rosy.h"

#ifdef __STDC__
#define	missingP(p) \
{ \
    if (p == NULL) \
	return rosaplose (roi, ROS_PARAMETER, NULLCP, \
			    "mandatory parameter \"%s\" missing", #p); \
}
#else
#define missingP(p) \
{ \
    if (p == NULL) \
        return rosaplose (roi, ROS_PARAMETER, NULLCP, \
                            "mandatory parameter \"%s\" missing", "p"); \
}
#endif

/*    clean-up after association termination */

int	RyLose (int sd, struct RoSAPindication *roi) {
	missingP (roi);
	loseopblk (sd, ROS_DONE);
	losedsblk (sd);
	return OK;
}
