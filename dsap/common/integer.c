

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/integer.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: integer.c,v $
 * Revision 9.0  1992/06/16  12:12:39  isode
 * Release 8.0
 *
 */

/*
 *                                NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

#include "quipu/util.h"
#include "quipu/attr.h"
#include "psap.h"

static PE intenc (int *x)
{
	return (int2prim(*x));
}

static PE enumenc (int *x)
{
	return enumint2prim ((integer)*x);
}

static int * intdec (PE pe)
{
	int * x;

	if (! test_prim_pe (pe,PE_CLASS_UNIV,PE_PRIM_INT))
		return (0);

	x = (int *) smalloc (sizeof (int));
	*x = prim2num(pe);

	return x;

}

static int * enumdec (PE pe)
{
	int *x;

	if (!test_prim_pe (pe,PE_CLASS_UNIV,PE_PRIM_ENUM))
		return 0;
	x = (int *) smalloc (sizeof (int));
	*x = prim2enum(pe);
	return x;
}

static void intprint (PS ps, int *x, int format)
{
	ps_printf (ps,"%d",*x);
}

#define enumprint intprint

static int *intdup (int *x) {
	int *y;
	y = (int *) smalloc (sizeof (int));
	*y = *x;
	return (y);
}

#define enumdup intdup

static int intcmp (int *x, int *y) {
	return ( *x == *y ? 0 : (*x > *y ? 1 : -1) );
}

#define enumcmp intcmp

static void intfree (int *x) {
	free ((char *) x);
}

#define enumfree intfree

static int *intparse (char *str) {
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
						  NULLIFP,	FALSE);

	add_attribute_syntax("enumerated",
						 enumenc, 	enumdec,
						 enumparse,	enumprint,
						 enumdup,	enumcmp,
						 enumfree,		NULLCP,
						 NULLIFP,		FALSE);
}
