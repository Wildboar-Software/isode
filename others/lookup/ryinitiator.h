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

void	adios (char *, char *, ...);
void	advise (int, char *, char *, ...);
void	acs_adios (), acs_advise ();
void	ros_adios (), ros_advise ();

int	ryinitiator ();
