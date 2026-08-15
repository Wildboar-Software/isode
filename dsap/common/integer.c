#include <stdlib.h>
#include "quipu/util.h"
#include "quipu/attr.h"
#include "psap.h"

static PE intenc (void *value)
{
	int *x = (int *) value;

	return (int2prim(*x));
}

static PE enumenc (void *value)
{
	int *x = (int *) value;

	return enumint2prim ((integer)*x);
}

static void * intdec (PE pe)
{
	int * x;

	if (! test_prim_pe (pe,PE_CLASS_UNIV,PE_PRIM_INT))
		return (0);
	x = (int *) smalloc (sizeof (int));
	*x = prim2num(pe);
	return x;
}

static void * enumdec (PE pe)
{
	int *x;

	if (!test_prim_pe (pe,PE_CLASS_UNIV,PE_PRIM_ENUM))
		return 0;
	x = (int *) smalloc (sizeof (int));
	*x = prim2enum(pe);
	return x;
}

static void intprint (PS ps, void *value, int format)
{
	int *x = (int *) value;

	ps_printf (ps,"%d",*x);
}

#define enumprint intprint

static void *intdup (void *value) {
	int *x = (int *) value;
	int *y;
	y = (int *) smalloc (sizeof (int));
	*y = *x;
	return (y);
}

#define enumdup intdup

static int intcmp (void *value1, void *value2) {
	int *x = (int *) value1;
	int *y = (int *) value2;

	return ( *x == *y ? 0 : (*x > *y ? 1 : -1) );
}

#define enumcmp intcmp

static void intfree (void *value) {
	int *x = (int *) value;

	free ((char *) x);
}

#define enumfree intfree

static void *intparse (char *str) {
	int atoi(const char *);
	int * x;
	x = (int *) smalloc (sizeof (int));
	*x = atoi (str);
	return (x);
}

#define enumparse intparse

void integer_syntax (void) {
	add_attribute_syntax ("integer",
						  intenc,		intdec,
						  intparse,		intprint,
						  intdup,		intcmp,
						  intfree,	NULLCP,
						  NULL,		FALSE);
	add_attribute_syntax("enumerated",
						 enumenc, 	enumdec,
						 enumparse,	enumprint,
						 enumdup,	enumcmp,
						 enumfree,		NULLCP,
						 NULL,			FALSE);
}
