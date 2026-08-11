-- testdebug.py - support routine for pepy generated routines

-- 
--
--
-- 
--
--
--
--

TESTDEBUG DEFINITIONS ::=

%{
#ifndef	lint
static char *rcsid = "";
#endif

#include <stdio.h>

%}

BEGIN

END

%{
int	testdebug (pe, s)
register PE	pe;
register char	*s;
{
    char  *cp;
    register PS ps;
    static int debug = OK;
    
    switch (debug) {
	case NOTOK:
	    return -1;

	case OK:
	    if ((debug = (cp = getenv ("PEPYDEBUG")) && *cp ? atoi (cp)
							    : NOTOK) == NOTOK)
		return -1;
	    (void) fflush (stdout);
#ifndef lint
	    (void) fprintf (stderr, "testdebug made with %s\n", pepyid);
#endif
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
