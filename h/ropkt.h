/* ropkt.h - include file for remote operation providers (RoS-PROVIDER) */
/*
 * Based on an TCP-based implementation by George Michaelson of University
 * College London.
 */

#ifndef	_RoSAP_
#include "rosap.h"		/* definitions for RoS-USERs */
#endif

#include "acpkt.h"		/* definitions for AcS-PROVIDERs */

#ifndef	_RtSAP_
#include "rtsap.h"		/* definitions for RtS-USERs */
#endif

#ifndef _PSAP2_
#include "psap2.h"		/* definitions for PS-USERs */
#endif

#ifndef	_SSAP_
#include "ssap.h"		/* definitions for SS-USERs */
#endif

#define	rosapPsig(acb, sd) \
{ \
    if ((acb = findacblk (sd)) == NULL) { \
	(void) sigiomask (smask); \
	return rosaplose (roi, ROS_PARAMETER, NULLCP, \
			    "invalid association descriptor"); \
    } \
    if (!(acb -> acb_flags & ACB_CONN)) { \
	(void) sigiomask (smask); \
	return rosaplose (roi, ROS_OPERATION, NULLCP, \
			    "association descriptor not connected"); \
    } \
    if (acb -> acb_flags & ACB_CLOSING) { \
	(void) sigiomask (smask); \
	return rosaplose (roi, ROS_WAITING, NULLCP, NULLCP); \
    } \
    if (acb -> acb_flags & ACB_FINN) { \
	(void) sigiomask (smask); \
	return rosaplose (roi, ROS_OPERATION, NULLCP, \
			    "association descriptor finishing"); \
    } \
    if (acb -> acb_putosdu == NULLIFP) { \
	(void) sigiomask (smask); \
	return rosaplose (roi, ROS_OPERATION, NULLCP, \
			    "association descriptor not bound"); \
    } \
}

#define	rosapXsig(acb, sd) \
{ \
    if ((acb = findacblk (sd)) == NULL) { \
	(void) sigiomask (smask); \
	return rosaplose (roi, ROS_PARAMETER, NULLCP, \
			    "invalid association descriptor"); \
    } \
    if (!(acb -> acb_flags & ACB_CONN)) { \
	(void) sigiomask (smask); \
	return rosaplose (roi, ROS_OPERATION, NULLCP, \
			    "association descriptor not connected"); \
    } \
    if (acb -> acb_flags & ACB_FINN) { \
	(void) sigiomask (smask); \
	return rosaplose (roi, ROS_OPERATION, NULLCP, \
			    "association descriptor finishing"); \
    } \
    if (acb -> acb_putosdu == NULLIFP) { \
	(void) sigiomask (smask); \
	return rosaplose (roi, ROS_OPERATION, NULLCP, \
			    "association descriptor not bound"); \
    } \
}

#define	rosapFsig(acb, sd) \
{ \
    if ((acb = findacblk (sd)) == NULL) { \
	(void) sigiomask (smask); \
	return rosaplose (roi, ROS_PARAMETER, NULLCP, \
			    "invalid association descriptor"); \
    } \
    if (!(acb -> acb_flags & ACB_ROS)) { \
	(void) sigiomask (smask); \
	return rosaplose (roi, ROS_OPERATION, NULLCP, \
			    "not an association descriptor for ROS"); \
    } \
    if (!(acb -> acb_flags & ACB_CONN)) { \
	(void) sigiomask (smask); \
	return rosaplose (roi, ROS_OPERATION, NULLCP, \
			    "association descriptor not connected"); \
    } \
    if (!(acb -> acb_flags & ACB_FINN)) { \
	(void) sigiomask (smask); \
	return rosaplose (roi, ROS_OPERATION, NULLCP, \
			    "association descriptor not finishing"); \
    } \
    if (acb -> acb_putosdu == NULLIFP) { \
	(void) sigiomask (smask); \
	return rosaplose (roi, ROS_OPERATION, NULLCP, \
			    "association descriptor not bound"); \
    } \
}

