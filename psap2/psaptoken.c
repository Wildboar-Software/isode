/* psaptoken.c - PPM: tokens */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "ppkt.h"

/* P-TOKEN-GIVE.REQUEST */

int PGTokenRequest (const int sd, const int tokens, struct PSAPindication *pi) {
	SBV	    smask;
	int     result;
	struct psapblk *pb;
	struct SSAPindication   sis;
	struct SSAPabort  *sa = &sis.si_abort;

	missingP (pi);
	smask = sigioblock ();
	psapPsig (pb, sd);
	if ((result = SGTokenRequest (sd, tokens, &sis)) == NOTOK)
		if (SC_FATAL (sa -> sa_reason))
			ss2pslose (pb, pi, "SGTokenRequest", sa);
		else {
			ss2pslose (NULLPB, pi, "SGTokenRequest", sa);
			goto out1;
		}
	else
		pb -> pb_owned &= ~tokens;
	if (result == NOTOK)
		freepblk (pb);
out1:
	;
	sigiomask (smask);
	return result;
}

/* P-TOKEN-PLEASE.REQUEST */

int PPTokenRequest (const int sd, const int tokens, PE *data, const int ndata, struct PSAPindication *pi) {
	SBV	    smask;
	int     len,
			result;
	char   *base,
		   *realbase;
	struct psapblk *pb;
	struct SSAPindication   sis;
	struct SSAPabort  *sa = &sis.si_abort;

	toomuchP (data, ndata, NPDATA, "token");
	missingP (pi);
	smask = sigioblock ();
	psapPsig (pb, sd);
	if ((result = info2ssdu (pb, pi, data, ndata, &realbase, &base, &len,
							 "P-TOKEN-PLEASE user-data", PPDU_NONE)) != OK)
		goto out2;
	if ((result = SPTokenRequest (sd, tokens, base, len, &sis)) == NOTOK)
		if (SC_FATAL (sa -> sa_reason))
			ss2pslose (pb, pi, "SPTokenRequest", sa);
		else {
			ss2pslose (NULLPB, pi, "SPTokenRequest", sa);
			goto out1;
		}
out2:
	;
	if (result == NOTOK)
		freepblk (pb);
	else if (result == DONE)
		result = NOTOK;
out1:
	;
	if (realbase)
		free (realbase);
	else if (base)
		free (base);
	sigiomask (smask);
	return result;
}
