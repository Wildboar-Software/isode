/* ryresponder.h - include file for the generic idempotent responder */

/*
 * $Header: /xtel/isode/isode/others/lookup/RCS/ryresponder.h,v 9.0 1992/06/16 12:42:23 isode Rel $
 *
 *
 * $Log: ryresponder.h,v $
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


#ifndef PEPSY_VERSION
#define PEPSY_VERSION 1
#endif
#include "rosy.h"
#include "logger.h"


static struct dispatch {
	char   *ds_name;
	int	    ds_operation;

	IFP	    ds_vector;
};


extern int  debug;


void	adios (char *, char *, ...);
void	advise (int, char *, char *, ...);
void	acs_advise ();
void	ros_adios (), ros_advise ();

int	ryresponder ();
