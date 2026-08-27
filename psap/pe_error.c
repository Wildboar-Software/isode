/* pe_error.c - presentation element error to string */

#include <stdio.h>
#include "psap.h"

static char *pe_errorlist[] = {
	"Error 0",
	"Overflow",
	"Out of memory",
	"No such bit",
	"Malformed universal timestring",
	"Malformed generalized timestring",
	"No such member",
	"Not a primitive form",
	"Not a constructor form",
	"Class/ID mismatch in constructor",
	"Malformed object identifier",
	"Malformed bitstring",
	"Type not supported",
	"Signed integer not expected",
	"Duplicate member of set",
	"Syntax coding error",
};

static int pe_maxerror = sizeof pe_errorlist / sizeof pe_errorlist[0];

char *pe_error (const int c) {
	char  *bp;
	static char buffer[30];

	if (c < pe_maxerror && (bp = pe_errorlist[c]))
		return bp;

	sprintf (buffer, "Error %d", c);
	return buffer;
}
