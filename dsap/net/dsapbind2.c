/* dsapbind2.c - DSAP: DirectoryBind mapping onto ABSTRACT-BIND */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/net/RCS/dsapbind2.c,v 9.0 1992/06/16 12:14:05 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/net/RCS/dsapbind2.c,v 9.0 1992/06/16 12:14:05 isode Rel $
 *
 *
 * $Log: dsapbind2.c,v $
 * Revision 9.0  1992/06/16  12:14:05  isode
 * Release 8.0
 *
 */

/*
 *                                NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */


/* LINTLIBRARY */

#include "logger.h"
#include "tailor.h"
#include "quipu/dsap.h"
#include "../x500as/DAS-types.h"

extern LLog	* log_dsap;

/*    D-BIND.INDICATION */

/* ARGSUSED */

int
DBindInit (int vecp, char **vec, struct DSAPstart *ds, struct DSAPindication *di) {
	int			  result;
	struct RoNOTindication	  rni_s;
	struct RoNOTindication	* rni = &(rni_s);
	struct AcSAPstart	* acs = &(ds->ds_start);
	struct ds_bind_arg	* bind_arg;

	watch_dog ("RoBindInit");
	if (result = RoBindInit (vecp, vec, acs, rni) != OK) {
		DLOG (log_dsap, LLOG_EXCEPTIONS, ("ROSE bind failed with reason %d", result));
		watch_dog_reset ();
		return (ronot2dsaplose (di, "D-BIND.INDICATION", rni));
	}
	watch_dog_reset ();

	/* ADT: Generalised context checking support would be useful */

	/* Check application context and set flag in ds->ds_ctx */

	ds->ds_sd = acs->acs_sd;

	switch (ds->ds_ctx = select_context (acs->acs_context)) {
	case DS_CTX_X500_DAP:
		if ((ds->ds_pctx_id = check_dap_ctxlist (&(acs->acs_start.ps_ctxlist))) == NOTOK) {
			LLOG(log_dsap,LLOG_EXCEPTIONS, ("Unacceptable Abstract Syntaxes for X.500 DAP"));
			watch_dog ("RoBindReject (DAP)");
			RoBindReject (acs, ACS_TRANSIENT, ACS_USER_NOREASON, rni);
			watch_dog_reset ();
			return (dsaplose (di, DA_PCDL, NULLCP, "DAP BIND INDICATION"));
		}
		break;

	case DS_CTX_X500_DSP:
		if ((ds->ds_pctx_id = check_dsp_ctxlist (&(acs->acs_start.ps_ctxlist))) == NOTOK) {
			LLOG(log_dsap,LLOG_EXCEPTIONS, ("Unacceptable Abstract Syntaxes for X.500 DSP"));
			watch_dog ("RoBindReject (DSP)");
			RoBindReject (acs, ACS_TRANSIENT, ACS_USER_NOREASON, rni);
			watch_dog_reset ();
			return (dsaplose (di, DA_PCDL, NULLCP, "DSP BIND INDICATION"));
		}
		break;

	case DS_CTX_QUIPU_DSP:
		if ((ds->ds_pctx_id = check_qsp_ctxlist (&(acs->acs_start.ps_ctxlist))) == NOTOK) {
			LLOG(log_dsap,LLOG_EXCEPTIONS, ("Unacceptable Abstract Syntaxes for QUIPU DSP"));
			watch_dog ("RoBindReject (QSP)");
			RoBindReject (acs, ACS_TRANSIENT, ACS_USER_NOREASON, rni);
			watch_dog_reset ();
			return (dsaplose (di, DA_PCDL, NULLCP, "QSP BIND INDICATION"));
		}
		break;

	case DS_CTX_INTERNET_DSP:
		if ((ds->ds_pctx_id = check_isp_ctxlist (&(acs->acs_start.ps_ctxlist))) == NOTOK) {
			LLOG(log_dsap,LLOG_EXCEPTIONS, ("Unacceptable Abstract Syntaxes for Internet DSP"));
			watch_dog ("RoBindReject (ISP)");
			RoBindReject (acs, ACS_TRANSIENT, ACS_USER_NOREASON, rni);
			watch_dog_reset ();
			return (dsaplose (di, DA_PCDL, NULLCP, "ISP BIND INDICATION"));
		}
		break;

	case NOTOK:
	default:
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("Unrecognizable abstract syntax."));
		watch_dog ("RoBindReject (default)");
		RoBindReject(acs, ACS_TRANSIENT, ACS_CONTEXT, rni);
		watch_dog_reset ();
		return (dsaplose (di, DA_APP_CONTEXT, NULLCP, "BIND INDICATION"));
	}

	/*
	* Most applications would dispatch on the context established,
	* but since all directory protocols have the same Bind Argument
	* we just fall through...
	*/

	/* Decode bind argument */
	if ((acs->acs_ninfo != 1) || (acs->acs_info[0] == NULLPE)) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("Invalid ACSE user data."));
		watch_dog ("RoBindReject (ninfo)");
		RoBindReject (acs, ACS_TRANSIENT, ACS_USER_NOREASON, rni);
		watch_dog_reset ();
		return (dsaplose (di, DA_ARG_DEC, NULLCP, "BIND INDICATION"));
	}

	if (decode_DAS_DirectoryBindArgument (acs->acs_info[0],
										  1, NULLCP, NULLIP, &bind_arg) != OK) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("Failed to decode directory bind argument."));
		watch_dog ("RoBindReject (decode)");
		RoBindReject (acs, ACS_TRANSIENT, ACS_USER_NOREASON, rni);
		watch_dog_reset ();
		return (dsaplose (di, DA_ARG_DEC, NULLCP, "BIND INDICATION"));
	}

	ds->ds_bind_arg = *bind_arg; /* struct copy */
	free ( (char *) bind_arg);
	DLOG(log_dsap,LLOG_DEBUG, ("Bind Argument decoded"));

	return (result);
}

