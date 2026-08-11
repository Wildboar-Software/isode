/* modifyrdn.h - */





#ifndef QUIPUMODRDN
#define QUIPUMODRDN

#include "quipu/commonarg.h"
#include "quipu/ds_error.h"
#include "quipu/dap.h"

struct ds_modifyrdn_arg {
	CommonArgs mra_common;
	DN mra_object;
	RDN mra_newrdn;
	char deleterdn;             /* set to TRUE or FLASE                 */
};

#endif
