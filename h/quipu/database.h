#include "quipu/entry.h"

int journal (Entry myentry);
void modify_attr (Entry eptr, DN who);
int do_get_edb (
	struct getedb_arg *arg,
	struct DSError *error,
	struct getedb_result *result,
	DN binddn,
	int fd
);
void slave_update (void);
int update_aux (DN dn, int isroot);
void write_dsa_entry (Entry eptr);