/* ARGSUSED */

/*    D-BIND.RESULT */

int	  DBindResult (sd, context, respondtitle,
				   respondaddr, ctxlist, defctxresult, prequirements,
				   srequirements, isn, settings, ref, bind_res, pctx_id, di)
int			  sd;
OID			  context;
AEI			  respondtitle;
struct PSAPaddr		* respondaddr;
struct PSAPctxlist	* ctxlist;
int			  defctxresult;
int			  prequirements;
int			  srequirements;
long			  isn;
int			  settings;
struct SSAPref		* ref;
struct ds_bind_arg	* bind_res;
int			  pctx_id;
struct DSAPindication	* di;
{
	int			  result;
	PE			  bindrespe;
	struct RoNOTindication	  rni_s;
	struct RoNOTindication	* rni = &(rni_s);

	if (encode_DAS_DirectoryBindResult (&(bindrespe), 1, NULLIP, NULLCP,
										bind_res) != OK) {
		/* RoBindReject ?? */
		return (dsaplose (di, DA_RES_ENC, NULLCP, "BIND RESULT"));
	}
	bindrespe->pe_context = pctx_id;

	watch_dog ("RoBindResult");
	result = RoBindResult (sd, context, respondtitle,
						   respondaddr, ctxlist, defctxresult, prequirements,
						   srequirements, isn, settings, ref, bindrespe, rni);
	watch_dog_reset();

	if (bindrespe != NULLPE) {
		pe_free (bindrespe);
	}

	if (result == NOTOK) {
		/* Have an RoNOTindication, need to return DSAPindication */
		return (ronot2dsaplose (di, "RO-BIND.RESULT", rni));
	}

	return (result);
}

/* ARGSUSED */

/*    D-BIND.ERROR */

int	  DBindError (sd, context, respondtitle, respondaddr, ctxlist,
				  defctxresult, prequirements, srequirements, isn, settings,
				  ref, bind_err, pctx_id, di)
int			  sd;
OID			  context;
AEI			  respondtitle;
struct PSAPaddr		* respondaddr;
struct PSAPctxlist	* ctxlist;
int			  defctxresult;
int			  prequirements;
int			  srequirements;
long			  isn;
int			  settings;
struct SSAPref		* ref;
struct ds_bind_error	* bind_err;
int			  pctx_id;
struct DSAPindication	* di;
{
	int			  result;
	PE			  binderrpe;
	struct RoNOTindication	  rni_s;
	struct RoNOTindication	* rni = &(rni_s);

	if (encode_DAS_DirectoryBindError (&(binderrpe), 1, NULLIP, NULLCP,
									   bind_err) != OK) {
		/* RoBindReject ?? */
		return (dsaplose (di, DA_ERR_ENC, NULLCP, "BIND ERROR"));
	}
	binderrpe->pe_context = pctx_id;

	watch_dog ("RoBindError");
	result = RoBindError (sd, context,
						  respondtitle, respondaddr, ctxlist, defctxresult,
						  prequirements, srequirements, isn, settings, ref,
						  binderrpe, rni);
	watch_dog_reset();

	if (binderrpe != NULLPE) {
		pe_free (binderrpe);
	}

	if (result == NOTOK) {
		return (ronot2dsaplose (di, "RO-BIND.ERROR", rni));
	}

	return (result);
}

/* ARGSUSED */

/*    D-BIND.REJECT */

int
DBindReject (struct DSAPstart *ds, int status, int reason, struct DSAPindication *di) {
	int			  result;
	struct AcSAPstart	* acs = &(ds->ds_start);
	struct RoNOTindication	  rni_s;
	struct RoNOTindication	* rni = &(rni_s);

	watch_dog ("RoBindReject");
	result = RoBindReject (acs, status, reason, rni);
	watch_dog_reset();

	if (result == NOTOK) {
		return (ronot2dsaplose (di, "RO-BIND.ERROR", rni));
	}

	return (result);
}

