/* imisc.c - miscellaneous network service -- initiator */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/imisc/RCS/imisc.c,v 9.0 1992/06/16 12:23:43 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/imisc/RCS/imisc.c,v 9.0 1992/06/16 12:23:43 isode Rel $
 *
 *
 * $Log: imisc.c,v $
 * Revision 9.0  1992/06/16  12:23:43  isode
 * Release 8.0
 *
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */


#include <ctype.h>
#include <stdio.h>
#include <pwd.h>
#include "ryinitiator.h"	/* for generic interactive initiators */
#include "IMISC-ops.h"		/* IMISC operation definitions */
#include "IMISC-types.h"	/* IMISC type definitions */


#ifdef	SYS5
struct passwd *getpwuid ();
#endif

/*    DATA */

static char *myservice = "isode miscellany";/* should be something other
					       than mycontext */

static char *mycontext = "isode miscellany";
static char *mypci = "isode miscellany pci";


extern int length;
static type_IMISC_Data *data = NULLPE;

/* TYPES */
struct type_IMISC_IA5List *vec2ia5list ();


/* ARGUMENTS */
static int	do_finger (), do_tell (), do_data (), do_help (), do_quit ();


/* RESULTS */
#define	gentime_result	utctime_result

static int	utctime_result (), timeofday_result (), ia5_result (), tell_result (),
		null_result (), echo_result ();

/* ERRORS */
static int imisc_error ();


static struct dispatch dispatches[] = {
	"utctime",	operation_IMISC_utcTime,
	NULLIFP, NULL, 0,
	utctime_result, imisc_error,
	"the universal time",

	"gentime",	operation_IMISC_genTime,
	NULLIFP, NULL, 0,
	gentime_result, imisc_error,
	"the generalized time",

	"time",	operation_IMISC_timeOfDay,
	NULLIFP, NULL, 0,
	timeofday_result, imisc_error,
	"the current time since the epoch",

	"users",	operation_IMISC_users,
	NULLIFP, NULL, 0,
	ia5_result, imisc_error,
	"the users logged in on the system",

	"chargen",	operation_IMISC_charGen,
	NULLIFP, NULL, 0,
	ia5_result, imisc_error,
	"the character generation pattern",

	"qotd",	operation_IMISC_qotd,
	NULLIFP, NULL, 0,
	ia5_result, imisc_error,
	"the quote of the day",

	"finger",	operation_IMISC_finger,
	do_finger, &_ZIMISC_mod, _ZIA5ListIMISC,
	ia5_result, imisc_error,
	"the finger of users logged in",

	"pwdgen",	operation_IMISC_pwdGen,
	NULLIFP, NULL, 0,
	ia5_result, imisc_error,
	"some pseudo-randomly generated passwords",

	"tell", operation_IMISC_tellUser,
	do_tell, &_ZIMISC_mod, _ZIA5ListIMISC,
	tell_result, imisc_error,
	"send a message to a remote user",

	"ping", operation_IMISC_ping,
	NULLIFP, NULL, 0,
	null_result, imisc_error,
	"ping responder",

	"sink", operation_IMISC_sink,
	do_data, NULL, 0,
	null_result, imisc_error,
	"sink data",

	"echo", operation_IMISC_echo,
	do_data, NULL, 0,
	echo_result, imisc_error,
	"echo data",

	"help", 0,
	do_help, NULL, 0,
	NULLIFP, NULLIFP,
	"print this information",

	"quit", 0,
	do_quit, NULL, 0,
	NULLIFP, NULLIFP,
	"terminate the association and exit",

	NULL
};


char   *ctime ();

/*    MAIN */

/* ARGSUSED */

int
main (int argc, char **argv, char **envp) {
	ryinitiator (argc, argv, myservice, mycontext, mypci,
				 table_IMISC_Operations, dispatches, do_quit);

	exit (0);			/* NOTREACHED */
}

/*    TYPES */

struct type_IMISC_IA5List *
vec2ia5list (char **vec) {
	struct type_IMISC_IA5List  *ia5;
	struct type_IMISC_IA5List **ia5p;

	ia5 = NULL;
	ia5p = &ia5;

	for (; *vec; vec++) {
		if ((*ia5p = (struct type_IMISC_IA5List *) calloc (1, sizeof **ia5p))
				== NULL)
			adios (NULLCP, "out of memory");

		if (((*ia5p) -> IA5String = str2qb (*vec, strlen (*vec), 1)) == NULL)
			adios (NULLCP, "out of memory");

		ia5p = &((*ia5p) -> next);
	}

	return ia5;
}

/*  */

static
print_ia5list (struct type_IMISC_IA5List *ia5) {
	struct qbuf *p,
			   *q;

	for (; ia5; ia5 = ia5 -> next) {
		p = ia5 -> IA5String;
		for (q = p -> qb_forw; q != p ; q = q -> qb_forw)
			printf ("%*.*s", q -> qb_len, q -> qb_len, q -> qb_data);
		printf ("\n");
	}
}

/*    ARGUMENTS */

/* ARGSUSED */

static int
do_finger (int sd, struct dispatch *ds, char **args, struct type_IMISC_IA5List **ia5) {
	*ia5 = vec2ia5list (args);

	return OK;
}

