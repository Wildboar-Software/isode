#include <signal.h>
#include <string.h>
#include <strings.h>
#include "quipu/util.h"
#include "quipu/ds_search.h"
#include "quipu/dua.h"
#include "filter.h"
#include "demanifest.h"
#include "namelist.h"
#include "destrings.h"
static int listPRRoc(char *parentstr, char *thisstr, struct namelist **listp);
static int listAllPRRoc(char *parentstr, struct namelist **listp);
static int listMatchingPRRoc(char *parentstr, char *thisstr, struct namelist **listp);
static int listExactPRRoc(char *objectstr, struct namelist **listp);
static struct ds_search_arg *
fillMostPRRSearchArgoc(char *parentstr, int searchdepth);


extern struct ds_search_arg sarg;
extern struct ds_search_result sresult;
extern struct DSError serror;

extern int abandoned;
extern int highNumber;
extern int exactMatch;
extern char exactString[];

/* PRR stands fpr people, rooms and roles */

struct namelist * prratts;

struct ds_search_arg *fillMostPRRSearchArgoc(char *parentstr, int searchdepth);

void makeExplicitPRRFilter(char *prrstr, struct s_filter **fpp);
void prrFilter1(char *prrstr, struct s_filter **fpp), prrFilter2(char *prrstr, struct s_filter **fpp), prrFilter3(char *prrstr, struct s_filter **fpp), prrFilter4(char *prrstr, struct s_filter **fpp);

VFP explicitPRRoc[] = {makeExplicitPRRFilter, NULLVFP};
VFP normalPRRoc[] = {prrFilter1, prrFilter2, prrFilter3, prrFilter4, NULLVFP};
/*VFP normalPRR[] = {prrFilter1, prrFilter3, NULLVFP};*/

int listPRRoc(char *parentstr, char *thisstr, struct namelist **listp) {
	clearProblemFlags();
	initAlarm();
	if (exactMatch == PERSON)
		return(listExactPRRoc(exactString, listp));
	if (strcmp(thisstr, "*") == 0)
		return (listAllPRRoc(parentstr, listp));
	else
		return (listMatchingPRRoc(parentstr, thisstr, listp));
}

int listAllPRRoc(char *parentstr, struct namelist **listp) {
	int ret;

	sarg = * fillMostPRRSearchArgoc(parentstr, SRA_WHOLESUBTREE);
	makeAllPRRFilter(&sarg.sra_filter);
	ret = makeListPRRs(listp, parentstr);
	if (ret != OK)
		logListSuccess(LIST_ERROR, "prr", 0);
	else
		logListSuccess(LIST_OK, "prr", listlen(*listp));
	freePRRSearchArgs();
	alarmCleanUp();
	return ret;
}

int listMatchingPRRoc(char *parentstr, char *thisstr, struct namelist **listp) {
	VFP * filtarray;
	VFP filterfunc;
	int filtnumber;

	if (index(thisstr, '*') != NULLCP) { /* contains at least one asterisk */
		filtarray = explicitPRRoc;
		filtnumber = -1;
	} else {
		filtarray = normalPRRoc;
		filtnumber = 0;
	}
	sarg = * fillMostPRRSearchArgoc(parentstr, SRA_WHOLESUBTREE);
	while ((filterfunc = *filtarray++) != NULLVFP) {
		filtnumber++;
		filterfunc(thisstr, &sarg.sra_filter);
		if (sarg.sra_filter == NULLFILTER)
			continue;
		if (makeListPRRs(listp, parentstr) != OK) {
			freePRRSearchArgs();
			logSearchSuccess(SEARCH_ERROR, "prr", thisstr, filtnumber, 0);
			alarmCleanUp();
			return NOTOK;
		}
		if (*listp != NULLLIST)
			break;
	}
	if (*listp != NULLLIST)
		logSearchSuccess(SEARCH_OK, "prr", thisstr, filtnumber, listlen(*listp));
	else
		logSearchSuccess(SEARCH_FAIL, "prr", thisstr, filtnumber, 0);
	freePRRSearchArgs();
	alarmCleanUp();
	return OK;
}

int listExactPRRoc(char *objectstr, struct namelist **listp) {
	int ret;

	sarg = * fillMostPRRSearchArgoc(objectstr, SRA_BASEOBJECT);
	makeExactPRRFilter(&sarg.sra_filter);
	ret = makeListPRRs(listp, objectstr);
	freePRRSearchArgs();
	alarmCleanUp();
	return ret;
}

struct ds_search_arg *
fillMostPRRSearchArgoc(char *parentstr, int searchdepth) {
	static struct ds_search_arg arg;
	Attr_Sequence * atl;
	AttributeType at;
	static CommonArgs sca = default_common_args;

	arg.sra_common = sca; /* struct copy */
	arg.sra_common.ca_servicecontrol.svc_timelimit = SVC_NOTIMELIMIT;
	arg.sra_common.ca_servicecontrol.svc_sizelimit= SVC_NOSIZELIMIT;
	arg.sra_subset = searchdepth;
	arg.sra_baseobject = str2dn(parentstr);
	arg.sra_searchaliases = TRUE;
	/* specify attributes of interest */
	arg.sra_eis.eis_allattributes = FALSE;
	atl = &(arg.sra_eis.eis_select);
	/*        for (x = prratts; x != NULLLIST; x = x->next)
	        {
			if ((at = str2AttrT(x->name)) == NULLAttrT)
				continue;
	                *atl = as_comp_alloc();
	                (*atl)->attr_type = at;
			(*atl)->attr_value = NULLAV;
	                atl = &(*atl)->attr_link;
		} */
	at = str2AttrT("objectClass");
	*atl = as_comp_alloc();
	(*atl)->attr_type = at;
	(*atl)->attr_value = NULLAV;
	atl = &(*atl)->attr_link;
	*atl = NULLATTR;
	arg.sra_eis.eis_infotypes = EIS_ATTRIBUTESANDVALUES;
	return (&arg);
}
