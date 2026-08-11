/* modify.h - */





#ifndef QUIPUMOD
#define QUIPUMOD

#include "quipu/commonarg.h"
#include "quipu/ds_error.h"
#include "quipu/dap.h"

struct entrymod {
	int em_type;
#define EM_ADDATTRIBUTE         1
#define EM_REMOVEATTRIBUTE      2
#define EM_ADDVALUES            3
#define EM_REMOVEVALUES         4
	Attr_Sequence em_what;      /* holds a single attribute.  the values */
	/* ignored for remove attribute          */
	struct entrymod *em_next;
};

#define NULLMOD (struct entrymod *)NULL
#define em_alloc() (struct entrymod *) smalloc (sizeof (struct entrymod))

struct ds_modifyentry_arg {
	CommonArgs mea_common;
	DN mea_object;
	struct entrymod *mea_changes;
};

#endif
