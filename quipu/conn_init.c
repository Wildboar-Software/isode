/* conn_init.c - deal with incoming association requests */

#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/quipu/RCS/conn_init.c,v 9.0 1992/06/16 12:34:01 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/quipu/RCS/conn_init.c,v 9.0 1992/06/16 12:34:01 isode Rel $
 *
 *
 * $Log: conn_init.c,v $
 * Revision 9.0  1992/06/16  12:34:01  isode
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


#include "quipu/dsap.h"
#include "tsap.h"
#include "quipu/util.h"
#include "quipu/connection.h"
#include "quipu/bind.h"

extern LLog * log_dsap;
#ifndef NO_STATS
extern LLog * log_stat;
#endif
extern  struct PSAPaddr		* dsaladdr;

void	  ds_log();

struct connection	* conn_alloc();

#ifdef QUIPU_CONSOLE
extern  void opening_analyse() ;
#endif /* QUIPU_CONSOLE */

extern  time_t timenow;
extern  char   quipu_shutdown;

int
conn_init (struct connection *cn) {
	int				  result, ds_bind_return ;
	char			**vec;
	struct DSAPstart		* ds;
	struct DSAPindication	  di_s;
	struct DSAPindication	* di = &di_s;
	struct DSAPabort		* da = &(di->di_abort);
	struct TSAPdisconnect	  td_s;
	struct TSAPdisconnect	* td = &td_s;

	DLOG (log_dsap,LLOG_TRACE,( "conn_init()"));

	vec = cn->cn_start.cs_vec;
	ds = &(cn->cn_start.cs_ds);

	result = DBindInit (cn->cn_start.cs_vecp, vec, ds, di);

	cn->cn_ad = ds->ds_sd;
	cn->cn_initiator = FALSE;
	cn->cn_ctx = ds->ds_ctx;
	cn->cn_dn = NULLDN;

	if (result != OK) {
		ds_log(da, "Incoming connection failed",cn->cn_ad);
		conn_extract (cn);
		return;
	}

	if (quipu_shutdown) {
		DBindReject (ds, ACS_PERMANENT, ACS_USER_NOREASON, di);
		conn_extract (cn);
		return;
	}

	if (conns_used >= MAX_CONNS) {
		LLOG (log_dsap,LLOG_EXCEPTIONS,( "Too many connections"));
		cn->cn_start.cs_err.dbe_version = DBA_VERSION_V1988;
		cn->cn_start.cs_err.dbe_type = DBE_TYPE_SERVICE;
		cn->cn_start.cs_err.dbe_version = DSE_SV_BUSY;
		if (TSetQueuesOK (cn->cn_ad, 1, td) == NOTOK)
			td_log (td, "TSetQueuesOK (incoming - reject)");

		conn_init_err (cn);
		return;
	}

	if ( (ds->ds_start.acs_start.ps_called.pa_selectlen !=
			dsaladdr->pa_selectlen) ||
			(dsaladdr->pa_selectlen && bcmp (
				 ds->ds_start.acs_start.ps_called.pa_selector,
				 dsaladdr->pa_selector,
				 dsaladdr->pa_selectlen) == 0)) {

out:
		;
		LLOG (log_dsap,LLOG_EXCEPTIONS, ("Bad Selector (%d): %s",
										 ds->ds_sd, paddr2str(&(ds->ds_start.acs_start.ps_calling),NULLNA)));

		DBindReject (ds, ACS_PERMANENT, ACS_USER_NOREASON, di);
		conn_extract (cn);
		return;
	}
	if ( (ds->ds_start.acs_start.ps_called.pa_addr.sa_selectlen !=
			dsaladdr->pa_addr.sa_selectlen) ||
			(dsaladdr->pa_addr.sa_selectlen && bcmp (
				 ds->ds_start.acs_start.ps_called.pa_addr.sa_selector,
				 dsaladdr->pa_addr.sa_selector,
				 dsaladdr->pa_addr.sa_selectlen) == 0))
		goto out;

	/* tsels are checked in the lower layers - just check the NULL ones */
	if ( (dsaladdr->pa_addr.sa_addr.ta_selectlen == 0) &&
			( ds->ds_start.acs_start.ps_called.pa_addr.sa_addr.ta_selectlen != 0))
		goto out;

	/*
	* Log the arrival of a connection request.
	*/
	DLOG (log_dsap,LLOG_TRACE, ("Context: %s; Caller: %s; Callee: %s.",
								oid2ode(ds->ds_start.acs_context),
								sprintaei (&ds->ds_start.acs_callingtitle),
								sprintaei (&ds->ds_start.acs_calledtitle)));

#ifndef NO_STATS
	switch (cn->cn_ctx) {
	case DS_CTX_X500_DAP:
		LLOG (log_stat, LLOG_NOTICE, ("X500 DAP context association (%d): %s", cn->cn_ad, pa2str (&(ds->ds_start.acs_start.ps_calling))));
		break;
	case DS_CTX_X500_DSP:
		LLOG (log_stat, LLOG_NOTICE, ("X500 DSP context association (%d): %s", cn->cn_ad, pa2str (&(ds->ds_start.acs_start.ps_calling))));
		break;
	case DS_CTX_QUIPU_DSP:
		LLOG (log_stat, LLOG_NOTICE, ("QUIPU DSP context association (%d): %s", cn->cn_ad, pa2str (&(ds->ds_start.acs_start.ps_calling))));
		break;
	case DS_CTX_INTERNET_DSP:
		LLOG (log_stat, LLOG_NOTICE, ("Internet DSP context association (%d): %s", cn->cn_ad, pa2str (&(ds->ds_start.acs_start.ps_calling))));
		break;
	default :
		LLOG (log_stat, LLOG_EXCEPTIONS, ("UNKNOWN context association (%d): %s", cn->cn_ad, pa2str (&(ds->ds_start.acs_start.ps_calling))));
		break;
	}
#endif

	cn->cn_dn = dn_cpy(ds->ds_bind_arg.dba_dn);

	/*
	*  If we haven't returned yet then the protocol for binding has been
	*  satisfactorily completed.
	*  Now attempt to perform the ds_bind for the argument decoded, which
	*  can either succeed, fail or suspend waiting for a remote compare.
	*  If a remote compare has been scheduled then return, otherwise
	*  complete the connection initialisation by sending a bind result.
	*/

	if (TSetQueuesOK (cn->cn_ad, 1, td) == NOTOK)
		td_log (td, "TSetQueuesOK (incoming)");

	ds_bind_return = ds_bind_init(cn);

#ifdef QUIPU_CONSOLE
	opening_analyse(cn);
#endif

	switch(ds_bind_return) {
	case DS_OK:
		conn_init_res(cn);
		break;
	case DS_ERROR_CONNECT:
		conn_init_err(cn);
		break;
	case DS_CONTINUE:
		cn->cn_state = CN_INDICATED;
		break;
	default:
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("Unexpected return by ds_bind_init"));
		DBindReject (ds, ACS_TRANSIENT, ACS_USER_NOREASON, di);
		break;
	}
}