/*  */

/* ARGSUSED */

static int
do_tell (int sd, struct dispatch *ds, char **args, struct type_IMISC_IA5List **ia5) {
	char   *cp,
		   *dp,
		   buffer[BUFSIZ];
	struct type_IMISC_IA5List  *ia52;
	struct passwd *pw;

	if (args[0] == NULL || args[1] == NULL) {
		advise (NULLCP, "usage: tell user message ...");
		return NOTOK;
	}

	*ia5 = vec2ia5list (args);

	cp = (pw = getpwuid (getuid ())) ? pw -> pw_name : "anon";
	dp = PLocalHostName ();

	if ((ia52 = (struct type_IMISC_IA5List *) calloc (1, sizeof *ia52))
			== NULL)
		adios (NULLCP, "out of memory");
	sprintf (buffer, "%s@%s", cp, dp);
	if ((ia52 -> IA5String = str2qb (buffer, strlen (buffer), 1)) == NULL)
		adios (NULLCP, "out of memory");

	/* kludge this arg onto front of list - HACK ATTACK */
	ia52 -> next = *ia5;
	*ia5 = ia52;

	return OK;
}

/*  */

/* ARGSUSED */

static int
do_data (int sd, struct dispatch *ds, char **args, struct type_IMISC_Data **pep) {
	char   *cp;

	if (data == NULLPE) {
		if (length > 0) {
			if ((cp = malloc ((unsigned) length)) == NULL)
				adios (NULLCP, "no memory");
		} else
			cp = NULL;
		if ((data = oct2prim (cp, length)) == NULLPE)
			adios (NULLCP, "no memory");
		if (cp)
			free (cp);
	}

	*pep = data;
	return OK;
}

/*  */

/* ARGSUSED */

static int
do_help (int sd, struct dispatch *ds, char **args, caddr_t *dummy) {
	printf ("\nCommands are:\n");
	for (ds = dispatches; ds -> ds_name; ds++)
		printf ("%s\t%s\n", ds -> ds_name, ds -> ds_help);

	return NOTOK;
}

/*  */

/* ARGSUSED */

static int
do_quit (int sd, struct dispatch *ds, char **args, caddr_t *dummy) {
	struct AcSAPrelease acrs;
	struct AcSAPrelease   *acr = &acrs;
	struct AcSAPindication  acis;
	struct AcSAPindication *aci = &acis;
	struct AcSAPabort *aca = &aci -> aci_abort;

	if (AcRelRequest (sd, ACF_NORMAL, NULLPEP, 0, NOTOK, acr, aci) == NOTOK)
		acs_adios (aca, "A-RELEASE.REQUEST");

	if (!acr -> acr_affirmative) {
		AcUAbortRequest (sd, NULLPEP, 0, aci);
		adios (NULLCP, "release rejected by peer: %d", acr -> acr_reason);
	}

	ACRFREE (acr);

	exit (0);
}

/*    RESULTS */

/* ARGSUSED */

static int
utctime_result (int sd, int id, int dummy, struct type_IMISC_UTCResult *result, struct RoSAPindication *roi) {
	struct qbuf *q;

	for (q = result -> qb_forw; q != result; q = q -> qb_forw)
		printf ("%*.*s", q -> qb_len, q -> qb_len, q -> qb_data);
	printf ("\n");

	return OK;
}

/*  */

/* ARGSUSED */

static int
timeofday_result (int sd, int id, int dummy, struct type_IMISC_TimeResult *result, struct RoSAPindication *roi) {
	long	s;

	s = result -> parm - 2208988800L;	/* UNIX epoch */
	printf ("%s", ctime (&s));

	return OK;
}

/*  */

/* ARGSUSED */

static int
ia5_result (int sd, int id, int dummy, struct type_IMISC_IA5List *result, struct RoSAPindication *roi) {
	print_ia5list (result);

	return OK;
}

/*  */

/* ARGSUSED */

static int
tell_result (int sd, int id, int dummy, caddr_t result, struct RoSAPindication *roi) {
	printf ("told.\n");

	return OK;
}

/*  */

/* ARGSUSED */

static int
null_result (int sd, int id, int dummy, caddr_t result, struct RoSAPindication *roi) {
	return OK;
}

/*  */

/* ARGSUSED */

static int
echo_result (int sd, int id, int dummy, struct type_IMISC_Data *result, struct RoSAPindication *roi) {
	if (pe_cmp (result, data))
		advise (NULLCP, "data mismatch");

	return OK;
}

/*    ERRORS */

/* ARGSUSED */

static int
imisc_error (int sd, int id, int error, struct type_IMISC_IA5List *parameter, struct RoSAPindication *roi) {
	struct RyError *rye;

	if (error == RY_REJECT) {
		advise (NULLCP, "%s", RoErrString ((ssize_t) parameter));
		return OK;
	}

	if (rye = finderrbyerr (table_IMISC_Errors, error))
		advise (NULLCP, "%s",  rye -> rye_name);
	else
		advise (NULLCP, "Error %d", error);

	if (parameter)
		print_ia5list (parameter);

	return OK;
}
