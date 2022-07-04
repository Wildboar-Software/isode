SalaryDefs DEFINITIONS ::=

%{
#include <stdio.h>

#define PEPYPARM struct salary_record *


static char *myname = "salary";

static struct salary_record {
    char   *name;
    int     salary;
}                           salary;


static void	adios (char *, char *, ...);

/*  */

/* ARGSUSED */

main (argc, argv, envp)
int     argc;
char  **argv,
      **envp;
{
    PE	    pe;

    myname = argv[0];

    if (argc != 3)
	adios (NULLCP, "usage: %s name salary", myname);
    salary.name = argv[1];
    salary.salary = atoi (argv[2]);

    if (build_SalaryDefs_Salary (&pe, 1, NULL, NULLCP, &salary) == NOTOK)
	adios (NULLCP, "encoder fails");

    salary.name = NULL;
    salary.salary = 0;

    if (unbuild_SalaryDefs_Salary (pe, 1, NULLIP, NULLVP, &salary) == NOTOK)
	adios (NULLCP, "decoder fails");

    exit (0);			/* NOTREACHED */
}
%}

BEGIN

SECTIONS build unbuild none

Salary ::=
	SEQUENCE {
	    name
		PrintableString [[s parm -> name]]
		%{ (void) printf("name %s ", parm -> name); %},

	    salary
		TheSalary [[i parm -> salary ]]
		%{ (void) printf("salary %d\n", parm -> salary); %}
	}

TheSalary ::=
	INTEGER

END

%{

/*    ERRORS */

#include <stdarg.h>


#ifndef	lint
static void	_advise ();


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
