/* ssaprespond.c - SPM: responder */

#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "spkt.h"
#include "tailor.h"
static int refuse (struct ssapblk *sb, struct ssapkt *s, struct SSAPindication *si);


/* S-CONNECT.INDICATION */

static int  refuse (struct ssapblk *sb, struct ssapkt *s, struct SSAPindication *si);

int SInit (const int vecp, char **vec, struct SSAPstart *ss, struct SSAPindication *si) {
	const int	    len;
	struct ssapblk *sb;
	struct ssapkt *s;
	struct TSAPstart tss;
	struct TSAPstart *ts = &tss;
	struct TSAPdisconnect tds;
	struct TSAPdisconnect *td = &tds;

	isodetailor (NULLCP, 0);
	if (vecp < 2)
		return ssaplose (si, SC_PARAMETER, NULLCP, "bad initialization vector");
	missingP (vec);
	missingP (ss);
	missingP (si);
	if ((sb = newsblk ()) == NULL)
		return ssaplose (si, SC_CONGEST, NULLCP, "out of memory");
	if (vecp == 2 || TInit (vecp, vec, ts, td) != NOTOK) {
		int	sd;
		struct TSAPdata txs;
		struct TSAPdata *tx = &txs;
		if (vecp == 2) {
			if (TRestoreState (vec[1], ts, td) == NOTOK) {
				ts2sslose (si, "TRestoreState", td);
				ssaplose (si, SC_PARAMETER, NULLCP, "bad initialization vector");
				goto out1;
			}
			bzero (vec[0], strlen (vec[0]));
			bzero (vec[1], strlen (vec[1]));
			*vec = NULL;
		} else {
			if (TConnResponse (ts -> ts_sd, &ts -> ts_called,  ts -> ts_expedited,  NULLCP, 0, NULLQOS, td) == NOTOK) {
				ts2sslose (si, "TConnResponse", td);
				TDiscRequest (ts -> ts_sd, NULLCP, 0, td);
				goto out1;
			}
		}
		sd = ts -> ts_sd;
		if (TReadRequest (sb -> sb_fd = sd, tx, NOTOK, td) == NOTOK) {
			ts2sslose (si, "TReadRequest", td);
			goto out1;
		}
		s = tsdu2spkt (&tx -> tx_qbuf, tx -> tx_cc, NULL);
		TXFREE (tx);
		if (s == NULL || s -> s_errno != SC_ACCEPT) {
			spktlose (sd, si, (s ? s -> s_errno : SC_CONGEST) | SC_REFUSE, NULLCP, NULLCP);
			goto out2;
		}
		if (s -> s_code != SPDU_CN) {
			spktlose (sd, si, (s ? s -> s_errno : SC_CONGEST) | SC_REFUSE, NULLCP, "session protocol mangled: expected 0x%x, got 0x%x", SPDU_CN, s -> s_code);
			goto out2;
		}
		if (s -> s_mask & SMASK_CN_VRSN	&& !(s -> s_cn_version & SB_ALLVRSNS)) {
			spktlose (sd, si, SC_VERSION | SC_REFUSE, NULLCP, "version mismatch: expecting something in 0x%x, got 0x%x",				 SB_ALLVRSNS, s -> s_cn_version);
			goto out2;
		}
		if ((s -> s_mask & SMASK_CN_REQ) &&
				(((s -> s_cn_require & SR_EXCEPTIONS) && !(s -> s_cn_require & SR_HALFDUPLEX))
				 ||
				 ((s -> s_cn_require & SR_CAPABILITY) && !(s -> s_cn_require & SR_ACTIVITY))
				 ||
				 (!(s -> s_cn_require & (SR_HALFDUPLEX | SR_DUPLEX)))
				)
		   ) {
			spktlose (sd, si, SC_PROTOCOL, NULLCP, "proposed session requirements error: got 0x%x", s -> s_cn_require);
			goto out2;
		}
		if ((s -> s_mask & SMASK_CN_REQ) &&
				((s -> s_cn_require & (SR_MINORSYNC | SR_MAJORSYNC | SR_RESYNC))
				 ? (!(s -> s_cn_require & SR_ACTIVITY)
					&& !(s -> s_mask & SMASK_CN_ISN))
				 : (s -> s_mask & SMASK_CN_ISN))) {
			spktlose (sd, si, SC_PROTOCOL, NULLCP,
					  "proposed session ISN error: %s, got FUs 0x%x",  (s -> s_mask & SMASK_CN_ISN) ? "present" : "absent", s -> s_cn_require);
			goto out2;
		}
	} else {
		int	reason;
		vec += vecp - 2;
		s = NULL;
		if ((reason = td -> td_reason) != DR_PARAMETER
				|| TRestoreState (vec[0], ts, td) == NOTOK
				|| (s = str2spkt (vec[1])) == NULL
				|| s -> s_errno != SC_ACCEPT) {
			if (s)
				freespkt (s);
			else
				ts2sslose (si, reason != DR_PARAMETER ? "TInit" : "TRestoreState", td);
			ssaplose (si, SC_PARAMETER, NULLCP, "bad initialization vector");
			goto out1;
		}
		bzero (vec[0], strlen (vec[0]));
		bzero (vec[1], strlen (vec[1]));
		*vec = NULL;
	}
	sb -> sb_fd = ts -> ts_sd;
	sb -> sb_version =
		(s -> s_mask & SMASK_CN_VRSN)
		? ((s -> s_cn_version & (1 << SB_VRSN2))
		   ? SB_VRSN2 : SB_VRSN1)
		: s -> s_ulen > SS_SIZE
		? SB_VRSN2 : SB_VRSN1;
	if (ts -> ts_expedited)
		sb -> sb_flags |= SB_EXPD;
	bzero ((char *) ss, sizeof *ss);
	ss -> ss_sd = sb -> sb_fd;
	if (s -> s_mask & SMASK_CN_REF)
		ss -> ss_connect = s -> s_cn_reference;	/* struct copy */
	if (s -> s_mask & SMASK_CN_OPT)
		sb -> sb_options = s -> s_options;
	if (s -> s_mask & SMASK_CN_ISN)
		ss -> ss_isn = sb -> sb_V_A = sb -> sb_V_M = s -> s_isn;
	else
		ss -> ss_isn = SERIAL_NONE;
	if (!(s -> s_mask & SMASK_CN_TSDU))
		s -> s_tsdu_init = s -> s_tsdu_resp = 0;
	{
		uint16_t tsz;

		if (int2u16 (GET_TSDU_SIZE (ts -> ts_tsdusize), &tsz) != 0)
			return ssaplose (si, SC_PROTOCOL, NULLCP, "TSDU size out of range");
		sb -> sb_tsdu_them = tsz;
		if (s -> s_tsdu_init < tsz)
			sb -> sb_tsdu_them = s -> s_tsdu_init;
		if (int2u16 (GET_TSDU_SIZE (ts -> ts_tsdusize), &tsz) != 0)
			return ssaplose (si, SC_PROTOCOL, NULLCP, "TSDU size out of range");
		sb -> sb_tsdu_us = tsz;
		if (s -> s_tsdu_resp < tsz)
			sb -> sb_tsdu_us = s -> s_tsdu_resp;
	}
	if (sb -> sb_version >= SB_VRSN2)		/* XXX */
		sb -> sb_tsdu_them = sb -> sb_tsdu_us = 0;
	if (s -> s_mask & SMASK_CN_SET) {
		ss -> ss_settings = s -> s_settings;
		sb -> sb_settings = s -> s_settings;
	}
	sb -> sb_requirements = (s -> s_mask & SMASK_CN_REQ ? s -> s_cn_require
							 : SR_DEFAULT) & SR_MYREQUIRE;
	if (!ts -> ts_expedited)
		sb -> sb_requirements = u16_bic (sb -> sb_requirements,
						 (unsigned) SR_EXPEDITED);
	ss -> ss_requirements = sb -> sb_requirements;
	ss -> ss_calling.sa_addr = ts -> ts_calling;	/* struct copy */
	if (s -> s_mask & SMASK_CN_CALLING) {
		if (copy_capped (s -> s_calling, ss -> ss_calling.sa_selector,
				 (ptrdiff_t) s -> s_callinglen,
				 sizeof ss -> ss_calling.sa_selector,
				 &ss -> ss_calling.sa_selectlen) != 0)
			return ssaplose (si, SC_PROTOCOL, NULLCP, "invalid calling selector");
	}
	sb -> sb_initiating = ss -> ss_calling;	/* struct copy */
	ss -> ss_called.sa_addr = ts -> ts_called;	/* struct copy */
	if (s -> s_mask & SMASK_CN_CALLED) {
		if (copy_capped (s -> s_called, ss -> ss_called.sa_selector,
				 (ptrdiff_t) s -> s_calledlen,
				 sizeof ss -> ss_called.sa_selector,
				 &ss -> ss_called.sa_selectlen) != 0)
			return ssaplose (si, SC_PROTOCOL, NULLCP, "invalid called selector");
	}
	sb -> sb_responding = ss -> ss_called;	/* struct copy */
	if ((ss -> ss_ssdusize = sb -> sb_tsdu_us - SSDU_MAGIC) < 0)
		ss -> ss_ssdusize = ts -> ts_tsdusize - SSDU_MAGIC;
	ss -> ss_qos = ts -> ts_qos;	/* struct copy */
	ss -> ss_qos.qos_sversion = sb -> sb_version + 1;
	ss -> ss_qos.qos_extended = (sb -> sb_flags & SB_EXPD) ? 1 : 0;
	copySPKTdata (s, ss);
	freespkt (s);
	return OK;
out2:
	;
	freespkt(s);
out1:
	;
	freesblk (sb);
	return NOTOK;
}

