/* psapminor1.c - PPM: initiate minorsyncs */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "ppkt.h"

/* P-MINOR-SYNC.REQUEST */

int PMinSyncRequest (int sd, int type, long *ssn, PE *data, int ndata, struct PSAPindication *pi) {
	int	    smask;
	int     len,
			result;
	char   *base,
		   *realbase;
	struct psapblk *pb;
	struct SSAPindication   sis;
	struct SSAPabort  *sa = &sis.si_abort;

	toomuchP (data, ndata, NPDATA, "minorsync");
	missingP (pi);
	smask = sigioblock ();
	psapPsig (pb, sd);
	if ((result = info2ssdu (pb, pi, data, ndata, &realbase, &base, &len,
							 "P-MINOR-SYNC user-data", PPDU_NONE)) != OK)
		goto out2;
	if ((result = SMinSyncRequest (sd, type, ssn, base, len, &sis)) == NOTOK)
		if (SC_FATAL (sa -> sa_reason))
			ss2pslose (pb, pi, "SMinSyncRequest", sa);
		else {
			ss2pslose (NULLPB, pi, "SMinSyncRequest", sa);
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
