/* dsapresult.c - DSAP : Send operation results */

#include <stdio.h>
#include "logger.h"
#include "quipu/dsap.h"
#include "../x500as/DAS-types.h"
#include "../x500as/Quipu-types.h"
#include "quipu/watchdog.h"
static int DapEncodeResult (PE *pep, struct DSResult *res);
static int DspEncodeResult (PE *pep, struct ds_op_res *res);
static int QspEncodeResult (PE *pep, struct ds_op_res *res);
static int IspEncodeResult (PE *pep, struct ds_op_res *res);


extern  LLog    * log_dsap;

static int DapEncodeResult (PE *pep, struct DSResult *res) {
	int		success;

	switch(res->result_type) {
	case    OP_READ :
		success = encode_DAS_ReadResult(pep,1,0,NULLCP,&(res->res_rd));
		break;
	case    OP_COMPARE :
		success = encode_DAS_CompareResult(pep,1,0,NULLCP,&(res->res_cm));
		break;
	case    OP_ABANDON :
		success = encode_DAS_AbandonResult(pep,1,0,NULLCP,NULLCP);
		break;
	case    OP_LIST :
		success = encode_DAS_ListResult(pep,1,0,NULLCP,&(res->res_ls));
		break;
	case    OP_SEARCH :
		merge_search_results (&res->res_sr,res->res_sr.srr_next);
		success = encode_DAS_SearchResult(pep,1,0,NULLCP,&(res->res_sr));
		break;
	case    OP_ADDENTRY :
		success = encode_DAS_AddEntryResult(pep,1,0,NULLCP,NULLCP);
		break;
	case    OP_REMOVEENTRY :
		success = encode_DAS_RemoveEntryResult(pep,1,0,NULLCP,NULLCP);
		break;
	case    OP_MODIFYENTRY :
		success = encode_DAS_ModifyEntryResult(pep,1,0,NULLCP,NULLCP);
		break;
	case    OP_MODIFYRDN :
		success = encode_DAS_ModifyRDNResult(pep,1,0,NULLCP,NULLCP);
		break;
	default :
		success = NOTOK;
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("DapEncodeResult(): unknown op type %d", res->result_type));
		break;
	}
	return(success);
}

int DapResultRequest (int sd, int id, struct DSResult *res, struct DSAPindication *di) {
	int				  result;
	PE				  res_pe;
	struct RoSAPindication	  roi_s;
	struct RoSAPindication	* roi = &(roi_s);
	struct RoSAPpreject		* rop = &(roi->roi_preject);

	if (DapEncodeResult (&(res_pe), res) != OK) {
		LLOG (log_dsap, LLOG_EXCEPTIONS, ("DapResultRequest: Encoding failed"));
		return (dsapreject (di, DP_INVOKE, id, NULLCP, "Failed to encode operation result"));
	}
	watch_dog ("RoResultRequest (DAP)");
	result = RoResultRequest (sd, id, res->result_type, res_pe, ROS_NOPRIO, roi);
	watch_dog_reset();
	if (result != OK) {
		if (ROS_FATAL (rop->rop_reason) || (rop->rop_reason == ROS_PARAMETER)) {
			LLOG (log_dsap, LLOG_EXCEPTIONS, ("DapResultRequest(): Fatal rejection"));
			return (dsaplose (di, DP_INVOKE, NULLCP, "RoResultRequest failed"));
		} else {
			LLOG (log_dsap, LLOG_EXCEPTIONS, ("DapResultRequest(): Non-Fatal rejection"));
			return (dsapreject (di, DP_INVOKE, id, NULLCP, "RoResultRequest failed"));
		}
	}
	if (res_pe != NULLPE)
		pe_free (res_pe);
	return (OK);
}

static int DspEncodeResult (PE *pep, struct ds_op_res *res) {
	int		success;

	switch(res->dcr_dsres.result_type) {
	case    OP_READ :
		success = encode_DO_ChainedReadResult(pep,1,0,NULLCP,res);
		break;
	case    OP_COMPARE :
		success = encode_DO_ChainedCompareResult(pep,1,0,NULLCP,res);
		break;
	case    OP_ABANDON :
		success = encode_DAS_AbandonResult(pep,1,0,NULLCP,NULLCP);
		break;
	case    OP_LIST :
		success = encode_DO_ChainedListResult(pep,1,0,NULLCP,res);
		break;
	case    OP_SEARCH :
		merge_search_results (&res->dcr_dsres.res_sr,res->dcr_dsres.res_sr.srr_next);
		success = encode_DO_ChainedSearchResult(pep,1,0,NULLCP,res);
		break;
	case    OP_ADDENTRY :
		success = encode_DO_ChainedAddEntryResult(pep,1,0,NULLCP,res);
		break;
	case    OP_REMOVEENTRY :
		success = encode_DO_ChainedRemoveEntryResult(pep,1,0,NULLCP,res);
		break;
	case    OP_MODIFYENTRY :
		success = encode_DO_ChainedModifyEntryResult(pep,1,0,NULLCP,res);
		break;
	case    OP_MODIFYRDN :
		success = encode_DO_ChainedModifyRDNResult(pep,1,0,NULLCP,res);
		break;
	default:
		success = NOTOK;
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("DspEncodeResult(): unknown op type %d", res->dcr_dsres.result_type));
		break;
	}
	return(success);
}

