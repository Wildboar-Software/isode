/* ryresponder.h - include file for the generic idempotent responder
 *
 * $Header: /xtel/isode/isode/others/rfa/RCS/ryresponder.h,v 9.0 1992/06/16 12:47:25 isode Rel $
 *
 * $Log: ryresponder.h,v $
 * Revision 9.0  1992/06/16  12:47:25  isode
 * Release 8.0
 *
 */

/*
 *                              NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

#include "rosy.h"
#include "logger.h"


static struct dispatch {
	char   *ds_name;
	int	    ds_operation;

	IFP	    ds_vector;
};


void	adios (char *, char *, ...);
void	advise (int, char *, char *, ...);
void	acs_advise ();
void	ros_adios (), ros_advise ();

int	ryresponder ();


extern int  debug;
