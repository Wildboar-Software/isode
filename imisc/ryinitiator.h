/* ryinitiator.h - include file for the generic interactive initiator */

#ifndef PEPSY_VERSION
#define PEPSY_VERSION 1
#endif
#include "rosy.h"

typedef struct dispatch dispatch;

typedef int (*ds_argument_t)(int sd, struct dispatch *ds, char **args, void *parameter);

typedef int (*ds_result_t)(int sd, int id, int dummy, caddr_t result, struct RoSAPindication *roi);
typedef void (*ds_error_t)(int sd, int id, int error, caddr_t parameter, struct RoSAPindication *roi);

struct dispatch {
	char   *ds_name;
	int	    ds_operation;

	/* parameter is a void pointer intentionally: it seems that the way this was
	written that anything could be passed in here. */
	ds_argument_t ds_argument;
	modtyp *ds_fr_mod;        /* pointer to table for arguement type */
	int     ds_fr_index;      /* index to entry in tables */

	ds_result_t ds_result;
	ds_error_t ds_error;

	char   *ds_help;
};

void adios (char* what, char* fmt, ...), advise (char* what, char* fmt, ...);
void acs_adios (struct AcSAPabort *aca, const char *event), acs_advise (struct AcSAPabort *aca, char *event);
void ros_adios (struct RoSAPpreject *rop, char *event), ros_advise (struct RoSAPpreject *rop, char *event);

void ryinitiator (const int argc, char **argv, char *myservice, char *mycontext, const char *mypci, struct RyOperation *ops, const struct dispatch *dispatches, ds_argument_t quit);
