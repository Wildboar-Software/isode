/* rtpkt.h - include file for reliable transfer providers (RtS-PROVIDER) */

#ifndef	_RtSAP_
#include "rtsap.h"		/* definitions for RtS-USERs */
#endif

#include "acpkt.h"		/* definitions for AcS-PROVIDERs */

#ifndef	_SSAP_
#include "ssap.h"		/* definitions for SS-USERs */
#endif

#define	rtsapPsig(acb, sd) \
{ \
    if ((acb = findacblk (sd)) == NULL) { \
	(void) sigiomask (smask); \
	return rtsaplose (rti, RTS_PARAMETER, NULLCP, \
			    "invalid association descriptor"); \
    } \
    if (!(acb -> acb_flags & ACB_RTS)) { \
	(void) sigiomask (smask); \
	return rtsaplose (rti, RTS_OPERATION, NULLCP, \
			    "not an association descriptor for RTS"); \
    } \
    if (!(acb -> acb_flags & ACB_CONN)) { \
	(void) sigiomask (smask); \
	return rtsaplose (rti, RTS_OPERATION, NULLCP, \
			    "association descriptor not connected"); \
    } \
    if (acb -> acb_flags & ACB_FINN) { \
	(void) sigiomask (smask); \
	return rtsaplose (rti, RTS_OPERATION, NULLCP, \
			    "association descriptor finishing"); \
    } \
}

#define	rtsapFsig(acb, sd) \
{ \
    if ((acb = findacblk (sd)) == NULL) { \
	(void) sigiomask (smask); \
	return rtsaplose (rti, RTS_PARAMETER, NULLCP, \
			    "invalid association descriptor"); \
    } \
    if (!(acb -> acb_flags & ACB_RTS)) { \
	(void) sigiomask (smask); \
	return rtsaplose (rti, RTS_OPERATION, NULLCP, \
			    "not an association descriptor for RTS"); \
    } \
    if (!(acb -> acb_flags & ACB_CONN)) { \
	(void) sigiomask (smask); \
	return rtsaplose (rti, RTS_OPERATION, NULLCP, \
			    "association descriptor not connected"); \
    } \
    if (!(acb -> acb_flags & ACB_FINN)) { \
	(void) sigiomask (smask); \
	return rtsaplose (rti, RTS_OPERATION, NULLCP, \
			    "association descriptor not finishing"); \
    } \
}

