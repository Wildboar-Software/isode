/* psapactivity.c - PPM: activities */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "ppkt.h"

/* P-CONTROL-GIVE.REQUEST */

int PGControlRequest (int sd, struct PSAPindication *pi) {
	int	    smask;
	int     result;
	struct psapblk *pb;
	struct SSAPindication   sis;
	struct SSAPabort  *sa = &sis.si_abort;

	missingP (pi);
	smask = sigioblock ();
	psapPsig (pb, sd);
	if ((result = SGControlRequest (sd, &sis)) == NOTOK)
		if (SC_FATAL (sa -> sa_reason))
			ss2pslose (pb, pi, "SGControlRequest", sa);
		else {
			ss2pslose (NULLPB, pi, "SGControlRequest", sa);
			goto out1;
		}
	else
		pb -> pb_owned = 0;
	if (result == NOTOK)
		freepblk (pb);
out1:
	;
	sigiomask (smask);
	return result;
}

/* P-ACTIVITY-START.REQUEST */

int PActStartRequest (int sd, struct SSAPactid *id, PE *data, int ndata, struct PSAPindication *pi) {
	int	    smask;
	int     len,
			result;
	char   *base,
		   *realbase;
	struct psapblk *pb;
	struct SSAPindication   sis;
	struct SSAPabort  *sa = &sis.si_abort;

	toomuchP (data, ndata, NPDATA, "activity start");
	missingP (pi);
	smask = sigioblock ();
	psapPsig (pb, sd);
	if ((result = info2ssdu (pb, pi, data, ndata, &realbase, &base, &len,
							 "P-ACTIVITY-START user-data", PPDU_NONE)) != OK)
		goto out2;
	if ((result = SActStartRequest (sd, id, base, len, &sis)) == NOTOK)
		if (SC_FATAL (sa -> sa_reason))
			ss2pslose (pb, pi, "SActStartRequest", sa);
		else {
			ss2pslose (NULLPB, pi, "SActStartRequest", sa);
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

/* P-ACTIVITY-RESUME.REQUEST */

int PActResumeRequest (int sd, struct SSAPactid *id, struct SSAPactid *oid, long ssn, struct SSAPref *ref, PE *data, int ndata, struct PSAPindication *pi) {
	int	    smask;
	int     len,
			result;
	char   *base,
		   *realbase;
	struct psapblk *pb;
	struct SSAPindication   sis;
	struct SSAPabort  *sa = &sis.si_abort;

	toomuchP (data, ndata, NPDATA, "activity resume");
	missingP (pi);
	smask = sigioblock ();
	psapPsig (pb, sd);
	if ((result = info2ssdu (pb, pi, data, ndata, &realbase, &base, &len,
							 "P-ACTIVITY-RESUME user-data", PPDU_NONE)) != OK)
		goto out2;
	if ((result = SActResumeRequest (sd, id, oid, ssn, ref, base, len, &sis))
			== NOTOK)
		if (SC_FATAL (sa -> sa_reason))
			ss2pslose (pb, pi, "SActResumeRequest", sa);
		else {
			ss2pslose (NULLPB, pi, "SActResumeRequest", sa);
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

/*    P-ACTIVITY-{INTERRUPT,DISCARD}.REQUEST */

int PActIntrRequestAux (
	int sd,
	int reason,
	struct PSAPindication *pi,
	int (*sfunc)(int sd, int reason, struct SSAPindication *si),
	char *stype
) {
	int	    smask;
	int     result;
	struct psapblk *pb;
	struct SSAPindication   sis;
	struct SSAPabort  *sa = &sis.si_abort;

	missingP (pi);
	missingP (sfunc);
	missingP (stype);
	smask = sigioblock ();
	psapPsig (pb, sd);
	if ((result = (*sfunc) (sd, reason, &sis)) == NOTOK)
		if (SC_FATAL (sa -> sa_reason))
			ss2pslose (pb, pi, stype, sa);
		else {
			ss2pslose (NULLPB, pi, stype, sa);
			goto out1;
		}
	if (result == NOTOK)
		freepblk (pb);
out1:
	;
	sigiomask (smask);
	return result;
}

/*    P-ACTIVITY-{INTERRUPT,DISCARD}.RESPONSE */

int PActIntrResponseAux (
	int sd,
	struct PSAPindication *pi,
	int (*sfunc)(int sd, struct SSAPindication *si),
	char *stype
) {
	int	    smask;
	int     result;
	struct psapblk *pb;
	struct SSAPindication   sis;
	struct SSAPabort  *sa = &sis.si_abort;

	missingP (pi);
	missingP (sfunc);
	missingP (stype);
	smask = sigioblock ();
	psapPsig (pb, sd);
	if ((result = (*sfunc) (sd, &sis)) == NOTOK)
		if (SC_FATAL (sa -> sa_reason))
			ss2pslose (pb, pi, stype, sa);
		else {
			ss2pslose (NULLPB, pi, stype, sa);
			goto out1;
		}
	if (result == NOTOK)
		freepblk (pb);
out1:
	;
	sigiomask (smask);
	return result;
}
