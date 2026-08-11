/* psapresync2.c - PPM: respond to resyncs */







#include <stdio.h>
#include <signal.h>
#include "ppkt.h"

/* P-RESYNCHRONIZE.RESPONSE */

int PReSyncResponse (int sd, long ssn, int settings, PE *data, int ndata, struct PSAPindication *pi) {
	SBV	    smask;
	int     len,
			result;
	char   *base,
		   *realbase;
	struct psapblk *pb;
	struct SSAPindication   sis;
	struct SSAPabort  *sa = &sis.si_abort;

	toomuchP (data, ndata, NPDATA, "resync");
	missingP (pi);

	smask = sigioblock ();

	psapPsig (pb, sd);

	if ((result = info2ssdu (pb, pi, data, ndata, &realbase, &base, &len,
							 "P-RESYNCHRONIZE user-data", PPDU_RSA)) != OK)
		goto out2;

	if ((result = SReSyncResponse (sd, ssn, settings, base, len, &sis))
			== NOTOK)
		if (SC_FATAL (sa -> sa_reason))
			ss2pslose (pb, pi, "SReSyncResponse", sa);
		else {
			ss2pslose (NULLPB, pi, "SReSyncResponse", sa);
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
