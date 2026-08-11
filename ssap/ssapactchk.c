/* ssapactchk.c - SPM: check activity constraints */







#include <stdio.h>
#include "spkt.h"

int SDoActivityAux (struct ssapblk *sb, struct SSAPindication *si, int act, int rls) {
	if (act) {
		if (!(sb -> sb_requirements & SR_ACT_EXISTS))
			return ssaplose (si, SC_OPERATION, NULLCP, "activity management service unavailable");

		if (sb -> sb_flags & SB_Vact)
			return ssaplose (si, SC_OPERATION, NULLCP, "activity in progress");
	} else if (!(sb -> sb_requirements & SR_MAJ_EXISTS))
		return ssaplose (si, SC_OPERATION, NULLCP, "major synchronize service unavailable");

	if ((sb -> sb_requirements & SR_DAT_EXISTS)	&& !(sb -> sb_owned & ST_DAT_TOKEN))
		return ssaplose (si, SC_OPERATION, NULLCP, "data token not owned by you");

	if ((sb -> sb_requirements & SR_MIN_EXISTS)	&& !(sb -> sb_owned & ST_MIN_TOKEN))
		return ssaplose (si, SC_OPERATION, NULLCP, "minorsync not owned by you");

	if (act) {
		if (!(sb -> sb_owned & ST_ACT_TOKEN))
			return ssaplose (si, SC_OPERATION, NULLCP, "activity token not owned by you");
	} else if (!(sb -> sb_owned & ST_MAJ_TOKEN))
		return ssaplose (si, SC_OPERATION, NULLCP, "majorsync token not owned by you");

	if (rls)
		if ((sb -> sb_requirements & SR_RLS_EXISTS)	&& !(sb -> sb_owned & ST_RLS_TOKEN))
			return ssaplose (si, SC_OPERATION, NULLCP, "release token not owned by you");

	return OK;
}
