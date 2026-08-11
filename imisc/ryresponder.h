/* ryresponder.h - include file for the generic idempotent responder */





#ifndef PEPSY_VERSION
#define PEPSY_VERSION 1
#endif
#include "rosy.h"
#include "logger.h"

struct dispatch {
	char   *ds_name;
	int	    ds_operation;
	int (*ds_vector)(int sd, struct RyOperation *ryo, struct RoSAPinvoke *rox, caddr_t in, struct RoSAPindication *roi);
};

void	adios (char *, char *, ...),
		advise (int, char *, char *, ...);
void	acs_advise ();
void	ros_adios (), ros_advise ();
void	ryr_advise (char*, ...);

void	ryresponder (int argc, char **argv, char *host, char *myservice, char *mycontext, struct dispatch *dispatches, struct RyOperation *ops, IFP start, IFP stop);

extern int  debug;

