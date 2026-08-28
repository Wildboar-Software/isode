/* ssapmajor2.c - SPM: respond to majorsyncs */

#include <stdio.h>
#include <signal.h>
#include "spkt.h"

/* S-MAJOR-SYNC.RESPONSE */

int SMajSyncResponse (int sd, const char *data, int cc, struct SSAPindication *si) {
	SBV	    smask;
	int     result;
	struct ssapblk *sb;

	missingP (si);

	smask = sigioblock ();

	ssapPsig (sb, sd);
	toomuchP (sb, data, cc, SN_SIZE, "majorsync");

	result = SMajSyncResponseAux (sb, data, cc, si);

	sigiomask (smask);

	return result;
}

int SMajSyncResponseAux (struct ssapblk *sb, const char *data, int cc, struct SSAPindication *si) {
	int     result;

	if (!(sb -> sb_requirements & SR_MAJORSYNC)
			&& !(sb -> sb_requirements & SR_ACTIVITY)
			&& !(sb -> sb_flags & SB_Vact))
		return ssaplose (si, SC_OPERATION, NULLCP,
						 "major synchronize service unavailable");
	if (!(sb -> sb_flags & SB_MAA))
		return ssaplose (si, SC_OPERATION, NULLCP,
						 "no majorsync in progress");

	if ((result = SWriteRequestAux (sb, SPDU_MAA, data, cc, 0,
									sb -> sb_V_M - 1, 0, NULLSD, NULLSD, NULLSR, si)) == NOTOK)
		freesblk (sb);
	else {
		sb -> sb_V_A = sb -> sb_V_R = sb -> sb_V_M;
		if (sb -> sb_requirements & SR_ACTIVITY)
			if (sb -> sb_flags & SB_Vnextact)
				sb -> sb_flags |= SB_Vact;
			else
				sb -> sb_flags &= ~SB_Vact;

		sb -> sb_flags &= ~(SB_MAA | SB_AE);
	}

	return result;
}
