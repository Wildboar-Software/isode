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

void	adios (char *what, char *fmt, ...),
		advise (int code, char *what, char *fmt, ...);
void	acs_advise (struct AcSAPabort *aca, char *event);
void	ros_adios (struct RoSAPpreject *rop, char *event), ros_advise (struct RoSAPpreject *rop, char *event);
void	ryr_advise (char *what, char *fmt, ...);

void	ryresponder (const int argc, char **argv, char *host, char *myservice, char *mycontext, const struct dispatch *dispatches, struct RyOperation *ops, int (*start)(int sd, struct AcSAPstart *acs), int (*stop)(int sd, struct AcSAPfinish *acf));

extern int  debug;
