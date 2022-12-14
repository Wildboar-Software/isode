/* lookupd.c - password lookup service -- responder */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/others/lookup/RCS/lookupd.c,v 9.0 1992/06/16 12:42:23 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/others/lookup/RCS/lookupd.c,v 9.0 1992/06/16 12:42:23 isode Rel $
 *
 *
 * $Log: lookupd.c,v $
 * Revision 9.0  1992/06/16  12:42:23  isode
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


#include <stdio.h>
#include <pwd.h>
#include "ryresponder.h"	/* for generic idempotent responders */
#include "PasswordLookup-ops.h"		/* operation definitions */
#include "PasswordLookup-types.h"	/* type definitions */


#define	xalloc(p, type) \
	((p) = (type) calloc (1, sizeof *(p)))

#define	salloc(s) \
	str2qb ((s), strlen (s), 1)


#ifdef	SYS5
struct passwd *getpwnam (), *getpwuid ();
#endif

/*    DATA */

static char *myservice = "passwdstore";

static char *mycontext = "isode passwd lookup demo";


/* OPERATIONS */
int	op_lookupUser (), op_lookupUID ();

static struct dispatch dispatches[] = {
	"lookupUser", operation_PasswordLookup_lookupUser, op_lookupUser,

	"lookupUID", operation_PasswordLookup_lookupUID, op_lookupUID,

	NULL
};

/*    MAIN */

/* ARGSUSED */

int
main (int argc, char **argv, char **envp) {
	ryresponder (argc, argv, PLocalHostName (), myservice, mycontext,
				 dispatches, table_PasswordLookup_Operations,
				 NULLIFP, NULLIFP);

	exit (0);			/* NOTREACHED */
}

/*    OPERATIONS */

static int  op_lookupUser (sd, ryo, rox, in, roi)
int	sd;
struct RyOperation *ryo;
struct RoSAPinvoke *rox;
caddr_t	in;
struct RoSAPindication *roi;
{
	int     result;
	char   *cp;
	struct type_PasswordLookup_UserName   *arg =
		(struct type_PasswordLookup_UserName   *) in;

	if (rox -> rox_nolinked == 0) {
		advise (LLOG_EXCEPTIONS, NULLCP,
				"RO-INVOKE.INDICATION/%d: %s, unknown linkage %d",
				sd, ryo -> ryo_name, rox -> rox_linkid);
		return ureject (sd, ROS_IP_LINKED, rox, roi);
	}
	if (debug)
		advise (LLOG_DEBUG, NULLCP, "RO-INVOKE.INDICATION/%d: %s",
				sd, ryo -> ryo_name);

	if ((cp = qb2str (arg)) == NULL)
		result = error (sd, error_PasswordLookup_congested, (caddr_t) NULL,
						rox, roi);
	else {
		result = lookup (sd, getpwnam (cp), rox, roi);
		free (cp);
	}

	return result;
}

/*  */

static int  op_lookupUID (sd, ryo, rox, in, roi)
int	sd;
struct RyOperation *ryo;
struct RoSAPinvoke *rox;
caddr_t	in;
struct RoSAPindication *roi;
{
	struct type_PasswordLookup_UserID   *arg =
		(struct type_PasswordLookup_UserID   *) in;

	if (rox -> rox_nolinked == 0) {
		advise (LLOG_EXCEPTIONS, NULLCP,
				"RO-INVOKE.INDICATION/%d: %s, unknown linkage %d",
				sd, ryo -> ryo_name, rox -> rox_linkid);
		return ureject (sd, ROS_IP_LINKED, rox, roi);
	}
	if (debug)
		advise (LLOG_DEBUG, NULLCP, "RO-INVOKE.INDICATION/%d: %s",
				sd, ryo -> ryo_name);

	return lookup (sd, getpwuid (arg -> parm), rox, roi);
}

/*  */

static int
lookup (int sd, struct passwd *pw, struct RoSAPinvoke *rox, struct RoSAPindication *roi) {
	int	    result;

	if (pw) {
		struct type_PasswordLookup_Passwd *res = NULL;

		if (xalloc (res, struct type_PasswordLookup_Passwd *) == NULL
				|| (res -> name = salloc (pw -> pw_name)) == NULL
				|| (*pw -> pw_passwd
					&& (res -> passwd = salloc (pw -> pw_passwd)) == NULL)
				|| xalloc (res -> uid, struct type_PasswordLookup_UserID *)
				== NULL
				|| xalloc (res -> gid, struct type_PasswordLookup_GroupID *)
				== NULL
#if !defined(_AIX) && !defined(__linux__)
				|| (pw -> pw_comment
					&& (res -> comment = salloc (pw -> pw_comment))
					== NULL)
#endif
				|| (pw -> pw_gecos
					&& (res -> gecos = salloc (pw -> pw_gecos)) == NULL)
				|| (pw -> pw_dir
					&& (res -> dir = salloc (pw -> pw_dir)) == NULL)
				|| (pw -> pw_shell
					&& (res -> shell = salloc (pw -> pw_shell)) == NULL))
			result = error (sd, error_PasswordLookup_congested, (caddr_t) NULL,
							rox, roi);
		else {
			res -> uid -> parm = pw -> pw_uid;
			res -> gid -> parm = pw -> pw_gid;
			/*
				    res -> quota = pw -> pw_quota;
			 */

			if (RyDsResult (sd, rox -> rox_id, (caddr_t) res, ROS_NOPRIO, roi)
					== NOTOK)
				ros_adios (&roi -> roi_preject, "RESULT");
			result = OK;
		}

		free_PasswordLookup_Passwd (res);
	} else
		result = error (sd, error_PasswordLookup_noSuchUser, (caddr_t) NULL,
						rox, roi);

	return result;
}

/*    ERROR */

static int  error (sd, err, param, rox, roi)
int	sd,
	err;
caddr_t	param;
struct RoSAPinvoke *rox;
struct RoSAPindication *roi;
{
	if (RyDsError (sd, rox -> rox_id, err, param, ROS_NOPRIO, roi) == NOTOK)
		ros_adios (&roi -> roi_preject, "ERROR");

	return OK;
}

/*    U-REJECT */

static int
ureject (int sd, int reason, struct RoSAPinvoke *rox, struct RoSAPindication *roi) {
	if (RyDsUReject (sd, rox -> rox_id, reason, ROS_NOPRIO, roi) == NOTOK)
		ros_adios (&roi -> roi_preject, "U-REJECT");

	return OK;
}
