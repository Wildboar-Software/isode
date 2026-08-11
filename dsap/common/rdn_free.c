#include "quipu/util.h"
#include "quipu/name.h"

void rdn_comp_free (RDN rdn)
{
	AttrV_free_aux (&rdn->rdn_av);
	free ((char *) rdn);
}

void rdn_free (RDN rdn)
{
	RDN eptr;
	RDN next;

	for (eptr=rdn; eptr!=NULLRDN; eptr=next) {
		next = eptr->rdn_next;
		rdn_comp_free (eptr);
	}
}
