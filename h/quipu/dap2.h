/* dap.h - */

#ifndef QUIPUDAP2
#define QUIPUDAP2

#ifndef	_MANIFEST_
#include "manifest.h"
#endif
#ifndef	_GENERAL_
#include "general.h"
#endif

#ifndef	_RoSAP_
#include "rosap.h"		/* definitions for ROSE-USERs */
#endif

#ifndef	_RoNot_
#include "ronot.h"		/* definitions for RoBIND-USERs */
#endif

#include "quipu/common.h"
#include "quipu/dsargument.h"
#include "quipu/bind.h"

/* An additional value for "asyn" parameters for interruptibility */
#define ROS_INTR	-1

struct DAPconnect {
	int	    dc_sd;		/* association descriptor */

	int     ds_pctx_id;		/* Context identifier of directory access AS */

	struct AcSAPconnect dc_connect;/* info from A-CONNECT.CONFIRMATION */

	int	    dc_result;		/* result */
#define DC_RESULT	1
#define DC_ERROR	2
#define DC_REJECT	3

	union {
		struct ds_bind_arg	  dc_bind_res;	/* DC_RESULT */
		struct ds_bind_error	  dc_bind_err;	/* DC_ERROR */
	} dc_un;
};
#ifndef DCFREE
#define	DCFREE(dc) { \
    ACCFREE (&((dc) -> dc_connect)); \
    switch ((dc) -> dc_result) { \
	case DC_RESULT: \
	    bind_arg_free (&(dc) -> dc_un.dc_bind_res); \
	    break; \
	case DC_ERROR: \
	    break; \
	default: \
	    break; \
    } \
}
#endif

struct DAPrelease {		/* DAP-UNBIND.CONFIRMATION */
	int	    dr_affirmative;	/* T   = connection released
				   NIL = request refused */

	int	    dr_reason;		/* reason for result */
	/* Takes values from acr_reason */
};

struct DAPabort {		/* DAP-ABORT.INDICATION */
	int	    da_source;		/* abort source */
#define	DA_USER		0	/* DAP-user */
#define	DA_PROVIDER	1	/* DAP-provider */
#define	DA_LOCAL	2	/*   DAP interface internal error */

	int	    da_reason;		/* reason for failure */
#define DA_NO_REASON	0	/* Ain't no reason to some things */
#define DA_RONOT	1	/* Error from RONOT provider */
#define DA_ROS		2	/* Error from ROS provider called */
#define DA_ARG_ENC 	3	/* Error encoding argument */
#define DA_RES_ENC 	4	/* Error encoding result */
#define DA_ERR_ENC 	5	/* Error encoding error */
#define DA_ARG_DEC 	6	/* Error decoding argument */
#define DA_RES_DEC	7	/* Error decoding result */
#define DA_ERR_DEC 	8	/* Error decoding error */

	/* diagnostics from provider */
#define	DA_SIZE	512
	int	    da_cc;		/* length */
	char    da_data[DA_SIZE];	/* data */
};

struct DAPresult {
	int			  dr_id;
	struct DSResult	  dr_res;	/* Decoded result and op type */
};
#define DAPRFREE(dr) ds_res_free (&((dr)->dr_res))

struct DAPerror {
	int			  de_id;
	struct DSError	  de_err;	/* Decoded error and error type */
};
#define DAPEFREE(de) ds_error_free (&((de)->de_err))

struct DAPpreject {
	int		  dp_id;	/* Operation id or -1 */

	int		  dp_source;	/* same values as DAPabort.da_source */

	int		  dp_reason;	/* reason for failure */
#define DP_NO_REASON	0	/* Ain't no reason to some things */
#define DP_ROS		1	/* ROSE error */
#define DP_INVOKE	2	/* Failure during invocation */
#define DP_RESULT	3	/* Failure during result */
#define DP_ERROR	4	/* Failure during error */

	/* diagnostics from provider */
#define	DP_SIZE	512
	int	    dp_cc;		/* length */
	char    dp_data[DP_SIZE];	/* data */
};

