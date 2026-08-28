/* ssapexpd.c - SPM: write expedited data */

#include <stdio.h>
#include <signal.h>
#include "spkt.h"
static int SExpdRequestAux (struct ssapblk *sb, const char *data, int cc, struct SSAPindication *si);


/* S-EXPEDITED-DATA.REQUEST */

static int  SExpdRequestAux (struct ssapblk *sb, const char *data, int cc, struct SSAPindication *si);

int SExpdRequest (int sd, const char *data, int cc, struct SSAPindication *si) {
	SBV	    smask;
	int     result;
	struct ssapblk *sb;

	missingP (data);
	if (cc > SX_EXSIZE)
		return ssaplose (si, SC_PARAMETER, NULLCP,
						 "too much expedited user data, %d octets", cc);
	missingP (si);
	smask = sigioblock ();
	ssapPsig (sb, sd);
	result = SExpdRequestAux (sb, data, cc, si);
	sigiomask (smask);
	return result;
}

static int SExpdRequestAux (struct ssapblk *sb, const char *data, int cc, struct SSAPindication *si) {
	int     result;
	struct ssapkt *s;

	if (!(sb -> sb_requirements & SR_EXPEDITED))
		return ssaplose (si, SC_OPERATION, NULLCP,
						 "expedited data service unavailable");
	if ((s = newspkt (SPDU_EX)) == NULL)
		return ssaplose (si, SC_CONGEST, NULLCP, "out of memory");
	s -> s_udata = data, s -> s_ulen = cc;
	result = spkt2sd (s, sb -> sb_fd, 1, si);
	s -> s_udata = NULL, s -> s_ulen = 0;
	freespkt (s);
	if (result == NOTOK)
		freesblk (sb);
	return result;
}
