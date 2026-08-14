/*
	SYNTAX:
		boolean ::= "TRUE" | "FALSE"
*/
#include <string.h>
#include "quipu/util.h"
#include "quipu/attr.h"
#include "psap.h"

extern int strprint(PS ps, char *str, int format);
extern int lexequ();

static PE boolenc (void *value) {
	char *x = (char *) value;
	return (bool2prim (lexequ (x,"TRUE") ? 0 : 1));
}

static void * booldec (PE pe) {
	if (! test_prim_pe (pe,PE_CLASS_UNIV,PE_PRIM_BOOL))
		return (NULLCP);
	if (prim2flag (pe) == 1)
		return (strdup ("TRUE"));
	else
		return (strdup ("FALSE"));
}

static void *boolget (char *x) {
	if ((lexequ (x, "TRUE") == 0) || (lexequ (x, "FALSE") == 0))
		return (strdup(x));
	parse_error ("TRUE or FALSE expected (%s)", x);
	return (NULLCP);
}

int boolean_syntax (void) {
	add_attribute_syntax ("boolean",
						  boolenc,		booldec,
						  boolget,		(AttributeValuePrinter)strprint,
						  (AttributeValueCopier)strdup,		lexequ,
						  free,		NULLCP,
						  NULL,		FALSE);
}
