/* psaprelease2.c - PPM: respond to release */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "ppkt.h"

/* P-RELEASE.RESPONSE */

int PRelResponse (const int sd, const int status, PE *data, const int ndata, struct PSAPindication *pi) {
	SBV	    smask;
	int	    len,
			result;
	char   *base,
		   *realbase;
	struct psapblk *pb;
	struct SSAPindication   sis;
	struct SSAPabort  *sa = &sis.si_abort;

	toomuchP (data, ndata, NPDATA, "release");
	missingP (pi);
	smask = sigioblock ();
	psapFsig (pb, sd);
	switch (result = info2ssdu (pb, pi, data, ndata, &realbase, &base, &len,
								"P-RELEASE user-data", PPDU_NONE)) {
	case NOTOK:
		goto out2;
	case OK:
	default:
		break;
	case DONE:
		result = NOTOK;
		goto out1;
	}
	if ((result = SRelResponse (pb -> pb_fd, status, base, len, &sis))
			== NOTOK)
		if (SC_FATAL (sa -> sa_reason)) {
			ss2pslose (pb, pi, "SRelResponse", sa);
			goto out2;
		} else {
			ss2pslose (NULLPB, pi, "SRelResponse", sa);
			goto out1;
		}
	if (status == SC_ACCEPT)
		pb -> pb_fd = NOTOK;
	else
		pb -> pb_flags &= ~PB_FINN;
	result = OK;
out2:
	;
	if (result == NOTOK || status == SC_ACCEPT)
		freepblk (pb);
out1:
	;
	if (base)
		free (base);
	sigiomask (smask);
	return result;
}
