/* remove.h - */

#ifndef QUIPUREMOVE
#define QUIPUREMOVE

#include "quipu/commonarg.h"
#include "quipu/ds_error.h"
#include "quipu/dap.h"

struct ds_removeentry_arg {
	CommonArgs rma_common;
	DN rma_object;
};

int dap_removeentry (const int ad, int *id, struct ds_removeentry_arg *arg, struct DSError *error);
void removeentry_arg_free (struct ds_removeentry_arg *arg);
int removeentry_arg_dup (struct ds_removeentry_arg *src, struct ds_removeentry_arg *tgt);

#endif
