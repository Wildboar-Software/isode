HelloWorldDefs DEFINITIONS ::=

%{
#include <stdio.h>


static char *text;
static char *myname = "hello_world";


static void adios (char *, char *, ...);
void advise (char *, char *, ...);

/*  */

/* ARGSUSED */

main (argc, argv, envp)
int     argc;
char  **argv,
      **envp;
{
    PE	    pe;

    myname = argv[0];

    if (build_HelloWorldDefs_MyStruct (&pe, 1, NULL, NULLCP, NULLCP) == NOTOK)
	adios (NULLCP, "encoder fails");
    if (unbuild_HelloWorldDefs_MyStruct (pe, 1, NULLIP, NULLVP, NULLCP)
	    == NOTOK)
	adios (NULLCP, "decoder fails");

    exit (0);			/* NOTREACHED */
}
%}

BEGIN

ENCODER	build

MyStruct ::=
	PrintableString [[s "Hello World" ]]


DECODER	unbuild

MyStruct ::=
	PrintableString [[s text]]
	%{ (void) printf("%s\n", text); %}

END

%{

/*    ERRORS */

#include <stdarg.h>


#ifndef	lint
void	_advise ();


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
}
#endif

%}
