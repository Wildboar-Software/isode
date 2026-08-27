/* psapmajor1.c - PPM: initiate majorsyncs */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "ppkt.h"

/*    P-{MAJOR-SYNC,ACTIVITY-END}.REQUEST */

int PMajSyncRequestAux (
	const int sd,
	long *ssn,
	PE *data,
	int ndata,
	struct PSAPindication *pi,
	const char *dtype,
	int (*sfunc)(int sd, long *ssn, const char *data, int cc, struct SSAPindication *si),
	const char *stype
) {
	SBV	    smask;
	int     len,
			result;
	char   *base,
		   *realbase;
	struct psapblk *pb;
	struct SSAPindication   sis;
	struct SSAPabort  *sa = &sis.si_abort;

	toomuchP (data, ndata, NPDATA, dtype);
	missingP (pi);
	missingP (sfunc);
	missingP (stype);
	smask = sigioblock ();
	psapPsig (pb, sd);
	if ((result = info2ssdu (pb, pi, data, ndata, &realbase, &base, &len,
							 "P-MAJOR-SYNC (ACTIVITY-END) user-data",
							 PPDU_NONE)) != OK)
		goto out2;
	if ((result = (*sfunc) (sd, ssn, base, len, &sis)) == NOTOK)
		if (SC_FATAL (sa -> sa_reason))
			ss2pslose (pb, pi, stype, sa);
		else {
			ss2pslose (NULLPB, pi, stype, sa);
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
