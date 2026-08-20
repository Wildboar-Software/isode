#include "quipu/commonarg.h"
#include "quipu/ds_error.h"
#include "quipu/entry.h"
#include "quipu/connection.h"

int find_entry (
	DN object,
	common_args *ca,
	DN acl_who,
	struct dn_seq *dn_stack,
	int master,
	Entry *ent_p,
	struct DSError *err,
	struct di_block **di_p,
	int optype
);

int find_child_entry (
	DN object,
	common_args *ca,
	DN acl_who,
	struct dn_seq *dn_stack,
	int master,
	Entry *ent_p,
	struct DSError *err,
	struct di_block **di_p
);

int really_find_entry (
	DN object,
	int deref,
	struct dn_seq *dn_stack,
	int master,	/* Generate only master references - NB
				   does not imply returned entry is master */
	Entry *ent_p,
	struct DSError *err,
	struct di_block **di_p
);