#ifdef __STDC__
#define missingP(p) \
{ \
    if (p == NULL) \
        return rosaplose (roi, ROS_PARAMETER, NULLCP, \
                            "mandatory parameter \"%s\" missing",#p); \
}
#else
#define	missingP(p) \
{ \
    if (p == NULL) \
	return rosaplose (roi, ROS_PARAMETER, NULLCP, \
			    "mandatory parameter \"%s\" missing", "p"); \
}
#endif

#ifndef	lint
#ifndef	__STDC__
#define	copyRoSAPdata(base,len,d) \
{ \
    register int i = len; \
    if ((d -> d/**/_cc = min (i, sizeof d -> d/**/_data)) > 0) \
	bcopy (base, d -> d/**/_data, d -> d/**/_cc); \
}
#else
#define	copyRoSAPdata(base,len,d) \
{ \
    register int i = len; \
    if ((d -> d##_cc = min (i, sizeof d -> d##_data)) > 0) \
	bcopy (base, d -> d##_data, d -> d##_cc); \
}
#endif
#else
#define	copyRoSAPdata(base,len,d)	bcopy (base, (char *) d, len)
#endif

#define	pylose() \
	ropktlose (acb, roi, ROS_PROTOCOL, NULLCP, "%s", PY_pepy)

int	ropktlose (struct assocblk *acb, ...);
int	rosapreject (struct assocblk *acb, ...);

int	acb2osdu (struct assocblk *acb, int *invokeID, PE pe, struct RoSAPindication *roi);

int	ro2rtswrite (struct assocblk *acb, PE pe, PE fe, int priority, struct RoSAPindication *roi), ro2rtswait (struct assocblk *acb, int *invokeID, int secs, struct RoSAPindication *roi), ro2rtsready (struct assocblk *acb, int priority, struct RoSAPindication *roi), ro2rtsasync (struct assocblk *acb, int (*indication)(int sd, struct RoSAPindication *roi), struct RoSAPindication *roi),
	ro2rtsmask (struct assocblk *acb, fd_set *mask, int *nfds, struct RoSAPindication *roi);

int	ro2pswrite (struct assocblk *acb, PE pe, PE fe, int priority, struct RoSAPindication *roi), ro2pswait (struct assocblk *acb, int *invokeID, int secs, struct RoSAPindication *roi), ro2psasync (struct assocblk *acb, int (*indication)(int sd, struct RoSAPindication *roi), struct RoSAPindication *roi), ro2psmask (struct assocblk *acb, fd_set *mask, int *nfds, struct RoSAPindication *roi);

int	ss2roslose (struct assocblk *acb, struct RoSAPindication *roi, char *event, struct SSAPabort *sa), ss2rosabort (struct assocblk *acb, struct SSAPabort *sa, struct RoSAPindication *roi);
int	ro2sswrite (struct assocblk *acb, PE pe, PE fe, int priority, struct RoSAPindication *roi), ro2sswait (struct assocblk *acb, int *invokeID, int secs, struct RoSAPindication *roi), ro2ssasync (struct assocblk *acb, int (*indication)(int sd, struct RoSAPindication *roi), struct RoSAPindication *roi), ro2ssmask (struct assocblk *acb, fd_set *mask, int *nfds, struct RoSAPindication *roi), ro2sslose (struct assocblk *acb, int result),
	ro2ssready (struct assocblk *acb, int priority, struct RoSAPindication *roi);

int RoURejectRequestAux (
	struct assocblk *acb,
	int *invokeID,
	int reason,
	PElementID id,
	int priority,
	struct RoSAPindication *roi
);

/* APDU types */
#define	APDU_INVOKE	1	/* Invoke */
#define	APDU_RESULT	2	/* Return result */
#define	APDU_ERROR	3	/* Return error */
#define	APDU_REJECT	4	/* Reject */

#define	APDU_UNKNOWN	(-1)	/* anything other than the above */

/* Reject APDU types */
#define	REJECT_GENERAL	0	/* General Problem */
#define	  REJECT_GENERAL_BASE	ROS_GP_UNRECOG
#define	REJECT_INVOKE	1	/* Invoke Problem */
#define	  REJECT_INVOKE_BASE	ROS_IP_DUP
#define	REJECT_RESULT	2	/* Return Result Problem */
#define	  REJECT_RESULT_BASE	ROS_RRP_UNRECOG
#define	REJECT_ERROR	3	/* Return Error Problem */
#define	  REJECT_ERROR_BASE	ROS_REP_UNRECOG
#define	REJECT_COMPLETE	4	/* more Invoke Problem codes */
