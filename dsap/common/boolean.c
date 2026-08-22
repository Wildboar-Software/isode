/*
	SYNTAX:
		boolean ::= "TRUE" | "FALSE"
*/
#include <string.h>
#include "quipu/util.h"
#include "quipu/attr.h"
#include "psap.h"

extern int strprint(PS ps, char *str, int format);
extern int lexequ(char *str1, char *str2);

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

static void bool_strprint (PS ps, void *value, int format) {
	strprint (ps, (char *) value, format);
}

static void *bool_strdup (void *value) {
	return strdup ((char *) value);
}

static int bool_lexequ (void *value1, void *value2) {
	return lexequ ((char *) value1, (char *) value2);
}

void boolean_syntax (void) {
	add_attribute_syntax ("boolean",
						  boolenc,		booldec,
						  boolget,		bool_strprint,
						  bool_strdup,	bool_lexequ,
						  free,		NULLCP,
						  NULL,		FALSE);
}