int DspResultRequest (int sd, int id, struct ds_op_res *res, struct DSAPindication *di) {
	int				  result;
	PE				  res_pe;
	struct RoSAPindication	  roi_s;
	struct RoSAPindication	* roi = &(roi_s);
	struct RoSAPpreject		* rop = &(roi->roi_preject);

	if (DspEncodeResult (&(res_pe), res) != OK) {
		LLOG (log_dsap, LLOG_EXCEPTIONS, ("DspResultRequest: Encoding failed"));
		return (dsapreject (di, DP_INVOKE, id, NULLCP, "Failed to encode operation result"));
	}
	watch_dog ("RoResultRequest (DSP)");
	result = RoResultRequest (sd, id, res->dcr_dsres.result_type, res_pe,
							  ROS_NOPRIO, roi);
	watch_dog_reset();
	if (result != OK) {
		if (ROS_FATAL (rop->rop_reason) || (rop->rop_reason == ROS_PARAMETER)) {
			LLOG (log_dsap, LLOG_EXCEPTIONS, ("DspResultRequest(): Fatal rejection"));
			return (dsaplose (di, DP_INVOKE, NULLCP, "RoResultRequest failed"));
		} else {
			LLOG (log_dsap, LLOG_EXCEPTIONS, ("DspResultRequest(): Non-Fatal rejection"));
			return (dsapreject (di, DP_INVOKE, id, NULLCP, "RoResultRequest failed"));
		}
	}
	if (res_pe != NULLPE)
		pe_free (res_pe);
	return (OK);
}

static int QspEncodeResult (PE *pep, struct ds_op_res *res) {
	int		success;

	switch(res->dcr_dsres.result_type) {
	case    OP_READ :
		success = encode_DO_ChainedReadResult(pep,1,0,NULLCP,res);
		break;
	case    OP_COMPARE :
		success = encode_DO_ChainedCompareResult(pep,1,0,NULLCP,res);
		break;
	case    OP_ABANDON :
		success = encode_DAS_AbandonResult(pep,1,0,NULLCP,NULLCP);
		break;
	case    OP_LIST :
		success = encode_DO_ChainedListResult(pep,1,0,NULLCP,res);
		break;
	case    OP_SEARCH :
		merge_search_results (&res->dcr_dsres.res_sr,res->dcr_dsres.res_sr.srr_next);
		success = encode_DO_ChainedSearchResult(pep,1,0,NULLCP,res);
		break;
	case    OP_ADDENTRY :
		success = encode_DO_ChainedAddEntryResult(pep,1,0,NULLCP,res);
		break;
	case    OP_REMOVEENTRY :
		success = encode_DO_ChainedRemoveEntryResult(pep,1,0,NULLCP,res);
		break;
	case    OP_MODIFYENTRY :
		success = encode_DO_ChainedModifyEntryResult(pep,1,0,NULLCP,res);
		break;
	case    OP_MODIFYRDN :
		success = encode_DO_ChainedModifyRDNResult(pep,1,0,NULLCP,res);
		break;
	case    OP_GETEDB :
		success = encode_Quipu_GetEntryDataBlockResult(pep,1,0,NULLCP,&(res->dcr_dsres.res_ge));
		break;
	default:
		success = NOTOK;
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("QspEncodeResult(): unknown op type %d", res->dcr_dsres.result_type));
		break;
	}
	return(success);
}

int QspResultRequest (int sd, int id, struct ds_op_res *res, struct DSAPindication *di) {
	int				  result;
	PE				  res_pe;
	struct RoSAPindication	  roi_s;
	struct RoSAPindication	* roi = &(roi_s);
	struct RoSAPpreject		* rop = &(roi->roi_preject);

	if (QspEncodeResult (&(res_pe), res) != OK) {
		LLOG (log_dsap, LLOG_EXCEPTIONS, ("QspResultRequest: Encoding failed"));
		return (dsapreject (di, DP_INVOKE, id, NULLCP, "Failed to encode operation result"));
	}
	watch_dog ("RoResultRequest (QSP)");
	result = RoResultRequest (sd, id, res->dcr_dsres.result_type, res_pe,
							  ROS_NOPRIO, roi);
	watch_dog_reset();
	if (result != OK) {
		if (ROS_FATAL (rop->rop_reason) || (rop->rop_reason == ROS_PARAMETER)) {
			LLOG (log_dsap, LLOG_EXCEPTIONS, ("QspResultRequest(): Fatal rejection"));
			return (dsaplose (di, DP_INVOKE, NULLCP, "RoResultRequest failed"));
		} else {
			LLOG (log_dsap, LLOG_EXCEPTIONS, ("QspResultRequest(): Non-Fatal rejection"));
			return (dsapreject (di, DP_INVOKE, id, NULLCP, "RoResultRequest failed"));
		}
	}
	if (res_pe != NULLPE)
		pe_free (res_pe);
	return (OK);
}

int IspResultRequest (int sd, int id, struct ds_op_res *res, struct DSAPindication *di) {
	return QspResultRequest (sd, id, res, di);
}

int IspEncodeResult (PE *pep, struct ds_op_res *res) {
	return QspEncodeResult (pep, res);
}
