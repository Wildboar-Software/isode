/* rydispatch.c - ROSY: dispatch  */

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
/* DISPATCH */

int	RyDispatch (
	const int sd,
	const struct RyOperation *ryo,
	const int op,
	int (*fnx)(int sd, struct RyOperation *ryo, struct RoSAPinvoke *rox, caddr_t in, struct RoSAPindication *roi),
	struct RoSAPindication *roi
) {
	struct dspblk *dsb;

	missingP (roi);
	if ((dsb = finddsblk (sd, op)) == NULLDSB) {
		missingP (ryo);
		missingP (fnx);
		for (; ryo -> ryo_name; ryo++)
			if (ryo -> ryo_op == op)
				break;
		if (!ryo -> ryo_name)
			return rosaplose (roi, ROS_PARAMETER, NULLCP,
							  "unknown operation code %d", op);
		if ((dsb = newdsblk (sd, ryo)) == NULLDSB)
			return rosaplose (roi, ROS_CONGEST, NULLCP, NULLCP);
	} else if (ryo)
		dsb -> dsb_ryo = ryo;
	if ((dsb -> dsb_vector = fnx) == NULLIFP)
		freedsblk (dsb);
	return OK;
}
