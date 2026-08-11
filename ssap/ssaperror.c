/* ssaperror.c - return SSAP error code in string form */

#include <stdio.h>
#include "ssap.h"

static char *reject_err0[] = {
	"Reason not specified",
	"Temporary congestion",
	"Rejected"
};

static int reject_err0_cnt = sizeof reject_err0 / sizeof reject_err0[0];

static char *reject_err8[] = {
	"unknown error code 0x80",
	"SSAP identifier unknown",
	"SS-user not attached to SSAP",
	"Congestion at SSAP",
	"Proposed protocol versions not supported",
	"Address unknown",
	"Connect request refused on this network connection",
	"Transport disconnect",
	"Provider-initiated abort",
	"Protocol error",
	"Invalid parameter",
	"Invalid operation",
	"Timer expired",
	"Indications waiting"
};

static int reject_err8_cnt = sizeof reject_err8 / sizeof reject_err8[0];

char *SErrString (int code) {
	int    fcode;
	static char buffer[50];

	code &= 0xff;
	if (code & SC_BASE) {
		if ((fcode = code & ~SC_BASE) < reject_err8_cnt)
			return reject_err8[fcode];
	} else if (code < reject_err0_cnt)
		return reject_err0[code];

	sprintf (buffer, "unknown error code 0x%x", code);
	return buffer;
}
