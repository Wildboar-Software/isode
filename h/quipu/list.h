/* list.h - */

#ifndef QUIPULIST
#define QUIPULIST

#include "quipu/commonarg.h"
#include "quipu/ds_error.h"
#include "quipu/dap.h"

struct ds_list_arg {
	CommonArgs lsa_common;
	DN lsa_object;
};

/*
.VE
.VS
*/
struct subordinate {
	RDN sub_rdn;
	char sub_aliasentry;        /* set to TRUE or FALSE                 */
	char sub_copy;              /* set to TRUE or FALSE                 */
	struct subordinate *sub_next;
};
#define NULLSUBORD (struct subordinate *) NULL
void subords_free (struct subordinate *subp);

/*
.VE
.VS
*/
struct ds_list_result {
	CommonResults lsr_common;
	DN lsr_object;
	time_t lsr_age;
	struct subordinate *lsr_subordinates;
	POQ	   lsr_poq;
#define lsr_limitproblem lsr_poq.poq_limitproblem
#define lsr_cr lsr_poq.poq_cref
	struct ds_list_result * lsr_next;
};
/* Note uncorrelated will need to be    */
/* added in to do the secure stuff      */
/* in a distributed manner              */
/* this also applies to search          */

struct list_cache {
	DN              list_dn;
	struct subordinate *list_subs;
	struct subordinate *list_sub_top;
	int             list_count;
	int             list_problem;
	struct list_cache *list_next;
};

#define NULLCACHE (struct list_cache *) NULL

struct list_cache *find_list_cache (DN dn, int sizelimit);

int dap_list (int ad, int *id, struct ds_list_arg *arg, struct DSError *error, struct ds_list_result *result);
void list_arg_free (struct ds_list_arg *arg);
int list_arg_dup (struct ds_list_arg *src, struct ds_list_arg *tgt);

int check_lacl (
	DN binddn,
	DN selfdn,
	AV_Sequence avs,
	int scope,
	int *sizelimit
);

void cache_list (struct subordinate *ptr, int prob, DN dn, int sizelimit);

#endif
