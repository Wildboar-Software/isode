/* lookup.c - password lookup service -- initiator */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "PasswordLookup-types.h"	/* type definitions */
#include "ryinitiator.h"		/* for generic interctive initiators */
#include "PasswordLookup-ops.h"		/* operation definitions */
static int do_lookupUser (int sd, struct dispatch *ds, char **args, struct type_PasswordLookup_UserName **arg);
static int do_lookupUID (int sd, struct dispatch *ds, char **args, struct type_PasswordLookup_UserID **arg);
static int  do_help (int sd, struct dispatch *ds, char **args, caddr_t *dummy);
static int lookup_result (int sd, int id, int dummy, struct type_PasswordLookup_Passwd *result, struct RoSAPindication *roi);
static print_qb (struct qbuf *q);
static int  lookup_error (int sd, int id, int error, caddr_t parameter, struct RoSAPindication *roi);


static char *myservice = "passwdstore";

static char *mycontext = "isode passwd lookup demo";
static char *mypci = "isode passwd lookup demo pci";

/* ARGUMENTS */
int	do_lookupUser (), do_lookupUID (), do_help (), do_quit (char **av);

/* RESULTS */
int	lookup_result ();

/* ERRORS */
int	lookup_error ();

static struct dispatch dispatches[] = {
	"lookupUser", operation_PasswordLookup_lookupUser,
	do_lookupUser, &_ZPasswordLookup_mod, _ZUserNamePasswordLookup,
	lookup_result, lookup_error,
	"find user by name",

	"lookupUID", operation_PasswordLookup_lookupUID,
	do_lookupUID, &_ZPasswordLookup_mod, _ZUserIDPasswordLookup,
	lookup_result, lookup_error,
	"find user by id",

	"help", 0,
	do_help, (modtyp *) 0, 0,
	NULLIFP, NULLIFP,
	"print this information",

	"quit", 0,
	do_quit, (modtyp *) 0, 0,
	NULLIFP, NULLIFP,
	"terminate the association and exit",

	NULL
};

int main (int argc, char **argv, char **envp) {
	ryinitiator (argc, argv, myservice, mycontext, mypci,
				 table_PasswordLookup_Operations, dispatches, do_quit);
	exit (0);			/* NOTREACHED */
}

/* ARGUMENTS */

static int do_lookupUser (int sd, struct dispatch *ds, char **args, struct type_PasswordLookup_UserName **arg) {
	char   *cp;

	if ((cp = *args++) == NULL) {
		advise (NULLCP, "usage: lookupUser username");
		return NOTOK;
	}
	if ((*arg = str2qb (cp, strlen (cp), 1)) == NULL)
		adios (NULLCP, "out of memory");
	return OK;
}

static int do_lookupUID (int sd, struct dispatch *ds, char **args, struct type_PasswordLookup_UserID **arg) {
	char   *cp;

	if ((cp = *args++) == NULL) {
		advise (NULLCP, "usage: lookupUID userid");
		return NOTOK;
	}
	if ((*arg = (struct type_PasswordLookup_UserID *) calloc (1, sizeof **arg))
			== NULL)
		adios (NULLCP, "out of memory");
	(*arg) -> parm = atoi (cp);
	return OK;
}

static int  do_help (int sd, struct dispatch *ds, char **args, caddr_t *dummy) {
	printf ("\nCommands are:\n");
	for (ds = dispatches; ds -> ds_name; ds++)
		printf ("%s\t%s\n", ds -> ds_name, ds -> ds_help);
	return NOTOK;
}

static int  do_quit (int sd, struct dispatch *ds, char **args, caddr_t *dummy) {
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

/* RESULTS */

static int lookup_result (int sd, int id, int dummy, struct type_PasswordLookup_Passwd *result, struct RoSAPindication *roi) {
	print_qb (result -> name);
	putchar (':');
	print_qb (result -> passwd);
	printf (":%d:%d:", result -> uid -> parm, result -> gid -> parm);
	print_qb (result -> gecos);
	putchar (':');
	print_qb (result -> dir);
	putchar (':');
	print_qb (result -> shell);
	putchar ('\n');
	return OK;
}

static print_qb (struct qbuf *q) {
	struct qbuf *p;

	if (q == NULL)
		return;
	for (p = q -> qb_forw; p != q; p = p -> qb_forw)
		printf ("%*.*s", p -> qb_len, p -> qb_len, p -> qb_data);
}

static int  lookup_error (int sd, int id, int error, caddr_t parameter, struct RoSAPindication *roi) {
	struct RyError *rye;

	if (error == RY_REJECT) {
		advise (NULLCP, "%s", RoErrString ((int) parameter));
		return OK;
	}
	if (rye = finderrbyerr (table_PasswordLookup_Errors, error))
		advise (NULLCP, "%s",  rye -> rye_name);
	else
		advise (NULLCP, "Error %d", error);
	return OK;
}
