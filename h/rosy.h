/* rosy.h - include file for ROSY users */

#ifndef	_ROSY_
#define	_ROSY_

#ifndef	_MANIFEST_
#include "manifest.h"
#endif
#ifndef	_GENERAL_
#include "general.h"
#endif

#ifndef	_RoSAP_
#include "rosap.h"		/* definitions for RoS-USERs */
#endif

#ifdef PEPSY_VERSION
#include "pepsy.h"
#endif

#ifdef PEPSY_DEFINITIONS
/* This is really to change the name of the function RyOperation but the macro
 * is unselective so we have to change the name of the structure as well, which
 * shouldn't cause any problems
 */
#define RyOperation		PRyOperation
#endif /* PEPSY_DEFINITIONS */
struct RyOperation {
	char   *ryo_name;		/* operation name */
	int	    ryo_op;		/* operation code */

#ifdef PEPSY_DEFINITIONS
	modtyp *ryo_arg_mod;	/* pointer to table for arguement type */
	int	    ryo_arg_index;	/* index to entry in tables */
#else
	int	  (*ryo_arg_encode)(PE *pe, int top, int len, char *buffer, caddr_t parm);
	int	  (*ryo_arg_decode)(PE pe, int top, int *len, char **buffer, caddr_t *parm);
	int	  (*ryo_arg_free)(caddr_t parm);
#endif

	int	    ryo_result;		/* result possible */
#ifdef PEPSY_DEFINITIONS
	modtyp *ryo_res_mod;	/* pointer to table for result type */
	int	    ryo_res_index;	/* index to entry in tables */
#else
	int	  (*ryo_res_encode)(PE *pe, int top, int len, char *buffer, caddr_t parm);
	int	  (*ryo_res_decode)(PE pe, int top, int *len, char **buffer, caddr_t *parm);
	int	  (*ryo_res_free)(caddr_t parm);	/* frees     .. */
#endif

	struct RyError **ryo_errors;/* errors possible */
};

struct RyError {
	char   *rye_name;		/* error name */
	int	    rye_err;		/* error code */

#ifdef PEPSY_DEFINITIONS
	modtyp *rye_param_mod;	/* pointer to table for result type */
	int	    rye_param_index;	/* index to entry in tables */
#else
	int	  (*rye_param_encode)(PE *pe, int top, int len, char *buffer, caddr_t parm);
	int	  (*rye_param_decode)(PE pe, int top, int *len, char **buffer, caddr_t *parm);
	int	  (*rye_param_free)(caddr_t parm);	/* frees     .. */
#endif
};

struct opsblk {
	struct opsblk *opb_forw;	/* doubly-linked list */
	struct opsblk *opb_back;	/* .. */

	short	opb_flags;	/* our state */
#define	OPB_NULL	0x0000
#define	OPB_EVENT	0x0001	/* event present */
#define	OPB_INITIATOR	0x0002	/* initiator block */
#define	OPB_RESPONDER	0x0004	/* responder block */

	int	    opb_fd;		/* association descriptor */
	int	    opb_id;		/* invoke ID */

	int (*opb_resfnx)(int sd, int id, int dummy, caddr_t result, struct RoSAPindication *roi);		/* result vector */
	void (*opb_errfnx)(int sd, int id, int error, caddr_t parameter, struct RoSAPindication *roi);		/* error vector */

	struct RyOperation *opb_ryo;/* entry in operation table */

	struct RoSAPindication opb_event;	/* resulting event */
	caddr_t opb_out;			/* event parameter */
#ifdef PEPSY_DEFINITIONS
	modtyp *opb_free_mod;	/* pointer to table for result type */
	int	    opb_free_index;	/* index to entry in tables */
#else
	int	  (*opb_free)(caddr_t parm);		/* free routine for event parameter */
#endif

	PE	    opb_pe;		/* for Simon */
};
#define	NULLOPB		((struct opsblk *) 0)

struct dspblk {
	struct dspblk *dsb_forw;	/* doubly-linked list */
	struct dspblk *dsb_back;	/* .. */

	int	    dsb_fd;		/* association descriptor */
	/* NOTOK-valued is wildcard for RyWait */

	struct RyOperation *dsb_ryo; /* entry in operation table */

	int (*dsb_vector)(int sd, struct RyOperation *ryo, struct RoSAPinvoke *rox, caddr_t in, struct RoSAPindication *roi); /* dispatch vector */
};
#define	NULLDSB		((struct dspblk *) 0)

