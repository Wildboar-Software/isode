/* text2spkt.c - read/write a SPDU thru a debug filter */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/ssap/RCS/text2spkt.c,v 9.0 1992/06/16 12:39:41 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/ssap/RCS/text2spkt.c,v 9.0 1992/06/16 12:39:41 isode Rel $
 *
 *
 * $Log: text2spkt.c,v $
 * Revision 9.0  1992/06/16  12:39:41  isode
 * Release 8.0
 *
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */


/* LINTLIBRARY */

#include <stdio.h>
#include "spkt.h"
#include "logger.h"


static	type_id ();
static	type_ssn ();
static	type_bits (LLog *, char *, char *, u_char,  int,  char *);
static	type_settings (LLog *, char *, u_char);
static	type_tsdu (LLog *, char *, u_short,  u_short);
static	type_ref ();
static	type_vrsn (LLog *, char *, u_char);
static	type_reason ();
static	type_prepare (LLog *, char *, u_char);
static	type_error (LLog *, char *, u_char);
static	type_resync (LLog *, char *, u_char);
static	type_data ();
static	type_info ();

#define	sprintc(v,b)	sprintb ((int) (v), (b))

/*  */

#define	SPDU_TYPE(e)	ll_printf (lp, "%sCODE/ %s\n", rw, e)

#define	DMASK	"\020\01RELEASE\02USER\03PROTOCOL\04UNKNOWN"
#define	EMASK	"\020\01BEGIN\02END"
#define	OMASK	"\020\01EXTD"
#define	RMASK \
    "\020\01HALFDUPLEX\02DUPLEX\03EXPEDITED\04MINORSYNC\05MAJORSYNC\06RESYNC\07ACTIVITY\010NEGOTIATED\011CAPABILITY\012EXCEPTIONS\013TYPEDATA"
#define	SMASK	"\020\01NOEND"
#define	TMASK	"\020\01DATA\03SYNC\05ACTIVITY\07RELEASE"
#define	YMASK	"\020\01NOEXPLICIT"



