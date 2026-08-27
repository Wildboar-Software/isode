/* ssapwrite.c - SPM: write various SPDUs  */

#include <stdio.h>
#include <stdlib.h>
#include "spkt.h"

int SWriteRequestAux (
	const struct ssapblk *sb,
	const int code,
	const char *data,
	const int cc,
	const int type,
	const long ssn,
	const int settings,
	const struct SSAPactid *id,
	const struct SSAPactid *oid,
	const struct SSAPref *ref,
	struct SSAPindication *si
) {
	int     result;
	struct ssapkt *s,
			   *p;
	struct TSAPdisconnect   tds;
	struct TSAPdisconnect *td = &tds;
	struct udvec    uvs[3];
	struct udvec  *uv;

	if (sb -> sb_flags & SB_EXPD)
		switch (code) {
		case SPDU_MAA:
			result = PR_MAA;
			goto send_pr;

		case SPDU_AI:
		case SPDU_AD:
		case SPDU_RS:
			result = PR_RS;
			goto send_pr;

		case SPDU_AIA:
		case SPDU_ADA:
		case SPDU_RA:
			result = PR_RA;
send_pr:
			;
			if ((p = newspkt (SPDU_PR)) == NULL)
				return ssaplose (si, SC_CONGEST, NULLCP, "out of memory");
			p -> s_mask |= SMASK_PR_TYPE;
			if (int2u8 (result, &p -> s_pr_type) != 0) {
				freespkt (p);
				return ssaplose (si, SC_PARAMETER, NULLCP,
						 "invalid prepare type");
			}
			result = spkt2sd (p, sb -> sb_fd, 1, si);
			freespkt (p);
			if (result == NOTOK)
				return NOTOK;
			break;

		default:
			break;
		}

	uv = uvs;
	uvs[0].uv_base = uvs[1].uv_base = NULL;

	switch (code) {
	case SPDU_MAP:
	case SPDU_MIP:
	case SPDU_RS:
	case SPDU_AS:
	case SPDU_AR:
	case SPDU_AD:
	case SPDU_AI:
#ifdef	notdef			/* aka SPDU_MAP */
	case SPDU_AE:
#endif
	case SPDU_CD:
		if (s = newspkt (SPDU_GT))
			s -> s_mask |= SMASK_SPDU_GT;
		break;

	default:
		s = newspkt (SPDU_PT);
		break;
	}
	if (s == NULL)
		return ssaplose (si, SC_CONGEST, NULLCP, "out of memory");

	if (spkt2tsdu (s, &uv -> uv_base, &uv -> uv_len) == NOTOK) {
		ssaplose (si, s -> s_errno, NULLCP, NULLCP);
		goto out1;
	}
	freespkt (s);
	uv++;

	if ((s = newspkt (code)) == NULL) {
		ssaplose (si, SC_CONGEST, NULLCP, "out of memory");
		goto out2;
	}
	switch (code) {
	case SPDU_MAP:
		if (type) {
			s -> s_mask |= SMASK_MAP_SYNC;
			if (int2u8 (type, &s -> s_map_sync) != 0)
				goto bad_conv;
		}
		s -> s_mask |= SMASK_MAP_SERIAL;
		if (long2u32 (ssn, &s -> s_map_serial) != 0)
			goto bad_conv;
		break;

	case SPDU_MAA:
		s -> s_mask |= SMASK_MAA_SERIAL;
		if (long2u32 (ssn, &s -> s_maa_serial) != 0)
			goto bad_conv;
		break;

	case SPDU_MIP:
		if (type == SYNC_NOCONFIRM) {
			s -> s_mask |= SMASK_MIP_SYNC;
			s -> s_mip_sync = MIP_SYNC_NOEXPL;
		}
		s -> s_mask |= SMASK_MIP_SERIAL;
		if (long2u32 (ssn, &s -> s_mip_serial) != 0)
			goto bad_conv;
		break;

	case SPDU_MIA:
		s -> s_mask |= SMASK_MIA_SERIAL;
		if (long2u32 (ssn, &s -> s_mia_serial) != 0)
			goto bad_conv;
		break;

	case SPDU_RS:
		if (sb -> sb_requirements & SR_TOKENS) {
			s -> s_mask |= SMASK_RS_SET;
			if (int2u8 (settings, &s -> s_rs_settings) != 0)
				goto bad_conv;
		}
		s -> s_mask |= SMASK_RS_TYPE;
		if (int2u8 (type, &s -> s_rs_type) != 0)
			goto bad_conv;
		s -> s_mask |= SMASK_RS_SSN;
		if (long2u32 (ssn, &s -> s_rs_serial) != 0)
			goto bad_conv;
		break;

	case SPDU_RA:
		if (sb -> sb_requirements & SR_TOKENS) {
			s -> s_mask |= SMASK_RA_SET;
			if (int2u8 (settings, &s -> s_ra_settings) != 0)
				goto bad_conv;
		}
		s -> s_mask |= SMASK_RA_SSN;
		if (long2u32 (ssn, &s -> s_ra_serial) != 0)
			goto bad_conv;
		break;

	case SPDU_AS:
		s -> s_mask |= SMASK_AS_ID;
		s -> s_as_id = *id;	/* struct copy */
		break;

	case SPDU_AR:
		s -> s_mask |= SMASK_AR_OID | SMASK_AR_SSN | SMASK_AR_ID;
		s -> s_ar_oid = *oid;	/* struct copy */
		if (long2u32 (ssn, &s -> s_ar_serial) != 0)
			goto bad_conv;
		s -> s_ar_id = *id;	/* struct copy */
		if (ref) {
			s -> s_mask |= SMASK_AR_REF;
			s -> s_ar_reference = *ref;	/* struct copy */
		}
		break;

	case SPDU_AI:
		s -> s_mask |= SMASK_AI_REASON;
		if (int2u8 (type, &s -> s_ai_reason) != 0)
			goto bad_conv;
		break;

	case SPDU_AD:
		s -> s_mask |= SMASK_AD_REASON;
		if (int2u8 (type, &s -> s_ad_reason) != 0)
			goto bad_conv;
		break;

	case SPDU_ED:
		s -> s_mask |= SMASK_ED_REASON;
		if (int2u8 (type, &s -> s_ed_reason) != 0)
			goto bad_conv;
		break;

	default:
		break;
	}

	if (cc > 0) {
		s -> s_mask |= SMASK_UDATA_PGI;
		s -> s_udata = data, s -> s_ulen = cc;
	} else
		s -> s_udata = NULL, s -> s_ulen = 0;
	result = spkt2tsdu (s, &uv -> uv_base, &uv -> uv_len);
	s -> s_mask &= ~SMASK_UDATA_PGI;
	s -> s_udata = NULL, s -> s_ulen = 0;

	if (result == NOTOK) {
		ssaplose (si, s -> s_errno, NULLCP, NULLCP);
		goto out3;
	}
	freespkt (s);
	uv++;

	uv -> uv_base = NULL;

	if ((result = TWriteRequest (sb -> sb_fd, uvs, td)) == NOTOK)
		ts2sslose (si, "TWriteRequest", td);

	free (uvs[0].uv_base);
	free (uvs[1].uv_base);

	return result;

bad_conv:
	ssaplose (si, SC_PARAMETER, NULLCP, "SPDU field out of range");
out3:
	;
	if (uvs[1].uv_base)
		free (uvs[1].uv_base);
out2:
	;
	if (uvs[0].uv_base)
		free (uvs[0].uv_base);
out1:
	;
	freespkt (s);

	return NOTOK;
}