/* S-CONNECT.RESPONSE */

#define	dotoken(requires,shift,bit,type) \
{ \
    if (sb -> sb_requirements & requires) \
	switch (sb -> sb_settings & (ST_MASK << shift)) { \
	    case ST_CALL_VALUE << shift: \
		switch (settings & (ST_MASK << shift)) { \
		    case ST_INIT_VALUE << shift: \
			settings &= ~(ST_MASK << shift); \
			settings |= ST_INIT_VALUE << shift; \
			break; \
 \
		    case ST_RESP_VALUE << shift: \
			settings &= ~(ST_MASK << shift); \
			settings |= ST_RESP_VALUE << shift; \
			sb -> sb_owned |= bit; \
			break; \
 \
		    default: \
			return ssaplose (si, SC_PARAMETER, NULLCP, \
				"improper choice of %s token setting", type); \
		} \
		break; \
 \
	    case ST_INIT_VALUE << shift: \
		if ((settings & (ST_MASK << shift)) == (ST_RSVD_VALUE << shift)) \
		    please |= bit; \
		settings &= ~(ST_MASK << shift); \
		settings |= ST_INIT_VALUE << shift; \
		break; \
 \
	    case ST_RESP_VALUE << shift: \
		settings &= ~(ST_MASK << shift); \
		settings |= ST_RESP_VALUE << shift; \
		sb -> sb_owned |= bit; \
		break; \
	} \
}

