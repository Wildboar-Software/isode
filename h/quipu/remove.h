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

#endif
