





#include "quipu/util.h"
#include "quipu/name.h"

void dn_comp_free (DN dn)
{
	rdn_free (dn->dn_rdn);
	free ((char *) dn);
}

void dn_free (DN dn)
{
	DN eptr;
	DN next;
	for (eptr = dn; eptr != NULLDN; eptr=next) {
		next = eptr->dn_parent;
		dn_comp_free (eptr);
	}
}