void
spkt2text (LLog *lp, struct ssapkt *s, int read) {
	char   *rw = read ? "<--- " : "---> ";

	LLOG (lp, LLOG_ALL,  ("dump of SPDU 0x%x, errno=0x%x mask=0x%x%s",   s, s -> s_errno, s -> s_mask, s -> s_mask & SMASK_SPDU_EXPD ? " (expedited)" : ""));
	ll_printf (lp, "%s(\n", rw);

	ll_printf (lp, "%sLI/ %d\n", rw, s -> s_li);

	switch (s -> s_code) {
	case SPDU_CN:
	case SPDU_AC:
		SPDU_TYPE (s -> s_code == SPDU_CN ? "CONNECT" : "ACCEPT");
		if (s -> s_mask & SMASK_CN_REF)
			type_ref (lp, rw, &s -> s_cn_reference);
		if (s -> s_mask & SMASK_CN_OPT)
			type_bits (lp, rw, "OPTIONS", s -> s_options, CR_OPT_MASK,
					   OMASK);
		if (s -> s_mask & SMASK_CN_TSDU)
			type_tsdu (lp, rw, s -> s_tsdu_init, s -> s_tsdu_resp);
		if (s -> s_mask & SMASK_CN_VRSN)
			type_vrsn (lp, rw, s -> s_cn_version);
		if (s -> s_mask & SMASK_CN_ISN)
			type_ssn (lp, rw, "ISN", s -> s_isn);
		if (s -> s_mask & SMASK_CN_SET)
			type_settings (lp, rw, s -> s_settings);
		if (s -> s_code == SPDU_AC && (s -> s_mask & SMASK_AC_TOKEN))
			type_bits (lp, rw, "TOKENS", s -> s_ac_token, -1, TMASK);
		if (s -> s_mask & SMASK_CN_REQ)
			type_bits (lp, rw, "REQUIREMENTS", s -> s_cn_require,
					   -1, RMASK);
		if (s -> s_mask & SMASK_CN_CALLING)
			type_id (lp, "CALLING", rw, s -> s_calling, s -> s_callinglen);
		if (s -> s_mask & SMASK_CN_CALLED)
			type_id (lp, "CALLED", rw, s -> s_called, s -> s_calledlen);
		break;

	case SPDU_RF:
		SPDU_TYPE ("REFUSE");
		if (s -> s_mask & SMASK_RF_REF)
			type_ref (lp, rw, &s -> s_rf_reference);
		if (s -> s_mask & SMASK_RF_DISC)
			type_bits (lp, rw, "DISCONNECT", s -> s_rf_disconnect,
					   RF_DISC_MASK, DMASK);
		if (s -> s_mask & SMASK_RF_REQ)
			type_bits (lp, rw, "REQUIREMENTS", s -> s_rf_require,
					   -1, RMASK);
		if (s -> s_mask & SMASK_RF_VRSN)
			type_vrsn (lp, rw, s -> s_rf_version);
		if (s -> s_rlen > 0) {
			type_reason (lp, rw, *s -> s_rdata & 0xff);
			if (s -> s_rlen > 1)
				type_data (lp, "REASON", rw, s -> s_rlen - 1,
						   s -> s_rdata + 1);
		}
		break;

	case SPDU_FN:
		SPDU_TYPE ("FINISH");
		if (s -> s_mask & SMASK_FN_DISC)
			type_bits (lp, rw, "DISCONNECT", s -> s_fn_disconnect,
					   FN_DISC_MASK, DMASK);
		break;

	case SPDU_DN:
		SPDU_TYPE ("DISCONNECT");
		break;

	case SPDU_NF:
		SPDU_TYPE ("NOT FINISHED");
		break;

	case SPDU_AB:
#ifdef	notdef
	case SPDU_AI: 		/* aka SPDU_AB */
#endif
		if (s -> s_mask & SMASK_SPDU_AB) {
			SPDU_TYPE ("ABORT");
			if (s -> s_mask & SMASK_AB_DISC)
				type_bits (lp, rw, "DISCONNECT", s -> s_ab_disconnect,
						   AB_DISC_MASK, DMASK);
			if (s -> s_mask & SMASK_AB_REFL)
				type_data (lp, "REFLECT", rw, sizeof s -> s_reflect,
						   (char *) s -> s_reflect);
			break;
		}
		SPDU_TYPE ("ACTIVITY INTERRUPT");
		if (s -> s_mask & SMASK_AI_REASON)
			type_error (lp, rw, s -> s_ai_reason);
		break;

	case SPDU_AA:
#ifdef	notdef
	case SPDU_AIA: 		/* aka SPDU_AA */
#endif
		if (s -> s_mask & SMASK_SPDU_AA)
			SPDU_TYPE ("ABORT ACCEPT");
		else
			SPDU_TYPE ("ACTIVITY INTERRUPT ACK");
		break;

	case SPDU_GT:
#ifdef	notdef
	case SPDU_DT: 		/* aka SPDU_GT */
#endif
		if (s -> s_mask & SMASK_SPDU_GT) {
			SPDU_TYPE ("GIVE TOKENS");
			if (s -> s_mask & SMASK_GT_TOKEN)
				type_bits (lp, rw, "TOKENS", s -> s_gt_token, -1, TMASK);
		} else
			SPDU_TYPE ("DATA TRANSFER");
		break;

	case SPDU_EX:
		SPDU_TYPE ("EXPEDITED");
		break;

	case SPDU_TD:
		SPDU_TYPE ("TYPED DATA");
		break;

	case SPDU_CD:
		SPDU_TYPE ("CAPABILITY DATA");
		break;

	case SPDU_CDA:
		SPDU_TYPE ("CAPABILITY DATA ACK");
		break;

	case SPDU_PT:
		SPDU_TYPE ("PLEASE TOKENS");
		if (s -> s_mask & SMASK_PT_TOKEN)
			type_bits (lp, rw, "TOKENS", s -> s_pt_token, -1, TMASK);
		break;

	case SPDU_GTC:
		SPDU_TYPE ("GIVE TOKENS CONFIRM");
		break;

	case SPDU_GTA:
		SPDU_TYPE ("GIVE TOKENS ACK");
		break;

	case SPDU_MIP:
		SPDU_TYPE ("MINOR SYNCHRONIZATION POINT");
		if (s -> s_mask & SMASK_MIP_SYNC)
			type_bits (lp, rw, "SYNC", s -> s_mip_sync, MIP_SYNC_MASK,
					   YMASK);
		if (s -> s_mask & SMASK_MIP_SERIAL)
			type_ssn (lp, rw, "SSN", s -> s_mip_serial);
		break;

	case SPDU_MIA:
		SPDU_TYPE ("MINOR SYNC ACK");
		if (s -> s_mask & SMASK_MIA_SERIAL)
			type_ssn (lp, rw, "SSN", s -> s_mia_serial);
		break;

	case SPDU_MAP:
#ifdef	notdef
	case SPDU_AE: 		/* aka SPDU_MAP */
#endif
		if ((s -> s_mask & SMASK_MAP_SYNC) && (s -> s_map_sync & MAP_SYNC_NOEND)) {
			SPDU_TYPE ("MAJOR SYNCHRONIZATION POINT");
			type_bits (lp, rw, "SYNC", s -> s_map_sync, MAP_SYNC_MASK, SMASK);
		} else
			SPDU_TYPE ("ACTIVITY END");
		if (s -> s_mask & SMASK_MAP_SERIAL)
			type_ssn (lp, rw, "SSN", s -> s_map_serial);
		break;

	case SPDU_MAA:
#ifdef	notdef
	case SPDU_AEA: 		/* aka SPDU_MAA */
#endif
		SPDU_TYPE ("MAJOR SYNC/ACTIVITY END ACK");
		if (s -> s_mask & SMASK_MAA_SERIAL)
			type_ssn (lp, rw, "SSN", s -> s_maa_serial);
		break;

	case SPDU_RS:
		SPDU_TYPE ("RESYNCHRONIZE");
		if (s -> s_mask & SMASK_RS_SET)
			type_settings (lp, rw, s -> s_rs_settings);
		if (s -> s_mask & SMASK_RS_TYPE)
			type_resync (lp, rw, s -> s_rs_type);
		if (s -> s_mask & SMASK_RS_SSN)
			type_ssn (lp, rw, "RSN", s -> s_rs_serial);
		break;

	case SPDU_RA:
		SPDU_TYPE ("RESYNCHRONIZE ACK");
		if (s -> s_mask & SMASK_RA_SET)
			type_settings (lp, rw, s -> s_ra_settings);
		if (s -> s_mask & SMASK_RA_SSN)
			type_ssn (lp, rw, "RSN", s -> s_ra_serial);
		break;

	case SPDU_PR:
		SPDU_TYPE ("PREPARE");
		type_prepare (lp, rw, s -> s_pr_type);
		break;

	case SPDU_ER:
		SPDU_TYPE ("EXCEPTION REPORT");
		break;

	case SPDU_ED:
		SPDU_TYPE ("EXCEPTION DATA");
		if (s -> s_mask & SMASK_ED_REASON)
			type_error (lp, rw, s -> s_ed_reason);
		break;

	case SPDU_AS:
		SPDU_TYPE ("ACTIVITY START");
		if (s -> s_mask & SMASK_AS_ID) {
			ll_printf (lp, "%s", rw);
			type_info (lp, "ID/ %d", (int) s -> s_as_id.sd_len,
					   s -> s_as_id.sd_data);
			ll_printf (lp, "\n");
		}
		break;

	case SPDU_AR:
		SPDU_TYPE ("ACTIVITY RESUME");
		if (s -> s_mask & SMASK_AR_REF)
			type_ref (lp, rw, &s -> s_ar_reference);
		if (s -> s_mask & SMASK_AR_OID) {
			ll_printf (lp, "%s", rw);
			type_info (lp, "OLD ID/ %d", (int) s -> s_ar_oid.sd_len,
					   s -> s_ar_oid.sd_data);
			ll_printf (lp, "\n");
		}
		if (s -> s_mask & SMASK_AR_SSN)
			type_ssn (lp, rw, "SSN", s -> s_ar_serial);
		if (s -> s_mask & SMASK_AR_ID) {
			ll_printf (lp, "%s", rw);
			type_info (lp, "ID/ %d", (int) s -> s_ar_id.sd_len,
					   s -> s_ar_id.sd_data);
			ll_printf (lp, "\n");
		}
		break;

	case SPDU_AD:
		SPDU_TYPE ("ACTIVITY DISCARD");
		if (s -> s_mask & SMASK_AD_REASON)
			type_error (lp, rw, s -> s_ad_reason);
		break;

	case SPDU_ADA:
		SPDU_TYPE ("ACTIVITY DISCARD ACK");
		break;

	default:
		ll_printf (lp, "%sCODE/ 0x%x\n", rw, s -> s_code);
		break;
	}

	if (s -> s_mask & SMASK_ENCLOSE)
		type_bits (lp, rw, "ENCLOSURE", s -> s_enclose, ENCL_MASK, EMASK);

	if (s -> s_udata)
		if (s -> s_code == SPDU_ER)
			type_data (lp, "REFLECT", rw, s -> s_ulen, s -> s_udata);
		else if (s -> s_mask & SMASK_UDATA_PGI)
			type_data (lp, "USER", rw, s -> s_ulen, s -> s_udata);
		else {
			ll_printf (lp, "%sUSER INFO/ ", rw);
			type_info (lp, "%d", s -> s_ulen, s -> s_udata);
			ll_printf (lp, "\n");
		}
	ll_printf (lp, "%s)\n", rw);

	ll_sync (lp);
}

