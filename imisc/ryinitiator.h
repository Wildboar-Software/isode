/* ryinitiator.h - include file for the generic interactive initiator */

/*
 * $Header: /xtel/isode/isode/imisc/RCS/ryinitiator.h,v 9.0 1992/06/16 12:23:43 isode Rel $
 *
 *
 * $Log: ryinitiator.h,v $
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


#ifndef PEPSY_VERSION
#define PEPSY_VERSION 1
#endif
#include "rosy.h"
#include "IMISC-types.h"

struct dispatch {
	char   *ds_name;
	int	    ds_operation;

	/* parameter is a void pointer intentionally: it seems that the way this was
	written that anything could be passed in here. */
	int (*ds_argument)(int sd, struct dispatch *ds, char **args, void *parameter);
	modtyp *ds_fr_mod;        /* pointer to table for arguement type */
	int     ds_fr_index;      /* index to entry in tables */

	int (*ds_result)(int sd, int id, int dummy, struct type_IMISC_UTCResult *result, struct RoSAPindication *roi);
	int (*ds_error)(int sd, int id, int error, struct type_IMISC_IA5List *parameter, struct RoSAPindication *roi);

	char   *ds_help;
};

void adios (char* what, char* fmt, ...), advise (char* what, char* fmt, ...);
void acs_adios (), acs_advise ();
void ros_adios (), ros_advise ();

void ryinitiator (int argc, char **argv, char *myservice, char *mycontext, char *mypci, struct RyOperation *ops, struct dispatch *dispatches, IFP quit);
