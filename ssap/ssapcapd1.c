/* ssapcapd1.c - SPM: write capability data */

#include <stdio.h>
#include <signal.h>
#include "spkt.h"

/* S-CAPABILITY-DATA.REQUEST */

static int SCapdRequestAux (struct ssapblk *sb, char *data, int cc, struct SSAPindication *si);

int SCapdRequest (int sd, char *data, int cc, struct SSAPindication *si) {
	int	    smask;
	int     result;
	struct ssapblk *sb;

	missingP (si);
	smask = sigioblock ();
	ssapPsig (sb, sd);
	toomuchP (sb, data, cc, SX_CDSIZE, "capability");
	result = SCapdRequestAux (sb, data, cc, si);
	sigiomask (smask);
	return result;
}

static int SCapdRequestAux (struct ssapblk *sb, char *data, int cc, struct SSAPindication *si) {
	int     result;

	if (!(sb -> sb_requirements & SR_CAPABILITY))
		return ssaplose (si, SC_OPERATION, NULLCP,
						 "capability data exchange service unavailable");
	if (SDoActivityAux (sb, si, 1, 0) == NOTOK)
		return NOTOK;
	if (sb -> sb_flags & SB_CD)
		return ssaplose (si, SC_OPERATION, NULLCP,
						 "capability data request in progress");
	sb -> sb_flags |= SB_CD;
	if ((result = SWriteRequestAux (sb, SPDU_CD, data, cc, 0, 0L, 0, NULLSD,
									NULLSD, NULLSR, si)) == NOTOK)
		freesblk (sb);
	return result;
}
