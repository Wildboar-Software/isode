/* imisc.c - miscellaneous network service -- initiator */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pwd.h>
#include "ryinitiator.h"	/* for generic interactive initiators */
#include "IMISC-ops.h"		/* IMISC operation definitions */
#include "IMISC-types.h"	/* IMISC type definitions */

#ifdef	SYS5
struct passwd *getpwuid (uid_t uid);
#endif

static struct type_IMISC_IA5List *vec2ia5list (char **vec);
static void print_ia5list (const struct type_IMISC_IA5List *ia5);
static int do_finger (int sd, struct dispatch *ds, char **args, void *parameter);
static int do_tell (int sd, struct dispatch *ds, char **args, void *parameter);
static int do_data (int sd, struct dispatch *ds, char **args, void *parameter);
static int do_help (int sd, struct dispatch *ds, char **args, void *parameter);
static int do_quit (int sd, struct dispatch *ds, char **args, void *parameter);
static int utctime_result (int sd, int id, int dummy, caddr_t res, struct RoSAPindication *roi);
static int timeofday_result (int sd, int id, int dummy, caddr_t res, struct RoSAPindication *roi);
static int ia5_result (int sd, int id, int dummy, caddr_t res, struct RoSAPindication *roi);
static int tell_result (int sd, int id, int dummy, caddr_t res, struct RoSAPindication *roi);
static int null_result (int sd, int id, int dummy, caddr_t result, struct RoSAPindication *roi);
static int echo_result (int sd, int id, int dummy, caddr_t res, struct RoSAPindication *roi);
static void imisc_error (int sd, int id, int error, caddr_t p, struct RoSAPindication *roi);

#define	gentime_result	utctime_result

static char *myservice = "isode miscellany";/* should be something other
					       than mycontext */

static char *mycontext = "isode miscellany";
static char *mypci = "isode miscellany pci";

extern int length;
static type_IMISC_Data *data = NULLPE;

static struct dispatch dispatches[] = {
	"utctime",	operation_IMISC_utcTime,
	NULL, NULL, 0,
	(ds_result_t)utctime_result, imisc_error,
	"the universal time",

	"gentime",	operation_IMISC_genTime,
	NULL, NULL, 0,
	(ds_result_t)gentime_result, imisc_error,
	"the generalized time",

	"time",	operation_IMISC_timeOfDay,
	NULL, NULL, 0,
	(ds_result_t)timeofday_result, imisc_error,
	"the current time since the epoch",

	"users",	operation_IMISC_users,
	NULL, NULL, 0,
	(ds_result_t)ia5_result, imisc_error,
	"the users logged in on the system",

	"chargen",	operation_IMISC_charGen,
	NULL, NULL, 0,
	(ds_result_t)ia5_result, imisc_error,
	"the character generation pattern",

	"qotd",	operation_IMISC_qotd,
	NULL, NULL, 0,
	(ds_result_t)ia5_result, imisc_error,
	"the quote of the day",

	"finger",	operation_IMISC_finger,
	(ds_argument_t)do_finger, &_ZIMISC_mod, _ZIA5ListIMISC,
	(ds_result_t)ia5_result, imisc_error,
	"the finger of users logged in",

	"pwdgen",	operation_IMISC_pwdGen,
	NULL, NULL, 0,
	(ds_result_t)ia5_result, imisc_error,
	"some pseudo-randomly generated passwords",

	"tell", operation_IMISC_tellUser,
	(ds_argument_t)do_tell, &_ZIMISC_mod, _ZIA5ListIMISC,
	(ds_result_t)tell_result, imisc_error,
	"send a message to a remote user",

	"ping", operation_IMISC_ping,
	NULL, NULL, 0,
	(ds_result_t)null_result, imisc_error,
	"ping responder",

	"sink", operation_IMISC_sink,
	(ds_argument_t)do_data, NULL, 0,
	(ds_result_t)null_result, imisc_error,
	"sink data",

	"echo", operation_IMISC_echo,
	(ds_argument_t)do_data, NULL, 0,
	echo_result, imisc_error,
	"echo data",

	"help", 0,
	(ds_argument_t)do_help, NULL, 0,
	NULL, NULL,
	"print this information",

	"quit", 0,
	(ds_argument_t)do_quit, NULL, 0,
	NULL, NULL,
	"terminate the association and exit",

	NULL
};


int main (int argc, char **argv, char **envp) {
	ryinitiator (argc, argv, myservice, mycontext, mypci,
				 table_IMISC_Operations, dispatches, (ds_argument_t)do_quit);
	exit (0);			/* NOTREACHED */
}

static struct type_IMISC_IA5List *vec2ia5list (char **vec) {
	struct type_IMISC_IA5List  *ia5;
	struct type_IMISC_IA5List **ia5p;

