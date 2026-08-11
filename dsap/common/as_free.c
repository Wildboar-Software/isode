





#include "quipu/util.h"
#include "quipu/attrvalue.h"

void as_comp_free (Attr_Sequence as) {
	avs_free (as->attr_value);
	free ((char *) as);
}

void as_free (Attr_Sequence as) {
	Attr_Sequence eptr;
	Attr_Sequence next;

	for(eptr = as; eptr != NULLATTR; eptr = next) {
		next = eptr->attr_link;
		as_comp_free (eptr);
	}
}
