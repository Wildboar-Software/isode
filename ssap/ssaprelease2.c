/* ssaprelease2.c - SPM: respond to release */

#include <stdio.h>
#include <signal.h>
#include "spkt.h"

static int  SRelResponseAux (struct ssapblk *sb, int status, char *data, int cc, struct SSAPindication *si);

/* S-RELEASE.RESPONSE */

int SRelResponse (int sd, int status, char *data, int cc, struct SSAPindication *si) {
	int	    smask;
	int     result;
	struct ssapblk *sb;

	missingP (si);
	smask = sigioblock ();
	ssapFsig (sb, sd);
	toomuchP (sb, data, cc, SR_SIZE, "release");
	result = SRelResponseAux (sb, status, data, cc, si);
	sigiomask (smask);
	return result;
}

/* S-RELEASE.RESPONSE */

static int SRelResponseAux (struct ssapblk *sb, int status, char *data, int cc, struct SSAPindication *si) {
	int     code,
			result;
	struct ssapkt *s;

	switch (status) {
	case SC_ACCEPT:
		code = SPDU_DN;
		break;
	case SC_REJECTED:
		if (!(sb -> sb_requirements & SR_NEGOTIATED))
			return ssaplose (si, SC_OPERATION, NULLCP,
							 "negotiated release service unavailable");
		if (!(sb -> sb_requirements & SR_RLS_EXISTS))
			return ssaplose (si, SC_OPERATION, NULLCP,
							 "release token unavailable");
		if (sb -> sb_owned & ST_RLS_TOKEN)
			return ssaplose (si, SC_OPERATION, NULLCP,
							 "release token owned by you");
		code = SPDU_NF;
		break;

	default:
		return ssaplose (si, SC_PARAMETER, NULLCP,
						 "invalid value for status parameter");
	}
	if ((s = newspkt (code)) == NULL)
		return ssaplose (si, SC_CONGEST, NULLCP, "out of memory");
	if (cc > 0) {
		s -> s_mask |= SMASK_UDATA_PGI;
		s -> s_udata = data, s -> s_ulen = cc;
	} else
		s -> s_udata = NULL, s -> s_ulen = 0;
	result = spkt2sd (s, sb -> sb_fd, 0, si);
	s -> s_mask &= ~SMASK_UDATA_PGI;
	s -> s_udata = NULL, s -> s_ulen = 0;
	freespkt (s);
	if (result == NOTOK || code == SPDU_DN)
		freesblk (sb);
	else
		sb -> sb_flags &= ~SB_FINN;
	return result;
}