/*  */

static
type_id (LLog *lp, char *type, char *rw, char *selector, int len) {
	char    buffer[BUFSIZ];

	buffer[explode (buffer, (u_char *) selector, len)] = NULL;

	ll_printf (lp, "%s%s/ %d/\"%s\"\n", rw, type, len, buffer);
}


static
type_ssn (LLog *lp, char *rw, char *what, u_long ssn) {
	ll_printf (lp, "%s%s/ %d\n", rw, what, ssn);
}


static
type_bits (LLog *lp, char *rw, char *s, u_char bits, int mask, char *t) {
	ll_printf (lp, "%s%s/ %s", rw, s, sprintc (bits & mask, t));
	if (bits & ~mask)
		ll_printf (lp, ": illegal use of %s", sprintc (bits & ~mask, t));
	ll_printf (lp, "\n");
}


#define dotoken(requires,shift,bit,type) \
{ \
    token = (settings >> shift) & ST_MASK; \
    ll_printf (lp, " %s:%s", type, token == ST_INIT_VALUE ? "initiator" \
	: token == ST_RESP_VALUE ? "responder" \
	: token == ST_CALL_VALUE ? "choice" \
	: "reserved"); \
}

static
type_settings (LLog *lp, char *rw, u_char settings) {
	int     token;

	ll_printf (lp, "%sSETTINGS/", rw);
	dotokens ();
	ll_printf (lp, "\n");
}

