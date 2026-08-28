-- testdebug.py - support routine for pepy generated routines
TESTDEBUG DEFINITIONS ::=

%{
#include <stdlib.h>
#include <stdio.h>
%}

BEGIN

END

%{
int	testdebug (PE pe, const char *s) {
    char  *cp;
    PS ps;
    static int debug = OK;
    
    switch (debug) {
	case NOTOK:
	    return -1;

	case OK:
	    if ((debug = (cp = getenv ("PEPYDEBUG")) && *cp ? atoi (cp)
							    : NOTOK) == NOTOK)
		return -1;
	    (void) fflush (stdout);
	    /* and fall... */

	default:
	    (void) fflush (stdout);
	    (void) fprintf (stderr, "%s\n", s);

	    if ((ps = ps_alloc (std_open)) == NULLPS)
		break;
	    if (std_setup (ps, stderr) != NOTOK)
		(void) pe2pl (ps, pe);
	    (void) fprintf (stderr, "--------\n");
	    ps_free (ps);
	    break;
    }
}

%}
