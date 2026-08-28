#include "quipu/ds_error.h"
#include "quipu/entry.h"
#include "quipu/connection.h"
#include "quipu/find.h"


int dsa_info_parent (DN name, struct DSError *err, struct di_block **di_p, const char master);

struct di_block *ap2di (
	const struct access_point *ap,
	DN name,
	const char master,
	const char di_type,
	const struct oper_act *oper,
	const int cr_type
);

int dsa_info_new (
	DN name,
	const struct dn_seq *dn_stack,
	const int master,
	Entry entry_ptr,
	struct DSError *err,
	struct di_block **di_p
);
