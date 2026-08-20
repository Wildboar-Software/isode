#include "quipu/ds_error.h"
#include "quipu/entry.h"
#include "quipu/connection.h"
#include "quipu/find.h"


int dsa_info_parent (DN name, struct DSError *err, struct di_block **di_p, char master);

struct di_block *ap2di (
	struct access_point *ap,
	DN name,
	char master,
	char di_type,
	struct oper_act *oper,
	int cr_type
);

int dsa_info_new (
	DN name,
	struct dn_seq *dn_stack,
	int master,
	Entry entry_ptr,
	struct DSError *err,
	struct di_block **di_p
);
