-- pepytest.py - test out PEPY

-- $Header: /xtel/isode/isode/pepy/RCS/pepytest.py,v 9.0 1992/06/16 12:25:01 isode Rel $
--
--
-- $Log: pepytest.py,v $
-- Revision 9.0  1992/06/16  12:25:01  isode
-- Release 8.0
--
--

--
--				  NOTICE
--
--    Acquisition, use, and distribution of this module and related
--    materials are subject to the restrictions of a license agreement.
--    Consult the Preface in the User's Manual for the full terms of
--    this agreement.
--
--


PEPYTEST DEFINITIONS   ::=

%{
#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/pepy/RCS/pepytest.py,v 9.0 1992/06/16 12:25:01 isode Rel $";
#endif

#include <stdio.h>

/*    DATA */

#define	ps_advise(ps, f) \
	advise (NULLCP, "%s: %s", (f), ps_error ((ps) -> ps_errno))


static char *myname = "pepytest";

static enum { ps2test, pl2test } mode = ps2test;

static int  process ();

static void	adios (char *, char *, ...);
void	advise (char *, char *, ...);

/*    MAIN */

/* ARGSUSED */

main (argc, argv, envp)
int	argc;
char  **argv,
      **envp;
{
    register int    status = 0;
    register char  *cp;
    register FILE  *fp;

    myname = *argv;
    for (argc--, argv++; cp = *argv; argc--, argv++)
	if (*cp == '-') {
	    if (strcmp (cp + 1, "ps") == 0) {
		mode = ps2test;
		continue;
	    }
	    if (strcmp (cp + 1, "pl") == 0) {
		mode = pl2test;
		continue;
	    }
	    adios (NULLCP, "usage: %s [ -ps | -pl ] [ files... ]",
		    myname);
	}
	else
	    break;

    if (argc == 0)
	status = process ("(stdin)", stdin);
    else
	while (cp = *argv++) {
	    if ((fp = fopen (cp, "r")) == NULL) {
		advise (cp, "unable to read");
		status++;
		continue;
	    }
	    status += process (cp, fp);
	    (void) fclose (fp);
	}

    exit (status);		/* NOTREACHED */
}

/*  */

static int  process (file, fp)
register char *file;
register FILE *fp;
{
    register PE	    pe;
    register PS	    ps;

    if ((ps = ps_alloc (std_open)) == NULLPS) {
	ps_advise (ps, "ps_alloc");
	return 1;
    }
    if (std_setup (ps, fp) == NOTOK) {
	advise (NULLCP, "%s: std_setup loses", file);
	return 1;
    }

    for (;;) {
	switch (mode) {
	    case ps2test: 
		if ((pe = ps2pe (ps)) == NULLPE)
		    if (ps -> ps_errno) {
			ps_advise (ps, "ps2pe");
		you_lose: ;
			ps_free (ps);
			return 1;
		    }
		    else {
		done: 	;
			ps_free (ps);
			return 0;
		    }
		break;

	    case pl2test: 
		if ((pe = pl2pe (ps)) == NULLPE)
		    if (ps -> ps_errno) {
			ps_advise (ps, "pl2pe");
			goto you_lose;
		    }
		    else
			goto done;
		break;
	}

	if (parse_PEPYTEST_PersonnelRecord (pe, 1, NULLIP, NULLVP, NULLCP)
	        == NOTOK)
	    advise (NULLCP, "parse error: %s", PY_pepy);
	else
	    (void) print_PEPYTEST_PersonnelRecord (pe, 1, NULLIP, NULLVP,
						   NULLCP);

	pe_free (pe);
    }
}

/*  */

%}

BEGIN

SECTIONS none parse print

PersonnelRecord
	::=
	[APPLICATION 0]
	    IMPLICIT SET {
		    Name,

		title[0]
		    VisibleString,

		number
		    EmployeeNumber,

		dateOfHire[1]
		    Date,

		nameOfSpouse[2]
		    Name,

		children[3]
		    IMPLICIT SEQUENCE OF
			ChildInformation
    		    DEFAULT {}
	    }


ChildInformation ::=
	SET {
		Name,

	    dateofBirth[0]
		Date
	}


Name ::=
	[APPLICATION 1]
	    IMPLICIT SEQUENCE {
		givenName
		    VisibleString,

		initial
		    VisibleString,

		familyName
		    VisibleString
	    }


EmployeeNumber ::=
	[APPLICATION 2]
	    IMPLICIT INTEGER


Date ::=
	[APPLICATION 3]
	    IMPLICIT VisibleString -- YYYYMMDD

END

%{

/*    ERRORS */

#include <stdarg.h>


#ifndef	lint
static void	_advise (char *, char *, va_list);


static void  adios (char *what, char *fmt, ...)
{
    va_list ap;

    va_start (ap, fmt);

    _advise (what, fmt, ap);

    va_end (ap);

    _exit (1);
}
#else
/* VARARGS */

static void  adios (what, fmt)
char   *what,
       *fmt;
{
    adios (what, fmt);
}
#endif


#ifndef	lint
static void  _advise (char *what, char *fmt, va_list ap)
{
    char    buffer[BUFSIZ];

    _asprintf (buffer, what, fmt, ap);

    (void) fflush (stdout);

    (void) fprintf (stderr, "%s: ", myname);
    (void) fputs (buffer, stderr);
    (void) fputc ('\n', stderr);

    (void) fflush (stderr);

    va_end (ap);
}
#endif

%}