#undef	dotoken


static
type_tsdu (LLog *lp, char *rw, u_short init, u_short resp) {
	ll_printf (lp, "%sTSDU/ INITIATOR: %d, RESPONDER: %d\n",
			   rw, init, resp);
}


static
type_ref (LLog *lp, char *rw, struct SSAPref *ref) {
	ll_printf (lp, "%sREFERENCE/", rw);
	if (ref -> sr_vlen)
		type_info (lp, "<CALLING %d", (int) ref -> sr_calling_len,
				   ref -> sr_calling);
	else
		type_info (lp, " <USER %d", (int) ref -> sr_ulen, ref -> sr_udata);
	type_info (lp, ", COMMON %d", (int) ref -> sr_clen, ref -> sr_cdata);
	type_info (lp, ", ADDITIONAL %d", (int) ref -> sr_alen, ref -> sr_adata);
	if (ref -> sr_vlen)
		type_info (lp, ", CALLED %d", (int) ref -> sr_called_len,
				   ref -> sr_called);
	ll_printf (lp, ">\n");
}


static
type_vrsn (LLog *lp, char *rw, u_char version) {
	ll_printf (lp, "%sVERSION/ 0x%x\n", rw, version);
}


static
type_reason (LLog *lp, char *rw, int reason) {
	ll_printf (lp, "%sREASON/ 0x%x: %s\n", rw, reason,
			   SErrString ((int) reason));
}


