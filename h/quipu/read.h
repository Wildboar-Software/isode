/* read.h - read operation */

#ifndef QUIPUREAD
#define QUIPUREAD

#include "quipu/commonarg.h"
#include "quipu/ds_error.h"
#include "quipu/dap.h"

/* THIS SECTION GIVES OPERATION PARAMETERS */

struct ds_read_arg {
	CommonArgs rda_common;
	DN rda_object;
	EntryInfoSelection rda_eis;
};

struct ds_read_result {
	CommonResults rdr_common;
	EntryInfo rdr_entry;
};

int dap_read (const int ad, int *id, struct ds_read_arg *arg, struct DSError *error, struct ds_read_result *result);
void read_arg_free (struct ds_read_arg *arg);
int read_arg_dup (struct ds_read_arg *src, struct ds_read_arg *tgt);

#endif