#define	RY_RESULT	(-1)	/* distinguishes RESULTs from ERRORs */
#define	RY_REJECT	(-2)	/* distinguishes REJECTs from ERRORs */

/* Change the names to use the pepsy based rosy library */
#ifdef PEPSY_DEFINITIONS
#define RyDiscard		PRyDiscard
#define RyDispatch		PRyDispatch
#define RyDsError		PRyDsError
#define RyDsResult		PRyDsResult
#define RyDsUReject		PRyDsUReject
#define RyGenID			PRyGenID
#define RyLose			PRyLose
#define RyOpInvoke		PRyOpInvoke
#define RyStub			PRyStub
#define RyWait			PRyWait
#define RyWaitAux		PRyWaitAux
#define finddsblk		Pfinddsblk
#define finderrbyerr		Pfinderrbyerr
#define finderrbyname		Pfinderrbyname
#define findopblk		Pfindopblk
#define findopbyname		Pfindopbyname
#define findopbyop		Pfindopbyop
#define firstopblk		Pfirstopblk
#define freedsblk		Pfreedsblk
#define freeopblk		Pfreeopblk
#define losedsblk		Plosedsblk
#define loseopblk		Ploseopblk
#define newdsblk		Pnewdsblk
#define newopblk		Pnewopblk
#endif

/* WAIT */
int	RyWait (
	const int sd,
	const int *id,
	caddr_t *out,
	const int secs,
	struct RoSAPindication *roi
);

int	RyWaitAux (
	const int sd,
	const struct opsblk *opb,
	caddr_t *out,
	const int secs,
	struct RoSAPindication *roi
);

/* Initiator */
/* STUB */
int	RyStub (
	const int sd,
	struct RyOperation *ryo,
	const int op,
	const int id,
	int *linked,
	caddr_t in,
	int (*rfx)(int sd, int id, int dummy, caddr_t result, struct RoSAPindication *roi),
	void (*efx)(int sd, int id, int error, caddr_t parameter, struct RoSAPindication *roi),
	const int class,
	struct RoSAPindication *roi
);
#define	ROS_INTR	2	/*   invoke stub but return on interrupt */
int	RyDiscard (const int sd, const int id, struct RoSAPindication *roi);		/* DISCARD */

/* OPERATION */
int	RyOperation (
	const int sd,
	struct RyOperation *ryo,
	const int op,
	caddr_t in,
	caddr_t *out,
	int *response,
	struct RoSAPindication *roi
);

/* INVOKE */
int	RyOpInvoke (
	const int sd,
	const struct RyOperation *ryo,
	const int op,
	caddr_t in,
	caddr_t *out,
	int (*rfx)(int sd, int id, int dummy, caddr_t result, struct RoSAPindication *roi),
	void (*efx)(int sd, int id, int error, caddr_t parameter, struct RoSAPindication *roi),
	const int class,
	const int invokeID,
	const int *linkedID,
	const int priority,
	struct RoSAPindication *roi
);
int	RyGenID (const int sd);		/* generate unique invoke ID */

/* Responder */
/* DISPATCH */
int	RyDispatch (
	const int sd,
	const struct RyOperation *ryo,
	const int op,
	int (*fnx)(int sd, struct RyOperation *ryo, struct RoSAPinvoke *rox, caddr_t in, struct RoSAPindication *roi),
	struct RoSAPindication *roi
);
int	RyDsResult (const int sd, const int id, caddr_t out, const int priority, struct RoSAPindication *roi);		/* RESULT */
int	RyDsError (const int sd, const int id, const int err, caddr_t out, const int priority, struct RoSAPindication *roi);		/* ERROR */
int	RyDsUReject (const int sd, const int id, const int reason, const int priority, struct RoSAPindication *roi);		/* U-REJECT */

int	RyLose (const int sd, struct RoSAPindication *roi);		/* clean-up after association termination */

struct RyOperation *findopbyop (struct RyOperation *ryo, const int op), *findopbyname (struct RyOperation *ryo, const char *name);

struct RyError *finderrbyerr (struct RyError *rye, const int err), *finderrbyname (struct RyError *rye, const char *name);

void freeopblk (struct opsblk *opb);
void loseopblk (const int sd, const int reason);
struct opsblk *newopblk (const int sd, const int id), *findopblk (const int sd, const int id, const int flags), *firstopblk (const int sd);

void freedsblk (struct dspblk *dsb);
void losedsblk (const int sd);
struct dspblk *newdsblk (const int sd, const struct RyOperation *ryo), *finddsblk (const int sd, const int op);

#endif