struct DAPindication {
	int	    di_type;
#define DI_RESULT	2	/* DAP operation result received */
#define DI_ERROR	3	/* DAP operation error received */
#define DI_PREJECT	4	/* DAP operation rejected */
#define	DI_ABORT	6	/* DAP association lost */
	union {
		struct DAPresult	di_un_result;
		struct DAPerror		di_un_error;
		struct DAPpreject	di_un_preject;
		struct DAPabort		di_un_abort;
	} di_un;
#ifndef di_result
#define di_result di_un.di_un_result
#endif

#ifndef di_error
#define di_error di_un.di_un_error
#endif

#ifndef di_preject
#define di_preject di_un.di_un_preject
#endif

#ifndef di_abort
#define di_abort di_un.di_un_abort
#endif
};

int	daplose (struct DAPindication *di, ...);
int	dapreject (struct DAPindication *di, ...);
int DapAbandon (int ad, int id, struct ds_abandon_arg *arg, struct DAPindication *di, int asyn);
int DapBindDecode (struct AcSAPconnect *acc, struct DAPconnect *dc, struct RoNOTindication *rni);
int DapInvokeReqAux (int sd, int id, int op, PE pe, struct DAPindication *di, int asyn);
int DapDecodeResult (int sd, struct RoSAPresult *ror, struct DAPindication *di);
int DapDecodeError (int sd, struct RoSAPerror *roe, struct DAPindication *di);

#ifndef	lint
#ifndef	__STDC__
#define	copyDAPdata(base,len,d) \
{ \
    register int i = len; \
    if ((d -> d/**/_cc = min (i, sizeof d -> d/**/_data)) > 0) \
	bcopy (base, d -> d/**/_data, d -> d/**/_cc); \
}
#else
#define	copyDAPdata(base,len,d) \
{ \
    register int i = len; \
    if ((d -> d##_cc = min (i, sizeof d -> d##_data)) > 0) \
	bcopy (base, d -> d##_data, d -> d##_cc); \
}
#endif
#else
#define	copyDAPdata(base,len,d)	bcopy (base, (char *) d, len)
#endif

/* DAP-BIND.REQUEST (ARGUMENT) */
int DapAsynBindReqAux (AEI callingtitle, AEI calledtitle, struct PSAPaddr *callingaddr,
	struct PSAPaddr *calledaddr, int prequirements, int srequirements, long isn, int settings,
	struct SSAPref *sf, struct ds_bind_arg *bindarg, struct QOStype *qos, struct DAPconnect *dc, struct DAPindication *di, int async);
/* DAP-BIND.REQUEST (ARGUMENT) */
int DapAsynBindRequest (struct PSAPaddr *calledaddr, struct ds_bind_arg *bindarg, struct DAPconnect *dc, struct DAPindication *di, int async);	/* DAP-BIND.REQUEST (ARGUMENT) */
/* DAP-BIND-RETRY.REQUEST */
int DapAsynBindRetry (int sd, int do_next_nsap, struct DAPconnect *dc, struct DAPindication *di);

int DapUnBindRequest (int sd, int secs, struct DAPrelease *dr, struct DAPindication *di);	/* DAP-UNBIND.REQUEST */
int DapUnBindRetry (int sd, int secs, struct DAPrelease *dr, struct DAPindication *di);	/* DAP-BIND-RETRY.REQUEST (pseudo) */

int ronot2daplose (struct DAPindication * di, char * event, struct RoNOTindication * rni);
int ros2daplose (struct DAPindication *di, char *event, struct RoSAPpreject *rop);
int ros2dapreject (struct DAPindication *di, char *event, struct RoSAPureject *rou);

int DapRead (int ad, int id, struct ds_read_arg *arg, struct DAPindication *di, int asyn);
int DapCompare (int ad, int id, struct ds_compare_arg *arg, struct DAPindication *di, int asyn);
int DapAbandon (int ad, int id, struct ds_abandon_arg *arg, struct DAPindication *di, int asyn);
int DapList (int ad, int id, struct ds_list_arg *arg, struct DAPindication *di, int asyn);
int DapSearch (int ad, int id, struct ds_search_arg *arg, struct DAPindication *di, int asyn);
int DapAddEntry (int ad, int id, struct ds_addentry_arg *arg, struct DAPindication *di, int asyn);
int DapRemoveEntry (int ad, int id, struct ds_removeentry_arg *arg, struct DAPindication *di, int asyn);
int DapModifyEntry (int ad, int id, struct ds_modifyentry_arg *arg, struct DAPindication *di, int asyn);
int DapModifyRDN (int ad, int id, struct ds_modifyrdn_arg *arg, struct DAPindication *di, int asyn);

#endif
