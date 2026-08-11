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

#endif
