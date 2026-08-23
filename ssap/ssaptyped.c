/* ssaptyped.c - SPM: write typed data */

#include <stdio.h>
#include <signal.h>
#include "spkt.h"

/* S-TYPED-DATA.REQUEST */

int STypedRequest (int sd, char *data, int cc, struct SSAPindication *si) {
	SBV	    smask;
	int     result;
	struct udvec uvs[2];
	struct udvec *uv = uvs;
	struct ssapblk *sb;

	missingP (data);
	if (cc <= 0)
		return ssaplose (si, SC_PARAMETER, NULLCP,
						 "illegal value for TSSDU length (%d)", cc);
	missingP (si);

	smask = sigioblock ();

	ssapPsig (sb, sd);

	uv -> uv_base = data, uv -> uv_len = cc, uv++;
	uv -> uv_base = NULL;

	result = SDataRequestAux (sb, SPDU_TD, uvs, 1, 1, si);

	sigiomask (smask);

	return result;
}
