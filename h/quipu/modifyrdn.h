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

int dap_modifyrdn (const int ad, int *id, struct ds_modifyrdn_arg *arg, struct DSError *error);
void modifyrdn_arg_free (const struct ds_modifyrdn_arg *arg);
int modifyrdn_arg_dup (const struct ds_modifyrdn_arg *src, struct ds_modifyrdn_arg *tgt);

#endif
