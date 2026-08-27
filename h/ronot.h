/* ronote.h - Additions to properly support ABSTRACT-BIND */

#ifndef	_RoNot_
#define	_RoNot_

#ifndef	_MANIFEST_
#include "manifest.h"
#endif
#ifndef	_GENERAL_
#include "general.h"
#endif

#ifndef	_AcSAP_
#include "acsap.h"		/* definitions for AcS-USERs */
#endif

#ifndef	_RoSAP_
#include "rosap.h"		/* definitions for RoS-USERs */
#endif

#define BIND_RESULT	1	/* indicates a bind result occured */
#define BIND_ERROR	2	/* indicates a bind error occured */

struct RoNOTindication {
	int	    rni_reason;		/* reason for failure */
#define RBI_ACSE		1	/* ACSE provider failed */
#define RBI_SET_ROSE_PRES	2	/* Failed to set ROS-USER */
#define RBI_ENC_BIND_ARG	3	/* Failed encoding bind argument */
#define RBI_ENC_BIND_RES	4	/* Failed encoding bind result */
#define RBI_ENC_BIND_ERR	5	/* Failed encoding bind error */
#define RBI_ENC_UNBIND_ARG	6	/* Failed encoding unbind argument */
#define RBI_ENC_UNBIND_RES	7	/* Failed encoding unbind result */
#define RBI_ENC_UNBIND_ERR	8	/* Failed encoding unbind error */
#define RBI_DEC_BIND_ARG	9	/* Failed decoding bind argument */
#define RBI_DEC_BIND_RES	10	/* Failed decoding bind result */
#define RBI_DEC_BIND_ERR	11	/* Failed decoding bind error */
#define RBI_DEC_UNBIND_ARG	12	/* Failed decoding unbind argument */
#define RBI_DEC_UNBIND_RES	13	/* Failed decoding unbind result */
#define RBI_DEC_UNBIND_ERR	14	/* Failed decoding unbind error */
#define RBI_DEC_NINFO		15	/* Erroneous number of user infos */

	/* diagnostics from provider */
#define	RB_SIZE	512
	int	    rni_cc;		/* length */
	char    rni_data[RB_SIZE];	/* data */
};

#ifndef	lint
#ifndef	__STDC__
#define	copyRoNOTdata(base,len,d) \
{ \
    (void) copy_capped (base, d -> d/**/_data, (ptrdiff_t) (len), \
			sizeof d -> d/**/_data, &d -> d/**/_cc); \
}
#else
#define	copyRoNOTdata(base,len,d) \
{ \
    (void) copy_capped (base, d -> d##_data, (ptrdiff_t) (len), \
			sizeof d -> d##_data, &d -> d##_cc); \
}
#endif
#else
#define	copyRoNOTdata(base,len,d)	bcopy (base, (char *) d, len)
#endif

int	ronotlose (struct RoNOTindication *rni, ...);
int acs2ronotlose (struct RoNOTindication *rni, const char *event, const struct AcSAPabort *aca);

int RoBindReject (const struct AcSAPstart *acs, const int status, const int reason, struct RoNOTindication *rni);

int RoAsynBindRequest(OID context, AEI callingtitle, AEI calledtitle,
	const struct PSAPaddr *callingaddr, const struct PSAPaddr *calledaddr,
	struct PSAPctxlist *ctxlist, OID defctxname,
	const int prequirements, const int srequirements, const long isn,
	const int settings, const struct SSAPref *ref, PE bindargpe,
	const struct QOStype *qos, struct AcSAPconnect *acc,
	struct RoNOTindication *rni, const int async);

int RoAsynBindRetry(const int ad, const int do_next_nsap, struct AcSAPconnect *acc,
	struct RoNOTindication *rni);

int RoBindInit (const int vecp, char **vec, struct AcSAPstart *acs, struct RoNOTindication *rni);

int	RoBindResult (
	const int sd,
	OID context,
	AEI respondtitle,
	const struct PSAPaddr *respondaddr,
	const struct PSAPctxlist *ctxlist,
	const int	defctxresult,
	const int	prequirements,
	const int	srequirements,
	const long isn,
	const int settings,
	const struct SSAPref *ref,
	PE bindrespe,
	struct RoNOTindication *rni
);

int RoBindError (
	const int sd,
	OID context,
	AEI respondtitle,
	const struct PSAPaddr *respondaddr,
	const struct PSAPctxlist *ctxlist,
	const int	defctxresult,
	const int	prequirements,
	const int	srequirements,
	const long isn,
	const int settings,
	const struct SSAPref *ref,
	PE binderrpe,
	struct RoNOTindication *rni
);

int RoBindReject (const struct AcSAPstart *acs, const int status, const int reason, struct RoNOTindication *rni);
int RoUnBindRequest (const int sd, PE unbindargpe, const int secs, struct AcSAPrelease *acr, struct RoNOTindication *rni);
int RoUnBindRetry (const int sd, const int secs, struct AcSAPrelease *acr, struct RoNOTindication *rni);
int RoBindUAbort (const int sd, struct RoNOTindication *rni);

int RoUnBindInit (const int sd, struct AcSAPfinish *acf, struct RoNOTindication *rni);
int RoUnBindError (const int sd, PE unbinderrpe, struct RoNOTindication *rni);
int RoUnBindResult (const int sd, PE unbindrespe, struct RoNOTindication *rni);
int RoUnBindReject (const int sd, const int status, const int reason, struct RoNOTindication *rni);

#endif
