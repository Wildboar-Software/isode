/* compare.h - */

#ifndef QUIPUCOMP
#define QUIPUCOMP

#include "quipu/commonarg.h"
#include "quipu/ds_error.h"
#include "quipu/dap.h"

struct ds_compare_arg {
	CommonArgs cma_common;
	DN cma_object;
	AVA cma_purported;
};

struct ds_compare_result {
	CommonResults cmr_common;
	DN cmr_object;
	char cmr_matched;           /* set to TRUE or FALSE                 */
	char cmr_iscopy;            /* values defined in entrystruct        */
	char cmr_pepsycopy;
	time_t cmr_age;
};

int dap_compare (const int ad, int *id, struct ds_compare_arg *arg, struct DSError *error, struct ds_compare_result *result);
void compare_arg_free (struct ds_compare_arg *arg);
int compare_arg_dup (struct ds_compare_arg *src, struct ds_compare_arg *tgt);

#endif
