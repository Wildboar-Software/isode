/* ssapreport.c - SPM: exception reports */

#include <stdio.h>
#include <signal.h>
#include "spkt.h"
static int SUReportRequestAux (struct ssapblk *sb, const int reason, const char *data, const int cc, struct SSAPindication *si);


static int  SUReportRequestAux (struct ssapblk *sb, const int reason, const char *data, const int cc, struct SSAPindication *si);

/* S-U-EXCEPTION-REPORT.REQUEST */

int SUReportRequest (const int sd, const int reason, const char *data, const int cc, struct SSAPindication *si) {
	SBV	    smask;
	int     result;
	struct ssapblk *sb;

	if (!(SP_OK (reason)))
		return ssaplose (si, SC_PARAMETER, NULLCP, "invalid reason");
	missingP (si);
	smask = sigioblock ();
	ssapPsig (sb, sd);
	toomuchP (sb, data, cc, SP_SIZE, "report");
	result = SUReportRequestAux (sb, reason, data, cc, si);
	sigiomask (smask);
	return result;
}

static int SUReportRequestAux (struct ssapblk *sb, const int reason, const char *data, const int cc, struct SSAPindication *si) {
	int	    result;

	if (!(sb -> sb_requirements & SR_EXCEPTIONS))
		return ssaplose (si, SC_OPERATION, NULLCP,
						 "exceptions service unavailable");
	if (!(sb -> sb_requirements & SR_DAT_EXISTS))
		return ssaplose (si, SC_OPERATION, NULLCP,
						 "data token not available");
	if (sb -> sb_owned & ST_DAT_TOKEN)
		return ssaplose (si, SC_OPERATION, NULLCP,
						 "data token owned by you");
	if ((sb -> sb_requirements & SR_ACTIVITY)
			&& !(sb -> sb_flags & SB_Vact))
		return ssaplose (si, SC_OPERATION, NULLCP,
						 "no activity in progress");
	if ((result = SWriteRequestAux (sb, SPDU_ED, data, cc, reason, 0L, 0,
									NULLSD, NULLSD, NULLSR, si)) == NOTOK)
		freesblk (sb);
	else
		sb -> sb_flags |= SB_ED;
	return result;
}
