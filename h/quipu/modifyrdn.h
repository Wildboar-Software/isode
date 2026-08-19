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

int dap_modifyrdn (int ad, int *id, struct ds_modifyrdn_arg *arg, struct DSError *error);
void modifyrdn_arg_free (struct ds_modifyrdn_arg *arg);
int modifyrdn_arg_dup (struct ds_modifyrdn_arg *src, struct ds_modifyrdn_arg *tgt);

#endif