	ia5 = NULL;
	ia5p = &ia5;
	for (; *vec; vec++) {
		if ((*ia5p = (struct type_IMISC_IA5List *) calloc (1, sizeof **ia5p))
				== NULL)
			adios (NULLCP, "out of memory");
		if (((*ia5p) -> IA5String = str2qb_s (*vec)) == NULL)
			adios (NULLCP, "out of memory");
		ia5p = &((*ia5p) -> next);
	}
	return ia5;
}

static void print_ia5list (const struct type_IMISC_IA5List *ia5) {
	struct qbuf *p, *q;

	for (; ia5; ia5 = ia5 -> next) {
		p = ia5 -> IA5String;
		for (q = p -> qb_forw; q != p ; q = q -> qb_forw)
			printf ("%*.*s", q -> qb_len, q -> qb_len, q -> qb_data);
		printf ("\n");
	}
}

static int do_finger (int sd, struct dispatch *ds, char **args, void *parameter) {
	struct type_IMISC_IA5List **ia5 = (struct type_IMISC_IA5List **) parameter;
	*ia5 = vec2ia5list (args);
	return OK;
}

static int do_tell (int sd, struct dispatch *ds, char **args, void *parameter) {
	struct type_IMISC_IA5List **ia5 = (struct type_IMISC_IA5List **) parameter;
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
	if ((ia52 -> IA5String = str2qb_s (buffer)) == NULL)
		adios (NULLCP, "out of memory");

	/* kludge this arg onto front of list - HACK ATTACK */
	ia52 -> next = *ia5;
	*ia5 = ia52;
	return OK;
}

static int do_data (int sd, struct dispatch *ds, char **args, void *parameter) {
	struct type_IMISC_Data **pep = (struct type_IMISC_Data **) parameter;
	char   *cp;
	if (data == NULLPE) {
		if (length > 0) {
			if ((cp = malloc ((unsigned) length)) == NULL)
				adios (NULLCP, "no memory");
		} else
			cp = 0;
		if ((data = oct2prim (cp, length)) == NULLPE)
			adios (NULLCP, "no memory");
		if (cp)
			free (cp);
	}
	*pep = data;
	return OK;
}

static int do_help (int sd, struct dispatch *ds, char **args, void *parameter) {
	printf ("\nCommands are:\n");
	for (ds = dispatches; ds -> ds_name; ds++)
		printf ("%s\t%s\n", ds -> ds_name, ds -> ds_help);
	return NOTOK;
}

static int do_quit (int sd, struct dispatch *ds, char **args, void *parameter) {
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

static int utctime_result (int sd, int id, int dummy, caddr_t res, struct RoSAPindication *roi) {
	struct type_IMISC_UTCResult *result = (struct type_IMISC_UTCResult *) res;
	struct qbuf *q;
	for (q = result -> qb_forw; q != result; q = q -> qb_forw)
		printf ("%*.*s", q -> qb_len, q -> qb_len, q -> qb_data);
	printf ("\n");
	return OK;
}

static int timeofday_result (int sd, int id, int dummy, caddr_t p, struct RoSAPindication *roi) {
	struct type_IMISC_TimeResult *result = (struct type_IMISC_TimeResult *) p;
	long	s;
	s = result -> parm - 2208988800L;	/* UNIX epoch */
	printf ("%s", ctime (&s));
	return OK;
}

static int ia5_result (int sd, int id, int dummy, caddr_t p, struct RoSAPindication *roi) {
	struct type_IMISC_IA5List *result = (struct type_IMISC_IA5List *) p;
	print_ia5list (result);
	return OK;
}

static int tell_result (int sd, int id, int dummy, caddr_t result, struct RoSAPindication *roi) {
	printf ("told.\n");
	return OK;
}

static int null_result (int sd, int id, int dummy, caddr_t result, struct RoSAPindication *roi) {
	return OK;
}

static int echo_result (int sd, int id, int dummy, caddr_t res, struct RoSAPindication *roi) {
	struct type_IMISC_Data *result = (struct type_IMISC_Data *) res;
	if (pe_cmp (result, data))
		advise (NULLCP, "data mismatch");
	return OK;
}

static void imisc_error (int sd, int id, int error, caddr_t p, struct RoSAPindication *roi) {
	struct type_IMISC_IA5List *parameter = (struct type_IMISC_IA5List *) p;
	struct RyError *rye;

	if (error == RY_REJECT) {
		int code;

		if (ssize2int ((ssize_t) parameter, &code) != 0)
			advise (NULLCP, "reject code too large");
		else
			advise (NULLCP, "%s", RoErrString (code));
		return;
	}
	if (rye = finderrbyerr (table_IMISC_Errors, error))
		advise (NULLCP, "%s",  rye -> rye_name);
	else
		advise (NULLCP, "Error %d", error);
	if (parameter)
		print_ia5list (parameter);
}
