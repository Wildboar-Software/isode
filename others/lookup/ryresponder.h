/* ryresponder.h - include file for the generic idempotent responder */

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
