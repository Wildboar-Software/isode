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


struct dispatch {
	char   *ds_name;
	int	    ds_operation;

	IFP	    ds_argument;
	modtyp *ds_fr_mod;        /* pointer to table for arguement type */
	int     ds_fr_index;      /* index to entry in tables */

	IFP	    ds_result;
	IFP	    ds_error;

	char   *ds_help;
};


void	adios (char* what, char* fmt, ...),
		advise (char* what, char* fmt, ...);
void	acs_adios (), acs_advise ();
void	ros_adios (), ros_advise ();

int	ryinitiator ();
