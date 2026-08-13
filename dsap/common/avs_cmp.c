#include "quipu/util.h"
#include "quipu/attrvalue.h"
#include "quipu/dsp.h"        /* for ds_error.h */
#include "quipu/ds_error.h"
#include "quipu/entry.h"

extern short acl_sntx;
int acl_cmp (struct acl *acl1, struct acl *acl2);

int avs_cmp (AV_Sequence a, AV_Sequence b) {
	int i;

	if ( (a == NULLAV) || (b == NULLAV) )   {
		/* @#$%ing access control ! */
		if ( a == b )
			return 0;
		if (a && (a->avseq_av.av_syntax == acl_sntx))
			return acl_cmp ((struct acl *)a->avseq_av.av_struct,
							(struct acl *)NULL);
		if (b && (b->avseq_av.av_syntax == acl_sntx))
			return acl_cmp ((struct acl *)NULL,
							(struct acl *)b->avseq_av.av_struct);
		return (a ?  1 : -1);
	}
	for (; (a != NULLAV) && (b != NULLAV) ; a = a->avseq_next, b = b->avseq_next)
		if ( (i = avs_cmp_comp (a,b))  != 0)
			return (i);
	if ( (a == NULLAV) && (b == NULLAV) )   {
		return 0;
	} else {
		return (a ?  1 : -1);
	}
}
