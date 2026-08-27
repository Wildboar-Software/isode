/* ryresponder.h - include file for the generic idempotent responder */

#include "rosy.h"
#include "logger.h"

static struct dispatch {
	char   *ds_name;
	int	    ds_operation;

	IFP	    ds_vector;
};

extern int  debug;

void	adios (char *, const char *, ...);
void	advise (int, char *, const char *, ...);
void	acs_advise (struct AcSAPabort *aca, char *event);
void	ros_adios (struct RoSAPpreject *rop, char *event), ros_advise (struct RoSAPpreject *rop, char *event);
void	ryr_advise ();

int	ryresponder (int argc, char **argv, char *host, char *myservice, char *mycontext, struct dispatch *dispatches, struct RyOperation *ops, IFP start, IFP stop);
