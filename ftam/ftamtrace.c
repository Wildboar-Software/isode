/* ftamtrace.c - FPM: tracing */







#include <stdio.h>
#include <signal.h>
#include "fpkt.h"

LLog _ftam_log = {
	"ftam.log", NULLCP, NULLCP, LLOG_FATAL | LLOG_EXCEPTIONS | LLOG_NOTICE,
	LLOG_NONE, -1, LLOGCLS | LLOGCRT | LLOGZER, NOTOK
};
LLog *ftam_log = &_ftam_log;

static char *ftam_appls[] = {
	"Abstract-Syntax-Name",
	"Access-Context",
	"Access-Passwords",
	"Access-Request",
	"Account",
	"Action-Result",
	"Activity-Identifier",
	"Application-Entity-Title",
	"Change-Attributes",
	"Charging",
	"Concurrency-Control",
	"Constraint-Set-Name",
	"Create-Attributes",
	"Diagnostic",
	"Document-Type-Name",
	"FADU-Identity",
	"FADU-Lock",
	"Password",
	"Read-Attributes",
	"Select-Attributes",
	"Shared-ASE-Information",
	"State-Result",
	"User-Identity"
};

static int ftam_nappl = sizeof ftam_appls / sizeof ftam_appls[0];

/*    set tracing */

int FHookRequest (int sd, IFP tracing, struct FTAMindication *fti) {
	SBV	    smask;
	struct ftamblk *fsb;

	missingP (fti);

	smask = sigioblock ();

	ftamPsig (fsb, sd);

	if (fsb -> fsb_trace = tracing)
		pe_applist = ftam_appls, pe_maxappl = ftam_nappl;

	sigiomask (smask);

	return OK;
}

/*    user-defined tracing */

int FTraceHook (int sd, char *event, char *fpdu, PE pe, int rw) {
	if (event)
		LLOG (ftam_log, LLOG_ALL, ("%s %s", rw > 0 ? "event"
								   : rw == 0 ? "action" : "exception", event));

	if (pe)
		pvpdu (ftam_log, print_FTAM_PDU_P, pe, fpdu ? fpdu : "FPDU", rw);

	ll_sync (ftam_log);
}
