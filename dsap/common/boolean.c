/*
	SYNTAX:
		boolean ::= "TRUE" | "FALSE"
*/

#include "quipu/util.h"
#include "quipu/attr.h"
#include "psap.h"

extern int strprint(PS ps, char *str, int format);
extern int sfree(char *x);
extern int lexequ();
extern char *strdup(const char *);

static PE boolenc (char *x)
{
	return (bool2prim (lexequ (x,"TRUE") ? 0 : 1));
}

static char * booldec (PE pe)
{
	if (! test_prim_pe (pe,PE_CLASS_UNIV,PE_PRIM_BOOL))
		return (NULLCP);
	if (prim2flag (pe) == 1)
		return (strdup ("TRUE"));
	else
		return (strdup ("FALSE"));
}

static char *
boolget (char *x) {
	if ((lexequ (x,"TRUE") == 0) || (lexequ (x,"FALSE") == 0))
		return (strdup(x));
	parse_error ("TRUE or FALSE expected (%s)",x);
	return (NULLCP);
}

int boolean_syntax (void) {
	add_attribute_syntax ("boolean",
						  boolenc,		booldec,
						  boolget,		strprint,
						  strdup,		lexequ,
						  sfree,		NULLCP,
						  NULLIFP,	FALSE);
}
