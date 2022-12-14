/*
 * RFA - Remote File Access
 *
 * Access and Management for a partial file system tree that exists
 * at two sites either as master files or slave files
 *
 * synctime.c : SyncTime operation
 *
 * Contributed by Oliver Wenzel, GMD Berlin, 1990
 *
 * $Header: /xtel/isode/isode/others/rfa/RCS/synctime.c,v 9.0 1992/06/16 12:47:25 isode Rel $
 *
 * $Log: synctime.c,v $
 * Revision 9.0  1992/06/16  12:47:25  isode
 * Release 8.0
 *
 */

#ifndef       lint
static char *rcsid = "$Header: /xtel/isode/isode/others/rfa/RCS/synctime.c,v 9.0 1992/06/16 12:47:25 isode Rel $";
#endif

/*
 *                              NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

#include <stdio.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>
#include "RFA-ops.h"        /* operation definitions */
#include "RFA-types.h"  /* type definitions */
#include "ryresponder.h"
#include "psap.h"
#include "rfa.h"
#include "rfainfo.h"

extern char *isodesbinpath;

/*--------------------------------------------------------------
 *  changeTimeWithRfatime - change time by executing external
 *			    rfatime command as root
 *-------------------------------------------------------------*/
int
changeTimeWithRfatime (long dt) {
	char buf[BUFSIZ];
	char dtbuf[BUFSIZ];
	int p[2];


	if (pipe(p) == -1) {
		sprintf(rfaErrStr, "can't set remote time (%s)",sys_errname(errno));
		return NOTOK;
	}

	switch (fork()) {
	case NOTOK:
		sprintf(rfaErrStr, "can't set remote time (%s)",sys_errname(errno));
		return NOTOK;

	case 0: /* child */
		close(2);
		dup2(p[1], 2);
		sprintf(buf, "%s/rfatime", isodesbinpath, dt);
		sprintf(dtbuf,"%ld", dt);
		advise (LLOG_NOTICE, NULLCP, "syncTime: start %s", buf);

		execl(buf, buf, dtbuf, NULL);

		fprintf(stderr, "can't set remote time (%s)",sys_errname(errno));
		advise (LLOG_NOTICE, NULLCP, "syncTime: exec failed: %s", buf);
		exit(-2);

	default:
		break;
	}

	/*-------- does not work, wait always returns error "no children"
	st=0;
	if ((rc = wait(&st)) == -1) {
	sprintf(rfaErrStr, "can't set remote time (%s)",sys_errname(errno));
	advise (LLOG_NOTICE, NULLCP, "syncTime: wait failed %s" sys_errname(errno));
	return NOTOK;
	}
	rc = WEXITSTATUS(st) >>= 8;
	if (rc > 0)
	sprintf(buf, "can't set time (%s)", sys_errname(rc));
	else
	sprintf(buf, "can't set remote time");
	advise (LLOG_NOTICE, NULLCP, "syncTime: start failed %x", rc);
	-------------------*/

	close(p[1]);
	if (read(p[0], buf, sizeof buf)) {
		*(buf+strlen(buf)-1) = '\0';
		advise (LLOG_EXCEPTIONS, NULLCP, "syncTime: rfatime failed - %s", buf);
		close(p[0]);
		strcpy(rfaErrStr, buf);
		return NOTOK;
	}
	close(p[0]);
	return OK;

}


/*--------------------------------------------------------------
 *  op_syncTime - synchronize time with peer
 *-------------------------------------------------------------*/
int
op_syncTime (int sd, struct RyOperation *ryo, struct RoSAPinvoke *rox, caddr_t in, struct RoSAPindication *roi) {
	struct type_RFA_SyncTimeArg *sta =
		(struct type_RFA_SyncTimeArg *) in;
	struct type_RFA_SyncTimeRes	 str;
	time_t lt, dt;

	time(&lt);

	if (rox -> rox_nolinked == 0) {
		advise (LLOG_NOTICE, NULLCP,
				"RO-INVOKE.INDICATION/%d: %s, unknown linkage %d",
				sd, ryo -> ryo_name, rox -> rox_linkid);
		return ureject (sd, ROS_IP_LINKED, rox, roi);
	}
	advise (LLOG_DEBUG, NULLCP, "RO-INVOKE.INDICATION/%d: %s",
			sd, ryo -> ryo_name);

	if (sta->role == int_RFA_role_slave) {
		if (timeSlave) {
			return str_error(sd, error_RFA_miscError,"remote is not TIME master"
							 , rox, roi);
		}
		time(&(str.parm));
		str.parm += SENDTIME_DELAY;
	} else {
		if (!timeSlave) {
			return str_error(sd, error_RFA_miscError,"remote is not TIME master"
							 , rox, roi);
		}
		str.parm = dt = sta->time - lt;

		if (changeTime(dt) != OK)
			if (changeTimeWithRfatime(dt) != OK)
				return str_error(sd, error_RFA_miscError, rfaErrStr, rox, roi);

		if (dt > 0)
			advise (LLOG_NOTICE, NULLCP, "syncTime: advanced time %ld sec", dt);
		else
			advise (LLOG_NOTICE, NULLCP, "syncTime: delayed time %ld sec", dt);
	}

	/*--- return result ---*/
	if (RyDsResult (sd, rox->rox_id, (caddr_t)&str, ROS_NOPRIO,roi) == NOTOK) {
		ros_adios (&roi -> roi_preject, "RESULT");
	}

	return OK;
}







