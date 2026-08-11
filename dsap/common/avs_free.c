

#include "quipu/util.h"
#include "quipu/attrvalue.h"

void avs_comp_free (AV_Sequence avs)
{
	AttrV_free_aux (&avs->avseq_av);
	free ((char *) avs);
}

void avs_free (AV_Sequence avs)
{
	AV_Sequence eptr;
	AV_Sequence next;

	for(eptr = avs; eptr != NULLAV; eptr = next) {
		next = eptr->avseq_next;
		avs_comp_free (eptr);
	}
}
