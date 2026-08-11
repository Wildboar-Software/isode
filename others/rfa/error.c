/*
 * RFA - Remote File Access
 *
 * Access and Management for a partial file system tree that exists
 * at two sites either as master files or slave files
 *
 * error.c : functions for the various error types
 *
 * Contributed by Oliver Wenzel, GMD Berlin, 1990
 *
 * 
 *
 * 
 *
 *
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "RFA-ops.h"        /* operation definitions */
#include "RFA-types.h"  /* type definitions */
#include "ryresponder.h"    /* for generic idempotent responders */
#include "psap.h"
#include "rfa.h"

/*--------------------------------------------------------------*/
/*  aux_error							*/
/*--------------------------------------------------------------*/
int aux_error (int sd, int err, caddr_t param, struct RoSAPinvoke *rox, struct RoSAPindication *roi) {
	if (RyDsError (sd, rox -> rox_id, err, param, ROS_NOPRIO, roi) == NOTOK)
		ros_adios (&roi -> roi_preject, "ERROR");
	return OK;
}

/*--------------------------------------------------------------*/
/*  str_error							*/
/*--------------------------------------------------------------*/
int str_error (int sd, int err, char *str, struct RoSAPinvoke *rox, struct RoSAPindication *roi) {
	struct type_RFA_Reason *qb;
	int r;

	qb= str2qb(str, strlen(str), 1);

	r = aux_error (sd, err, (caddr_t)qb, rox, roi);

	qb_free(qb);
	return r;
}

/*--------------------------------------------------------------*/
/* syserror */
/*--------------------------------------------------------------*/
int syserror (int sd, int err, struct RoSAPinvoke *rox, struct RoSAPindication *roi) {
	return str_error (sd, err, sys_errname (errno), rox, roi);
}

/*--------------------------------------------------------------*/
/* error */
/*--------------------------------------------------------------*/
int error (int sd, int err, int type, struct RoSAPinvoke *rox, struct RoSAPindication *roi) {
	if(type == NOTOK)
		return str_error(sd, err, rfaErrStr, rox, roi);
	else
		return syserror(sd, err, rox, roi);
}

/*--------------------------------------------------------------*/
/* errMsg */
/*--------------------------------------------------------------*/
char *errMsg (int type) {
	if(type == NOTOK)
		return rfaErrStr;
	else
		return sys_errname(errno);
}

/*--------------------------------------------------------------*/
/* statusError */
/*--------------------------------------------------------------*/
int statusError (int sd, int reason, char *user, long since, struct RoSAPinvoke *rox, struct RoSAPindication *roi) {
	struct type_RFA_StatusErrorParm se, *sep = & se;

	if((sep->reason = reason) == int_RFA_reason_locked) {
		sep->user = str2qb(user, strlen(user), 1);
		sep->since = (int)since;
	} else {
		sep->user = NULL;
		sep->since = 0;
	}

	return aux_error (sd, error_RFA_statusError, (caddr_t)sep, rox, roi);
}
