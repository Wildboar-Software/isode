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

#endif