static
type_prepare (LLog *lp, char *rw, u_char type) {
	ll_printf (lp, "%sTYPE/ ", rw);
	switch (type) {
	case PR_MAA:
		ll_printf (lp, "MAA");
		break;
	case PR_RS:
		ll_printf (lp, "RS");
		break;
	case PR_RA:
		ll_printf (lp, "RA");
		break;
	case PR_AB:
		ll_printf (lp, "AB");
		break;
	default:
		ll_printf (lp, "%d: illegal value", type);
		break;
	}
	ll_printf (lp, "\n");
}


static
type_error (LLog *lp, char *rw, u_char reason) {
	ll_printf (lp, "%sREASON/ ", rw);
	switch (reason) {
	case SP_NOREASON:
		ll_printf (lp, "No specific reason stated");
		break;
	case SP_JEOPARDY:
		ll_printf (lp, "User receiving ability jeopardized");
		break;
	case SP_SEQUENCE:
		ll_printf (lp, "User sequence error");
		break;
	case SP_LOCAL:
		ll_printf (lp, "Local SS-user error");
		break;
	case SP_PROCEDURAL:
		ll_printf (lp, "Unrecoverable procedural error");
		break;
	case SP_DEMAND:
		ll_printf (lp, "Demand data token");
		break;
	default:
		ll_printf (lp, "%d: illegal value", reason);
		break;
	}
	ll_printf (lp, "\n");
}


static
type_resync (LLog *lp, char *rw, u_char type) {
	ll_printf (lp, "%sTYPE/ ", rw);
	switch (type) {
	case SYNC_RESTART:
		ll_printf (lp, "restart");
		break;
	case SYNC_ABANDON:
		ll_printf (lp, "abandon");
		break;
	case SYNC_SET:
		ll_printf (lp, "set");
		break;
	default:
		ll_printf (lp, "%d: illegal value", type);
		break;
	}
	ll_printf (lp, "\n");
}


static
type_data (LLog *lp, char *type, char *rw, int len, char *data) {
	ll_printf (lp, "%s%s DATA/ ", rw, type);
	type_info (lp, "%d", len, data);
	ll_printf (lp, "\n");
}


static
type_info (LLog *lp, char *fmt, int len, char *data) {
	char    buffer[BUFSIZ];

	ll_printf (lp, fmt, len);
	if (0 < len && len < sizeof buffer / 2) {
		buffer[explode (buffer, (u_char *) data, len)] = NULL;
		ll_printf (lp, " %s", buffer);
	}
}

/*  */

/* ARGSUSED */

void
text2spkt (struct ssapkt *s) {
	/* NOT YET IMPLEMENTED */
}
