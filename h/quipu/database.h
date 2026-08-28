#include "quipu/entry.h"

int journal (Entry myentry);
void modify_attr (Entry eptr, DN who);
int do_get_edb (
	const struct getedb_arg *arg,
	struct DSError *error,
	struct getedb_result *result,
	DN binddn,
	const int fd
);
void slave_update (void);
int update_aux (DN dn, const int isroot);
void write_dsa_entry (Entry eptr);
int get_header (FILE * file, int * typeptr, char ** versionptr);
int new_cacheEDB (DN dn);
void check_getedb_ops (const int fd);
