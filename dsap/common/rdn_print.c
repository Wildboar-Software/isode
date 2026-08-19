#include <string.h>
#include "quipu/util.h"
#include "quipu/name.h"

void rdn_comp_print (PS ps, RDN rdn, int format) {
	if (rdn!=NULLRDN) {
		AttrT_print (ps,rdn->rdn_at,format);
		ps_print (ps,"=");
		AttrV_print (ps,&rdn->rdn_av,format);
	}
	return;
}

void rdn_print (PS ps, RDN rdn, int format) {
	RDN eptr;
	if (rdn ==  NULLRDN) {
		if (format == READOUT)
			ps_print (ps,"NULL RDN");
		return;
	}
	rdn_comp_print (ps,rdn,format);
	for (eptr=rdn->rdn_next; eptr!=NULLRDN; eptr=eptr->rdn_next) {
		ps_print (ps,"%");
		rdn_comp_print (ps,eptr,format);
	}
}
