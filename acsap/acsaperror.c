/* acsaperror.c - return AcSAP error code in string form */

#include <stdio.h>
#include "acsap.h"

static char *reject_err0[] = {
	"Accepted",
	"Permanent",
	"Transient",
	"Rejected by service-user: null",
	"Rejected by service-user: no reason given",
	"Application context name not supported",
	"Calling AP title not recognized",
	"Calling AP invocation-ID not recognized",
	"Calling AE qualifier not recognized",
	"Calling AE invocation-ID not recognized",
	"Called AP title not recognized",
	"Called AP invocation-ID not recognized",
	"Called AE qualifier not recognized",
	"Called AE invocation-ID not recognized",
	"Rejected by service-provider: null",
	"Rejected by service-provider: no reason given",
	"No common acse version",
	"Address unknown",
	"Connect request refused on this network connection",
	"Local limit exceeded",
	"Presentation disconnect",
	"Protocol error",
	"Peer aborted association",
	"Invalid parameter",
	"Invalid operation",
	"Timer expired"
};

static int reject_err0_cnt = sizeof reject_err0 / sizeof reject_err0[0];

char *AcErrString (int code) {
	static char buffer[50];

	if (code < reject_err0_cnt)
		return reject_err0[code];

	sprintf (buffer, "unknown error code %d", code);
	return buffer;
}
