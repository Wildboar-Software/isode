-- mpp.py - test out PEPY

-- $Header: /xtel/isode/isode/pepy/RCS/mpp.py,v 9.0 1992/06/16 12:25:01 isode Rel $
--
--
-- $Log: mpp.py,v $
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


MPP DEFINITIONS   ::=

%{
#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/pepy/RCS/mpp.py,v 9.0 1992/06/16 12:25:01 isode Rel $";
#endif

#include <stdio.h>


#define	ps_advise(ps, f) \
	advise (NULLCP, "%s: %s", (f), ps_error ((ps) -> ps_errno))

/*    DATA */

static char *myname = "mpp";

static enum { ps2mpp, pl2mpp } mode = ps2mpp;

static enum format { p1, p2, sfd, ean, eanp2 } topfmt = ean;


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

    if (myname = rindex (argv[0], '/'))
	myname++;
    if (myname == NULL || *myname == NULL)
	myname = argv[0];

    for (argc--, argv++; cp = *argv; argc--, argv++)
	if (*cp == '-') {
	    if (strcmp (cp + 1, "ps") == 0) {
		mode = ps2mpp;
		continue;
	    }
	    if (strcmp (cp + 1, "pl") == 0) {
		mode = pl2mpp;
		continue;
	    }
	    if (strcmp (cp + 1, "p1") == 0) {
		topfmt = p1;
		continue;
	    }
	    if (strcmp (cp + 1, "p2") == 0) {
		topfmt = p2;
		continue;
	    }
	    if (strcmp (cp + 1, "sfd") == 0) {
		topfmt = sfd;
		continue;
	    }
	    if (strcmp (cp + 1, "ean") == 0) {
		topfmt = ean;
		continue;
	    }
	    adios (NULLCP, "usage: %s [ -ps | -pl ] [-p1 | -p2 | -sfd | -ean ] [ files... ]",
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
    enum format curfmt = topfmt;
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
	    case ps2mpp: 
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

	    case pl2mpp: 
		if ((pe = pl2pe (ps)) == NULLPE)
		    if (ps -> ps_errno) {
			ps_advise (ps, "pl2pe");
			goto you_lose;
		    }
		    else
			goto done;
		break;
	}

	switch (curfmt) {
	    case p1:
	    default:
		(void) print_P1_MPDU (pe, 1, NULLIP, NULLVP, NULLCP);
		break;

	    case p2:
		(void) print_P2_UAPDU (pe, 1, NULLIP, NULLVP, NULLCP);
		break;

	    case sfd:
		(void) print_SFD_Document (pe, 1, NULLIP, NULLVP, NULLCP);
		break;

	    case ean:
		(void) print_EAN_MPDU (pe, 1, NULLIP, NULLVP, NULLCP);
		curfmt = eanp2;
		break;

	    case eanp2:
		(void) print_EAN_UAPDU (pe, 1, NULLIP, NULLVP, NULLCP);
		curfmt = ean;
		break;
	}

	pe_free (pe);
    }
}

/*  */

%}

BEGIN

END

%{

/*    ERRORS */

#include <stdarg.h>


#ifndef	lint
static void	_advise ();


static void	adios (char *what, char *fmt, ...)
{
    va_list ap;

    va_start (ap, fmt);

    _advise (what, fmt, ap);

    va_end (ap);

    _exit (1);
}
#else
/* VARARGS */

void	adios (what, fmt)
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

    fprintf (stderr, "%s: ", myname);
    (void) fputs (buffer, stderr);
    (void) fputc ('\n', stderr);

    (void) fflush (stderr);
}
#endif

%}
