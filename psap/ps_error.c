/* ps_error.c - presentation stream error to string */

#include <stdio.h>
#include "psap.h"

static char *ps_errorlist[] = {
	"Error 0",
	"Overflow in ID",
	"Overflow in length",
	"Out of memory",
	"End of file",
	"End of file reading extended ID",
	"End of file reading extended length",
	"Length Mismatch",
	"Truncated",
	"Indefinite length in primitive form",
	"I/O error",
	"Extraneous octets",
	"XXX"
};

static int ps_maxerror = sizeof ps_errorlist / sizeof ps_errorlist[0];

char *ps_error (const int c) {
	char  *bp;
	static char buffer[30];

	if (c < ps_maxerror && (bp = ps_errorlist[c]))
		return bp;

	sprintf (buffer, "Error %d", c);
	return buffer;
}
