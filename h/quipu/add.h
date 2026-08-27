/* add.h - */

#ifndef QUIPUADD
#define QUIPUADD

#include "quipu/commonarg.h"
#include "quipu/ds_error.h"
#include "quipu/dap.h"

struct ds_addentry_arg {
	CommonArgs ada_common;
	DN ada_object;
	Attr_Sequence ada_entry;
};

int dap_addentry (const int ad, int *id, struct ds_addentry_arg *arg, struct DSError *error);
void addentry_arg_free (const struct ds_addentry_arg *arg);
int addentry_arg_dup (struct ds_addentry_arg *src, struct ds_addentry_arg *tgt);

#endif