int SConnResponse (
	int sd,
	struct SSAPref *ref,
	const struct SSAPaddr *responding,
	int status,
	const int requirements,
	int settings,
	const long isn,
	char *data,
	int cc,
	struct SSAPindication *si
) {
	int     result,
			please;
	struct ssapkt *s;
	struct ssapblk *sb;

	if ((sb = findsblk (sd)) == NULL || (sb -> sb_flags & SB_CONN))
		return ssaplose (si, SC_PARAMETER, NULLCP, "invalid session descriptor");
	missingP (ref);
	refmuchP (ref);
	if (ref -> sr_vlen)
		return ssaplose (si, SC_PARAMETER, NULLCP, "bad format for reference");
#ifdef	notdef
	missingP (responding);
#endif
	if (responding)
		sb -> sb_responding = *responding;	/* struct copy */
	switch (status) {
	case SC_ACCEPT:
		if (requirements & ~SR_MYREQUIRE)
			return ssaplose (si, SC_PARAMETER, NULLCP,
							 "requirements settings not supported");
#ifdef	notdef		/* screwy session protocol... */
		if (requirements & ~sb -> sb_requirements)
			return ssaplose (si, SC_PARAMETER, NULLCP,
							 "requirements settings not available");
#endif
		if ((requirements & SR_HALFDUPLEX) && (requirements & SR_DUPLEX))
			return ssaplose (si, SC_PARAMETER, NULLCP,
							 "half-duplex and duplex services are incompatible");
		if ((requirements & SR_EXCEPTIONS)
				&& !(requirements & SR_HALFDUPLEX))
			return ssaplose (si, SC_PARAMETER, NULLCP,
							 "exception service requires half-duplex service");
		{
			uint16_t req;

			if (int2u16 (requirements, &req) != 0)
				return ssaplose (si, SC_PARAMETER, NULLCP,
								 "requirements out of range");
			sb -> sb_requirements &= req;
		}
		sb -> sb_owned = 0, please = 0;
		dotokens ();
		if (sb -> sb_requirements
				& (SR_MINORSYNC | SR_MAJORSYNC | SR_RESYNC)) {
			if (!(sb -> sb_requirements & SR_ACTIVITY)
					|| isn != SERIAL_NONE)
				if (SERIAL_MIN > isn || isn > SERIAL_MAX + 1)
					return ssaplose (si, SC_PARAMETER, NULLCP,
									 "bad choice for initial serial number");
		} else if (isn != SERIAL_NONE)
			return ssaplose (si, SC_PARAMETER, NULLCP,
							 "initial serial number invalid given requirements");
		break;

	case SC_NOTSPECIFIED:
	case SC_CONGESTION:
	case SC_REJECTED:
		break;

	default:
		return ssaplose (si, SC_PARAMETER, NULLCP, "invalid result");
	}
	if (data == NULL)
		cc = 0;
	else if (cc > (sb -> sb_version < SB_VRSN2 ? SC_SIZE : ENCLOSE_MAX))
		return ssaplose (si, SC_PARAMETER, NULLCP, "too much initial user data, %d octets", cc);

	missingP (si);

	if (status != SC_ACCEPT) {
		if ((s = newspkt (SPDU_RF)) == NULL) {
			ssaplose (si, SC_CONGEST, NULLCP, "out of memory");
			goto out1;
		}

		s -> s_mask |= SMASK_RF_REF;
		s -> s_rf_reference = *ref;	/* struct copy */
		if (status == SC_REJECTED) {
			s -> s_mask |= SMASK_RF_REQ;
			if (int2u16 (requirements, &s -> s_rf_require) != 0) {
				ssaplose (si, SC_PARAMETER, NULLCP,
					  "requirements out of range");
				goto out2;
			}
		}
		if ((s -> s_rdata = malloc ((unsigned) (s -> s_rlen = 1 + cc))) == NULL) {
			ssaplose (si, SC_CONGEST, NULLCP, "out of memory");
			goto out2;
		}
		if (int2octet (status & 0xff, s -> s_rdata) != 0) {
			ssaplose (si, SC_PARAMETER, NULLCP, "invalid refuse status");
			goto out2;
		}
		if (cc > 0 && bcopy_int (data, s -> s_rdata + 1, cc) != 0) {
			ssaplose (si, SC_PARAMETER, NULLCP, "invalid user data length");
			goto out2;
		}
		result = refuse (sb, s, si);
		freesblk (sb);

		return (result != NOTOK && status != SC_ACCEPT ? OK : NOTOK);
	}

	if ((s = newspkt (SPDU_AC)) == NULL) {
		ssaplose (si, SC_CONGEST, NULLCP, "out of memory");
		goto out1;
	}

	s -> s_mask |= SMASK_CN_REF | SMASK_CN_OPT | SMASK_CN_VRSN;
	s -> s_cn_reference = *ref;	/* struct copy */
	s -> s_options = CR_OPT_NULL;
	if (int2u8 (1 << sb -> sb_version, &s -> s_cn_version) != 0) {
		ssaplose (si, SC_PARAMETER, NULLCP, "version out of range");
		goto out2;
	}
	if (isn != SERIAL_NONE) {
		s -> s_mask |= SMASK_CN_ISN;
		if (long2u32 (isn, &s -> s_isn) != 0) {
			ssaplose (si, SC_PARAMETER, NULLCP, "serial number out of range");
			goto out2;
		}
	}
	if (sb -> sb_tsdu_us || sb -> sb_tsdu_them) {
		s -> s_mask |= SMASK_CN_TSDU;
		if (int2u16 (GET_TSDU_SIZE (sb -> sb_tsdu_us), &s -> s_tsdu_resp) != 0
				|| int2u16 (GET_TSDU_SIZE (sb -> sb_tsdu_them), &s -> s_tsdu_init) != 0) {
			ssaplose (si, SC_PROTOCOL, NULLCP, "TSDU size out of range");
			goto out2;
		}
	}
	s -> s_mask |= SMASK_CN_REQ;
	if ((s -> s_cn_require = sb -> sb_requirements) & SR_TOKENS) {
		s -> s_mask |= SMASK_CN_SET;
		if (int2u8 (settings, &s -> s_settings) != 0) {
			ssaplose (si, SC_PARAMETER, NULLCP, "token settings out of range");
			goto out2;
		}
	}
	if (please) {
		s -> s_mask |= SMASK_AC_TOKEN;
		if (int2u8 (please, &s -> s_ac_token) != 0) {
			ssaplose (si, SC_PARAMETER, NULLCP, "token request out of range");
			goto out2;
		}
	}
	if (responding) {
		s -> s_mask |= SMASK_CN_CALLED;
		if (bcopy_int (sb -> sb_responding.sa_selector, s -> s_called,
				   sb -> sb_responding.sa_selectlen) != 0)
			return ssaplose (si, SC_PARAMETER, NULLCP, "invalid responding selector");
		s -> s_calledlen = sb -> sb_responding.sa_selectlen;
	}
	if (cc > 0) {
		s -> s_mask |= SMASK_UDATA_PGI;
		s -> s_udata = data, s -> s_ulen = cc;
	} else
		s -> s_udata = NULL, s -> s_ulen = 0;
	if ((result = spkt2sd (s, sb -> sb_fd, 0, si)) == NOTOK)
		freesblk (sb);
	else
		sb -> sb_flags |= SB_CONN;
	s -> s_mask &= ~SMASK_UDATA_PGI;
	s -> s_udata = NULL, s -> s_ulen = 0;
	freespkt(s);
	return result;
out2:
	;
	freespkt (s);
out1:
	;
	freesblk (sb);
	return NOTOK;
}

#undef	dotoken

static int refuse (struct ssapblk *sb, struct ssapkt *s, struct SSAPindication *si) {
	int     result;
	struct TSAPdata txs;
	struct TSAPdata   *tx = &txs;
	struct TSAPdisconnect   tds;
	struct TSAPdisconnect *td = &tds;

	s -> s_mask |= SMASK_RF_DISC;
	s -> s_rf_disconnect = u8_bis (s -> s_rf_disconnect, RF_DISC_RELEASE);
	result = spkt2sd (s, sb -> sb_fd, sb -> sb_flags & SB_EXPD ? 1 : 0, si);
	freespkt (s);
	if (result == NOTOK)
		return NOTOK;
	if (ses_rf_timer >= 0)
		switch (TReadRequest (sb -> sb_fd, tx, ses_rf_timer, td)) {
		case OK:
		default: 		/* what could this be? */
			TXFREE (tx);
			break;

		case NOTOK:
			sb -> sb_fd = NOTOK;
			break;
		}
	return OK;
}
