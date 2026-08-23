/* ssapmajor1.c - SPM: initiate majorsyncs */

#include <stdio.h>
#include <signal.h>
#include "spkt.h"

/* S-MAJOR-SYNC.REQUEST */

int SMajSyncRequest (int sd, long *ssn, char *data, int cc, struct SSAPindication *si) {
	SBV	    smask;
	int     result;
	struct ssapblk *sb;

	missingP (ssn);
	missingP (si);

	smask = sigioblock ();

	ssapPsig (sb, sd);
	toomuchP (sb, data, cc, SN_SIZE, "majorsync");

	result = SMajSyncRequestAux (sb, ssn, data, cc, MAP_SYNC_NOEND, si);

	sigiomask (smask);

	return result;
}

int SMajSyncRequestAux (
	struct ssapblk *sb,
	long *ssn,
	char *data,
	int cc,
	int opts,
	struct SSAPindication *si
) {
	int     result;

	if (SDoActivityAux (sb, si, 0, 0) == NOTOK)
		return NOTOK;

	if ((sb -> sb_requirements & SR_ACTIVITY) && !(sb -> sb_flags & SB_Vact))
		return ssaplose (si, SC_OPERATION, NULLCP, "no activity in progress");

	if (sb -> sb_flags & SB_MAA)
		return ssaplose (si, SC_OPERATION, "awaiting your majorsync response");

	if ((result = SWriteRequestAux (sb, SPDU_MAP, data, cc, opts, *ssn = sb -> sb_V_M, 0, NULLSD, NULLSD, NULLSR, si)) == NOTOK)
		freesblk (sb);
	else {
		sb -> sb_flags |= SB_MAP;
		if (opts & MAP_SYNC_NOEND) {
			if (sb -> sb_requirements & SR_ACTIVITY)
				sb -> sb_flags |= SB_Vnextact;
		} else
			sb -> sb_flags |= SB_AE, sb -> sb_flags &= ~SB_Vnextact;
		if (sb -> sb_flags & SB_Vsc) {
			sb -> sb_V_A = sb -> sb_V_M;
			sb -> sb_flags &= ~SB_Vsc;
		}
		sb -> sb_V_M++;
	}

	return result;
}
