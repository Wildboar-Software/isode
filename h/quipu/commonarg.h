/* commonarg.h - directory operation common arguments */

#ifndef COMMONARG
#define COMMONARG

#include "pepsy.h"
#include "quipu/attrvalue.h"
#include "quipu/dsp.h"
#include "quipu/authen.h"

#define OP_READ 1
#define OP_COMPARE 2
#define OP_ABANDON 3
#define OP_LIST 4
#define OP_SEARCH 5
#define OP_ADDENTRY 6
#define OP_REMOVEENTRY 7
#define OP_MODIFYENTRY 8
#define OP_MODIFYRDN 9
#define OP_GETEDB 10

struct security_parms {
	struct certificate_list *sp_path;
	DN		sp_name;
	char		*sp_time;
	struct random_number *sp_random;
	int		sp_target;
};

struct security_parms *secp_cpy (struct security_parms *sp);
int check_security_parms(caddr_t data, int type, modtyp *module,
	struct security_parms *sp, struct signature *sig,
	DN *nameptr);

typedef struct extension {
	int		  ext_id;
	char		  ext_critical;
	PE		  ext_item;
	struct extension	* ext_next;
} *Extension;
#define NULLEXT		((Extension) 0)
#define ext_alloc()	(Extension) smalloc(sizeof(struct extension))

void extension_free (struct extension *ext);
struct extension *ext_cpy (struct extension *ext);

typedef struct common_args {    /* Common arguments for operations      */
	ServiceControl      ca_servicecontrol;
	DN                  ca_requestor;
	struct op_progress  ca_progress;
	int                 ca_aliased_rdns;
#define CA_NO_ALIASDEREFERENCED -1
	struct security_parms * ca_security;
	struct signature    * ca_sig;
	struct extension	* ca_extensions;
} common_args, CommonArgs;
#define NULL_COMMONARG ((struct common_args *) NULL)

int ca_dup (struct common_args *src, struct common_args *tgt);
int service_control (PS opt, int argc, char **argv, CommonArgs *ca);
int do_service_control (PS opt, int argc, char **argv, CommonArgs *ca);
void set_my_common_args (struct common_args *ca);

typedef struct common_results {
	DN          cr_requestor;
	/* Secuity stuff to go here.  The       */
	/* is not relevant until this is added  */
	char        cr_aliasdereferenced;
	/* set to TRUE or FALSE                 */

	/* essentially for pepsy, but may be used for real later */
	struct security_parms * cr_security;
	struct alg_id * cr_alg;
	char * 	cr_tmp;
	int 	cr_len;
} common_results, CommonResults;

typedef struct entrystruct {   /* Represents EntryInformation           */
	DN                  ent_dn;
	Attr_Sequence       ent_attr;
	char                ent_iscopy;
#define INFO_MASTER 0x001
#define INFO_COPY   0x002
	/* This is the only info derivable by   */
	/* protocol                             */
	/* INCA also distingusihes local cached */
#define INFO_CACHE 0x003
	char                ent_pepsycopy;
	time_t              ent_age;
	/* age of chaced info                   */
	struct entry	*ent_eptr;
	/* for search acl purposes... yuck!	*/
	struct entrystruct  *ent_next;
} entrystruct, EntryInfo;
void entryinfo_free (EntryInfo *a, int state);
void entryinfo_comp_free (EntryInfo *a, int state);
void entryinfo_cpy (EntryInfo *a, EntryInfo *b);
void entryinfo_append (EntryInfo *a, EntryInfo *b);
void entryinfo_print (PS ps, EntryInfo *entryinfo, int format);
void cache_entry (EntryInfo *ptr, char complete, char vals);

#define NULLENTRYINFO ((EntryInfo *) 0)
#define entryinfo_alloc()          (EntryInfo *) smalloc(sizeof(EntryInfo))
#define entryinfo_cmp(x,y)          (((dn_cmp (x.ent_dn ,y.ent_dn) == OK) && (as_cmp (x.ent_attr ,y.ent_attr) == OK)) ? OK : NOTOK)

void entryinfo_merge (EntryInfo *a, EntryInfo *b, int fast);

typedef struct entryinfoselection {
	/* Rerpesents EntryInformationSelection */
	char        eis_allattributes;
	/* if set to TRUE, all attributes       */
	/* returned, if not as per next arg     */
	Attr_Sequence eis_select;
	/* Sequence of attributes used to show  */
	/* which TYPES are wanted               */
	int        eis_infotypes;
#define EIS_ATTRIBUTETYPESONLY 0
#define EIS_ATTRIBUTESANDVALUES 1
} entryinfoselection, EntryInfoSelection;

int eis_dup (struct entryinfoselection *src, struct entryinfoselection *tgt);

#define LSR_NOLIMITPROBLEM      -1
#define LSR_TIMELIMITEXCEEDED   0
#define LSR_SIZELIMITEXCEEDED   1
#define LSR_ADMINSIZEEXCEEDED   2
typedef struct part_outcome {
	int             poq_limitproblem;
	ContinuationRef poq_cref;
	char		poq_no_ext;
} POQ;

#endif
