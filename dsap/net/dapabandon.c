/* dapabandon.c - Send an abandon request */

#include "quipu/util.h"
#include "quipu/dsap.h"
#include "quipu/dap2.h"
#include "../x500as/DAS-types.h"

extern  LLog    * log_dsap;
void ros_log (const struct RoSAPpreject *rop, const char *event);

int dap_abandon (const int ad, int *id, struct ds_abandon_arg *arg, struct DSError *error) {
	struct DAPindication	  di_s;
	struct DAPindication	* di = &(di_s);

	++(*id);
	DapAbandon (ad, (*id), arg, di, ROS_INTR);
	error->dse_type = DSE_NOERROR;
	switch (di->di_type) {
	case DI_RESULT: {
		struct DAPresult	* dr = &(di->di_result);
		DAPRFREE (dr);
		return (DS_OK);
	}

	case DI_ERROR: {
		struct DAPerror	* de = &(di->di_error);
		(*error) = de->de_err;	/* struct copy */
		return (DS_ERROR_REMOTE);
	}

	case DI_PREJECT:
		error->dse_type = DSE_REMOTEERROR;
		return (DS_ERROR_PROVIDER);

	case DI_ABORT:
		error->dse_type = DSE_REMOTEERROR;
		return (DS_ERROR_CONNECT);

	default:
		error->dse_type = DSE_REMOTEERROR;
		return (DS_ERROR_PROVIDER);
	}
}

int DapAbandon (const int ad, const int id, struct ds_abandon_arg *arg, struct DAPindication *di, const int asyn) {
	PE                  arg_pe;

	if(encode_DAS_AbandonArgument(&arg_pe,1,0,NULLCP,arg) != OK) {
		return(dapreject (di, DP_INVOKE, id, NULLCP, "Abandon argument encoding failed"));
	}
	return (DapInvokeReqAux (ad, id, OP_ABANDON, arg_pe, di, asyn));
}
