/* ds_ext.c - */

#include "quipu/util.h"
#include "quipu/common.h"

extern	LLog	* log_dsap;

void subords_free (struct subordinate *subp) {
	if(subp == NULLSUBORD)
		return;
	subords_free(subp->sub_next);
	rdn_free(subp->sub_rdn);
	free((char *)subp);
}

void ems_free (struct entrymod *emp) {
	if(emp == NULLMOD)
		return;
	ems_free(emp->em_next);
	as_free(emp->em_what);
	free((char *)emp);
}

void aps_free (struct access_point *app) {
	if(app == NULLACCESSPOINT)
		return;
	aps_free(app->ap_next);
	dn_free(app->ap_name);
	if (app->ap_address)
		psap_free (app->ap_address);
	free((char *)app);
}

void crefs_free(ContinuationRef crefp)
{
	if(crefp == NULLCONTINUATIONREF)
		return;
	crefs_free(crefp->cr_next);
	dn_free(crefp->cr_name);
	aps_free(crefp->cr_accesspoints);
	free((char *)crefp);
}