int
conn_init_res (struct connection *cn) {
	int				  result;
	struct DSAPindication      di_s;
	struct DSAPindication      *di = &(di_s);
	struct DSAPabort           *da = &(di->di_abort);
	struct DSAPstart           * ds = &(cn->cn_start.cs_ds);
	struct AcSAPstart           * acs = &(ds->ds_start);
	struct PSAPstart            * ps = &(acs->acs_start);

	DLOG(log_dsap,LLOG_TRACE, ("conn_init_res(%d)",cn->cn_ad));

#ifdef DEBUG
	{
		int	  i;

		for(i=0; i<ps->ps_ctxlist.pc_nctx; i++) {
			DLOG(log_dsap, LLOG_DEBUG, ("ctx[%d] id = %d, res = %d.", i,
										ps->ps_ctxlist.pc_ctx[i].pc_id,
										ps->ps_ctxlist.pc_ctx[i].pc_result));
		}
	}
#endif

	result = DBindResult (cn->cn_ad, acs->acs_context, NULLAEI, NULLPA,
						  &(ps->ps_ctxlist), ps->ps_defctxresult, PR_MYREQUIRE,
						  ps->ps_srequirements & (ROS_MYREQUIRE | SR_NEGOTIATED),
						  SERIAL_NONE, ps->ps_settings, &(ps->ps_connect),
						  &(cn->cn_start.cs_res), ds->ds_pctx_id, di);

	if (result == OK) {
		cn->cn_state = CN_OPEN;
	} else {
		ds_log(da, "D-BIND.RESULT",cn->cn_ad);
		conn_extract(cn);
	}

	ACSFREE (acs);
}

