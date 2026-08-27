/* abandon.h - */

#ifndef QUIPUABANDON
#define QUIPUABANDON

#include "quipu/ds_error.h"
#include "quipu/dap.h"

struct ds_abandon_arg {
	int aba_invokeid;
};

int dap_abandon (const int ad, int *id, struct ds_abandon_arg *arg, struct DSError *error);
int abandon_arg_dup (const struct ds_abandon_arg *src, struct ds_abandon_arg *tgt);

#endif
