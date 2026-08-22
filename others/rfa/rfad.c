/*
 * RFA - Remote File Access
 *
 * Access and Management for a partial file system tree that exists
 * at two sites either as master files or slave files
 *
 * rfad.c : responder for RFA commands
 *
 * Contributed by Oliver Wenzel, GMD Berlin, 1990
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <crypt.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "RFA-types.h"	/* type definitions */
#include "RFA-ops.h"	    /* operation definitions */
#include "ryresponder.h"
#include "tsap.h"
#include "rfa.h"
#include "vt.h"

int initiate (int sd, struct AcSAPstart *acs, PE *pe);
int init_lose (int type, PE *pe, char *str);


static char *myservice = "rfa";
static char *mycontext = "rfa";

extern	struct type_RFA_QueryResult *query ();
extern	struct type_RFA_FileList *do_listcdir ();
extern int op_init ();
extern int ros_init (int vecp, char **vec), ros_work (int fd), ros_indication (int sd, struct RoSAPindication *roi), ros_lose (struct TSAPdisconnect *td);
extern IFP startfnx, stopfnx;
extern char *isodetcpath;

extern int op_getFileData(int sd, struct RyOperation *ryo, struct RoSAPinvoke *rox, caddr_t in, struct RoSAPindication *roi), op_requestMaster(int sd, struct RyOperation *ryo, struct RoSAPinvoke *rox, caddr_t in, struct RoSAPindication *roi), op_listDir(int sd, struct RyOperation *ryo, struct RoSAPinvoke *rox, caddr_t in, struct RoSAPindication *roi), op_syncTime(int sd, struct RyOperation *ryo, struct RoSAPinvoke *rox, caddr_t in, struct RoSAPindication *roi);
static struct dispatch dispatches[] = {
	"getFileData",  operation_RFA_getFileData, op_getFileData,
	"requestMaster",	    operation_RFA_requestMaster, op_requestMaster,
	"listDir",	    operation_RFA_listDir, op_listDir,
	"syncTime",	    operation_RFA_syncTime, op_syncTime,
	NULL
};

char target[BUFSIZ];
char	*host;
int groupid, userid;
char homedir[BUFSIZ];

/* MAIN */

int main (int argc, char **argv) {
	int initiate (int sd, struct AcSAPstart *acs, PE *pe);
	char buf[BUFSIZ];
	struct dispatch   *ds;
	AEI	    aei;
	struct TSAPdisconnect   tds;
	struct TSAPdisconnect  *td = &tds;
	struct RoSAPindication  rois;
	struct RoSAPindication *roi = &rois;
	struct RoSAPpreject   *rop = &roi -> roi_preject;
	char *myname;

	host = getlocalhost ();
	if (myname = rindex (argv[0], '/'))
		myname++;
	else
		myname = argv[0];
	/*--- isode initialization and tailoring ---*/
	sprintf(buf,"HOME=%s", RFA_TAILDIR);
	putenv(buf);
	isodetailor (myname, 1);
	initLog(myname);
	/*--- rfa tailoring ---*/
	sprintf(buf, "%s/rfatailor", isodetcpath);
	if (tailor(buf) != OK)
		advise (LLOG_EXCEPTIONS, NULLCP, rfaErrStr);
	sprintf(buf, "%s/rfatailor", RFA_TAILDIR);
	if (tailor(buf) != OK)
		advise (LLOG_EXCEPTIONS, NULLCP, rfaErrStr);
	/*--- get application entity identifier for rfa service ---*/
	if ((aei = _str2aei (host, myservice,mycontext,0,NULLCP,NULLCP)) == NULLAEI)
		adios (NULLCP, "%s-%s: unknown application-entity",
			   host, myservice);
	/*--- register operation to serve ---*/
	advise (LLOG_EXCEPTIONS, NULLCP, "doing");
	for (ds = dispatches; ds -> ds_name; ds++)
		if (RyDispatch (NOTOK, table_RFA_Operations, ds -> ds_operation,
						ds -> ds_vector, roi) == NOTOK)
			ros_adios (rop, ds -> ds_name);
	advise (LLOG_EXCEPTIONS, NULLCP, "done");
	startfnx = initiate;
	stopfnx = NULLIFP;
	advise (LLOG_NOTICE, NULLCP, "starting");
	if (isodeserver (argc, argv, aei, ros_init, ros_work, ros_lose, td)
			== NOTOK) {
		if (td -> td_cc > 0)
			adios (NULLCP, "isodeserver [%s] %*.*s",
				   TErrString (td -> td_reason),
				   td -> td_cc, td -> td_cc, td -> td_data);
		else
			adios (NULLCP, "isodeserver: [%s]",
				   TErrString (td -> td_reason));
	}
	exit(0);
}

int cleanup (void) {
}

int initiate (int sd, struct AcSAPstart *acs, PE *pe) {
	struct type_RFA_Initiate *initial;
	char    *cp, *crypt ();
	struct passwd *pw;

	*pe	 = NULLPE;
	if ( acs -> acs_ninfo != 1)
		return init_lose (ACS_PERMANENT, pe, "No Bind data");
	if (decode_RFA_Initiate (acs -> acs_info[0], 1, NULL, NULLVP,
							 &initial) == NOTOK)
		return init_lose (ACS_PERMANENT, pe, "Can't parse Bind data");
	user = qb2str (initial -> user);
	advise (LLOG_NOTICE, NULLCP, "Bind of user %s", user);
	if (baduser (NULLCP, user)) {
		advise (LLOG_EXCEPTIONS, NULLCP, "Bad listed user '%s'", user);
		return init_lose (ACS_PERMANENT, pe, "Bad user/password");
	}
	if ((pw = getpwnam (user)) == NULL) {
		advise (LLOG_EXCEPTIONS, NULLCP, "Unknown user '%s'", user);
		return init_lose (ACS_PERMANENT, pe, "Bad user/password");
	}
	userid = pw -> pw_uid;
	groupid = pw -> pw_gid;
	strcpy (homedir, pw -> pw_dir);
	cp = qb2str (initial -> password);
	if (pw -> pw_passwd == NULL
			|| strcmp (crypt (cp, pw -> pw_passwd), pw -> pw_passwd) != 0) {
		advise (LLOG_NOTICE, NULLCP, "Password mismatch for %s", user);
		return init_lose (ACS_PERMANENT, pe, "Bad user/password");
	}
	bzero (cp, strlen(cp)); /* in case of cores */
	free (cp);
	free_RFA_Initiate (initial);
	if (chdir (homedir) == -1) {
		advise (LLOG_NOTICE, NULLCP, "Can't set home directory to '%s'",
				homedir);
		return init_lose (ACS_PERMANENT, pe, "No home directory");
	}
	if (initUserId(userid, groupid, user) != OK) {
		advise (LLOG_NOTICE, NULLCP, "%s\n", rfaErrStr);
		return init_lose (ACS_PERMANENT, pe, rfaErrStr);
	}
	return ACS_ACCEPT;
}

int init_lose (int type, PE *pe, char *str) {
	*pe = ia5s2prim (str, strlen(str));
	(*pe) -> pe_context = 3;	/* magic!! - don't ask me why */
	return type;
}