#ifdef __STDC__
#define	missingP(p) \
{ \
    if (p == NULL) \
	return rtsaplose (rti, RTS_PARAMETER, NULLCP, \
			    "mandatory parameter \"%s\" missing", #p); \
}
#else
#define missingP(p) \
{ \
    if (p == NULL) \
        return rtsaplose (rti, RTS_PARAMETER, NULLCP, \
                            "mandatory parameter \"%s\" missing", "p"); \
}
#endif

#ifndef	lint
#ifndef	__STDC__
#define	copyRtSAPdata(base,len,d) \
{ \
    (void) copy_capped (base, d -> d/**/_data, (ptrdiff_t) (len), \
			sizeof d -> d/**/_data, &d -> d/**/_cc); \
}
#else
#define	copyRtSAPdata(base,len,d) \
{ \
    (void) copy_capped (base, d -> d##_data, (ptrdiff_t) (len), \
			sizeof d -> d##_data, &d -> d##_cc); \
}
#endif
#else
#define	copyRtSAPdata(base,len,d)	bcopy (base, (char *) d, len)
#endif

#define	pylose() \
	rtpktlose (acb, rti, RTS_PROTOCOL, NULLCP, "%s", PY_pepy)

/* would really prefer to determine DEFAULT_CKPOINT dynamically, but can't
   since need to know it *before* doing the A-ASSOCIATE.REQUEST or
   S-CONNECT.REQUEST... */

#define	DEFAULT_CKPOINT	(65518 >> 10)
#define	DEFAULT_WINDOW	PCONN_WD_DFLT

#define	RTS_MYREQUIRE	(SR_EXCEPTIONS | SR_ACTIVITY | SR_HALFDUPLEX \
			    | SR_MINORSYNC)

#define	RT_ASN		"rtse pci version 1"

#if	USE_BUILTIN_OIDS
#define RT_ASN_OID	str2oid ("2.3.0")
#else
#define RT_ASN_OID	ode2oid (RT_ASN)
#endif

int	rtpktlose (struct assocblk*, ...), rtsaplose (struct RtSAPindication*, ...);

#define	SetPS2RtService(acb) \
{ \
    (acb) -> acb_pturnrequest = rt2pspturn; \
    (acb) -> acb_gturnrequest = rt2psgturn; \
    (acb) -> acb_transferequest = rt2pstrans; \
    (acb) -> acb_rtwaitrequest = rt2pswait; \
    (acb) -> acb_rtsetindications = rt2psasync; \
    (acb) -> acb_rtselectmask = rt2psmask; \
    (acb) -> acb_rtpktlose = rt2pslose; \
}

int ps2rtslose (
	struct assocblk *acb,
	const struct RtSAPindication *rti,
	const char *event,
	const struct PSAPabort *pa
);
int acs2rtsabort (struct assocblk *acb, const struct AcSAPabort *aca, struct RtSAPindication *rti);
int acs2rtslose (struct assocblk *acb, struct RtSAPindication *rti, const char *event, const struct AcSAPabort *aca);
void rt2pslose (struct assocblk *acb, int result);
int rt2pspturn (struct assocblk *acb, int priority, struct RtSAPindication *rti);
int rt2psgturn (struct assocblk *acb, struct RtSAPindication *rti);
int rt2pstrans (struct assocblk *acb, PE data, int secs, struct RtSAPindication *rti);
int rt2pswait (struct assocblk *acb, int secs, int trans, struct RtSAPindication *rti);
int rt2psasync (
	struct assocblk *acb,
	int (*indication)(int sd, struct RtSAPindication *rti),
	struct RtSAPindication *rti
);
int rt2psmask (struct assocblk *acb, fd_set *mask, int *nfds, struct RtSAPindication *rti);

#define	SetSS2RtService(acb) \
{ \
    (acb) -> acb_pturnrequest = rt2sspturn; \
    (acb) -> acb_gturnrequest = rt2ssgturn; \
    (acb) -> acb_transferequest = rt2sstrans; \
    (acb) -> acb_rtwaitrequest = rt2sswait; \
    (acb) -> acb_rtsetindications = rt2ssasync; \
    (acb) -> acb_rtselectmask = rt2ssmask; \
    (acb) -> acb_rtpktlose = rt2sslose; \
}

int	ss2rtslose (struct assocblk *acb, const struct RtSAPindication *rti, const char *event, const struct SSAPabort *sa), ss2rtsabort (struct assocblk *acb, const struct SSAPabort *sa, struct RtSAPindication *rti);
int	rt2sspturn (struct assocblk *acb, int priority, struct RtSAPindication *rti), rt2ssgturn (struct assocblk *acb, struct RtSAPindication *rti), rt2sstrans (struct assocblk *acb, PE data, int secs, struct RtSAPindication *rti), rt2sswait (struct assocblk *acb, int secs, int trans, struct RtSAPindication *rti),
	rt2ssasync (struct assocblk *acb, int (*indication)(int sd, struct RtSAPindication *rti), struct RtSAPindication *rti), rt2ssmask (struct assocblk *acb, fd_set *mask, int *nfds, struct RtSAPindication *rti);
void rt2sslose (struct assocblk *acb, int result);

/* RTORQ apdu */
#define	RTORQ_CKPOINT	0	/* checkpointSize tag */
#define	  RTORQ_CK_DFLT	0	/* default */
#define	RTORQ_WINDOW	1	/* windowSize tag */
#define	  RTORQ_WD_DFLT	3	/* default */
#define	RTORQ_DIALOGUE	2	/* dialogueMode tag */
#define	  RTORQ_DM_MONO 0	/* monologue */
#define	  RTORQ_DM_TWA	1	/*   two-way alternate */
#define	  RTORQ_DM_DFLT	RTORQ_DM_MONO
#define	RTORQ_CONNDATA	3	/* connectionDataRQ tag */
#define	  RTORQ_CD_OPEN 0	/*   open tag */
#define	  RTORQ_CD_RCVR	1	/*   recover tag */

/* RTOAC apdu */
#define	RTOAC_CKPOINT	0	/* checkpointSize tag */
#define	  RTOAC_CK_DFLT	0	/* default */
#define	RTOAC_WINDOW	1	/* windowSize tag */
#define	  RTOAC_WD_DFLT	3	/* default */
#define	RTOAC_CONNDATA	2	/* connectionDataAC */
#define	  RTOAC_CD_OPEN	0	/*   open tag */
#define	  RTOAC_CD_RCVR	1	/*   recover tag */

/* RTORJ apdu */
#define	RTORJ_REFUSE	0	/* refuseReason tag */
#define	RTORJ_USERDATA	1	/* userDataRJ */

/* RTAB apdu */
#define	RTAB_REASON	0	/* abortReason tag */
#define	RTAB_REFLECT	1	/* relectedParameter tag */
#define	RTAB_USERDATA	2	/* userDataAB */

extern int rtsap_priority;
