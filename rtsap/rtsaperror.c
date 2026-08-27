/* rtsaperror.c - return RtSAP error code in string form */

#include <stdio.h>
#include "rtsap.h"

static char *reject_err0[] = {
	"Busy",
	"Cannot recover",
	"Validation failure",
	"Unacceptable dialogue mode",
	"Rejected by responder",
	"Address unknown",
	"Connect request refused on this network connection",
	"Session disconnect",
	"Protocol error",
	"Congestion at RtSAP",
	"Remote system problem",
	"Presentation disconnect",
	"ACS disconnect",
	"Peer aborted association",
	"Invalid parameter",
	"Invalid operation",
	"Timer expired",
	"Indications waiting",
	"Transfer failure"
};

static int reject_err0_cnt = sizeof reject_err0 / sizeof reject_err0[0];

char *RtErrString (const int code) {
	static char buffer[50];

	if (code < reject_err0_cnt)
		return reject_err0[code];

	sprintf (buffer, "unknown error code 0x%x", code);
	return buffer;
}
