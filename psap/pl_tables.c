/* pl_tables.c - tables for presentation lists */

#include <stdio.h>
#include "psap.h"

char   *pe_classlist[] = {
	"UNIV",
	"APPL",
	"CONT",
	"PRIV"
};

int	pe_maxclass = sizeof pe_classlist / sizeof pe_classlist[0];

char   *pe_univlist[] = {
	"EOC",
	"BOOL",
	"INT",
	"BITS",
	"OCTS",
	"NULL",
	"OID",
	"ODE",
	"EXTN",
	"REAL",
	"ENUM",
	"ENCR",
	NULLCP,
	NULLCP,
	NULLCP,
	NULLCP,
	"SEQ",
	"SET",
	"NUMS",
	"PRTS",
	"T61S",
	"VTXS",
	"IA5S",
	"UTCT",
	"GENT",
	"GFXS",
	"VISS",
	"GENS",
	"CHRS",
};

int	pe_maxuniv = sizeof pe_univlist / sizeof pe_univlist[0];

int	pe_maxappl = 0;
char  **pe_applist = NULL;

int	pe_maxpriv = 0;
char  **pe_privlist = NULL;
