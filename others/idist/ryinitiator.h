/* ryinitiator.h - include file for the generic interactive initiator */

#ifndef PEPSY_VERSION
#define PEPSY_VERSION	1
#endif

#include "rosy.h"

static struct dispatch {
	char   *ds_name;
	int	    ds_operation;

	IFP	    ds_argument;
	modtyp *ds_mod;
	int	    ds_index;

	IFP	    ds_result;
	IFP	    ds_error;

	char   *ds_help;
};

void	adios (char *, char *, ...);
void	advise (char *, char *, ...);
void	acs_adios (struct AcSAPabort *aca, char *event), acs_advise (struct AcSAPabort *aca, char *event);
void	ros_adios (struct RoSAPpreject *rop, char *event), ros_advise (struct RoSAPpreject *rop, char *event);

int	ryinitiator (int argc, char **argv, char *myservice, char *mycontext, char *mypci, struct RyOperation ops[], struct dispatch *dispatches, IFP quit);