int
conn_init_err (struct connection *cn) {
	int				  result;
	struct DSAPindication      di_s;
	struct DSAPindication      *di = &di_s;
	struct DSAPabort           *da = &di->di_abort;
	struct DSAPstart           * ds = &(cn->cn_start.cs_ds);
	struct AcSAPstart           * acs = &(ds->ds_start);
	struct PSAPstart            * ps = &(acs->acs_start);

	DLOG(log_dsap,LLOG_TRACE, ("conn_init_err(%d)",cn->cn_ad));

#ifdef DEBUG
	{
		int	  i;

		for(i=0; i<ps->ps_ctxlist.pc_nctx; i++) {
			DLOG(log_dsap, LLOG_DEBUG, ("ctx[%d] id = %d, res = %d.", i,
										ps->ps_ctxlist.pc_ctx[i].pc_id,
										ps->ps_ctxlist.pc_ctx[i].pc_result));
		}
	}
#endif

	result = DBindError (cn->cn_ad, acs->acs_context, NULLAEI, NULLPA,
						 &(ps->ps_ctxlist), ps->ps_defctxresult, PR_MYREQUIRE,
						 ps->ps_srequirements & (ROS_MYREQUIRE | SR_NEGOTIATED),
						 SERIAL_NONE, ps->ps_settings, &(ps->ps_connect),
						 &(cn->cn_start.cs_err), ds->ds_pctx_id, di);

	if (result == OK) {
		DLOG(log_dsap,LLOG_TRACE, ("status != ACS_ACCEPT"));
		conn_extract(cn);
	} else {
		ds_log(da, "D-BIND.ERROR",cn->cn_ad);
		conn_extract(cn);
	}

}

int
conn_pre_init (int newfd, int vecp, char **vec) {
	struct connection	* cn = NULLCONN;

	for (cn = connlist; cn != NULLCONN; cn = cn->cn_next)
		if (newfd == cn->cn_ad)
			break;

	if (cn == NULLCONN) {
		cn = conn_alloc();

		cn->cn_next = connlist;
		connlist = cn;
		conns_used++;

		cn->cn_ad = newfd;
		cn->cn_initiator = FALSE;
	}

	cn->cn_start.cs_vecp = vecp;
	if (vec[0])
		cn->cn_start.cs_svec[0] = cn->cn_start.cs_vec[0] = strdup (vec[0]);
	if (vec[1])
		cn->cn_start.cs_svec[1] = cn->cn_start.cs_vec[1] = strdup (vec[1]);
	if (vec[2])
		cn->cn_start.cs_svec[2] = cn->cn_start.cs_vec[2] = strdup (vec[2]);
	if (vec[3])
		cn->cn_start.cs_svec[3] = cn->cn_start.cs_vec[3] = strdup (vec[3]);

	cn->cn_state = CN_OPENING;

	cn->cn_last_used = timenow;

	if (newfd == NOTOK)
		conn_init (cn);
	else
		DLOG (log_dsap,LLOG_NOTICE, ("opening association on %d",newfd ));
}

int
warn_conn_init (int newfd) {
	/* An association will come soon... */
	struct connection	* cn = NULLCONN;

	for (cn = connlist; cn != NULLCONN; cn = cn->cn_next)
		if (newfd == cn->cn_ad)
			break;

	if (cn != NULLCONN) {

		if ( cn->cn_state != CN_PRE_OPENING ) {

			LLOG (log_dsap,LLOG_EXCEPTIONS, ("Help needed on %d !",newfd ));
			net_send_abort (cn);	/* what else ? */
		} else {

			cn->cn_last_used = timenow;
			DLOG (log_dsap,LLOG_NOTICE,
				  ("second about to open association on %d",newfd ));
		}

	} else {

		cn = conn_alloc();

		cn->cn_next = connlist;
		connlist = cn;
		conns_used++;

		cn->cn_ad = newfd;
		cn->cn_initiator = FALSE;

		cn->cn_state = CN_PRE_OPENING;

		cn->cn_last_used = timenow;

		DLOG (log_dsap,LLOG_NOTICE,
			  ("about to open association on %d",newfd ));
	}

}
