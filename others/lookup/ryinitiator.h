/* ryinitiator.h - include file for the generic interactive initiator */

#include "rosy.h"

static struct dispatch {
	char   *ds_name;
	int	    ds_operation;

	IFP	    ds_argument;
	modtyp	*ds_mod;	/* pointer to module table */
	int		ds_ind;		/* Index into module table */

	IFP	    ds_result;
	IFP	    ds_error;

	char   *ds_help;
};

void	adios (char *, const char *, ...);
void	advise (int, char *, const char *, ...);
void	acs_adios (struct AcSAPabort *aca, char *event), acs_advise (struct AcSAPabort *aca, char *event);
void	ros_adios (struct RoSAPpreject *rop, char *event), ros_advise (struct RoSAPpreject *rop, char *event);

int	ryinitiator (int argc, char **argv, char *myservice, char *mycontext, char *mypci, struct RyOperation ops[], struct dispatch *dispatches, IFP quit);
