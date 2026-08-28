/* ssapminor2.c - SPM: respond to minorsyncs */

#include <stdio.h>
#include <signal.h>
#include "spkt.h"
static int SMinSyncResponseAux (struct ssapblk *sb, const long ssn, const char *data, int cc, struct SSAPindication *si);


static int  SMinSyncResponseAux (struct ssapblk *sb, const long ssn, const char *data, int cc, struct SSAPindication *si);

/* S-MINOR-SYNC.RESPONSE */

int SMinSyncResponse (int sd, const long ssn, const char *data, int cc, struct SSAPindication *si) {
	SBV	    smask;
	int     result;
	struct ssapblk *sb;

	if (SERIAL_MIN > ssn || ssn > SERIAL_MAX)
		return ssaplose (si, SC_PARAMETER, NULLCP, "invalid serial number");
	missingP (si);
	smask = sigioblock ();
	ssapPsig (sb, sd);
	toomuchP (sb, data, cc, SN_SIZE, "minorsync");
	result = SMinSyncResponseAux (sb, ssn, data, cc, si);
	sigiomask (smask);
	return result;
}

static int SMinSyncResponseAux (struct ssapblk *sb, const long ssn, const char *data, int cc, struct SSAPindication *si) {
	int     result;

	if (!(sb -> sb_requirements & SR_MINORSYNC))
		return ssaplose (si, SC_OPERATION, NULLCP,
						 "minor synchronize service unavailable");
	if (!(sb -> sb_flags & SB_Vsc))
		return ssaplose (si, SC_OPERATION, NULLCP,
						 "no minorsync in progress");
	if (ssn < sb -> sb_V_A)
		return ssaplose (si, SC_OPERATION, NULLCP,
						 "bad choice for minor ssn, should be >= %ld", sb -> sb_V_A);
	if ((result = SWriteRequestAux (sb, SPDU_MIA, data, cc, 0, ssn, 0, NULLSD,
									NULLSD, NULLSR, si)) == NOTOK)
		freesblk (sb);
	else
		sb -> sb_V_A = ssn + 1;
	return result;
}
