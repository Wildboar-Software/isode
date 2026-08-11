





#include "quipu/util.h"
#include "quipu/attrvalue.h"

short oc_sntx = -1;
IFP oc_hier = NULLIFP;
short acl_sntx = -1;
IFP merge_acl = NULLIFP;

AV_Sequence str2avs (char *str, AttributeType at)
{
	char * ptr;
	char * save,val;
	AV_Sequence avs = NULLAV;
	AV_Sequence newavs;

	if (str == NULLCP)
		return (NULLAV);

	if ((at) && (at->oa_syntax == oc_sntx))
		return ((AV_Sequence)(*oc_hier)(str));

	while ((ptr = index (str,'&')) != 0) {
		save = ptr++;
		save--;
		if (! isspace (*save))
			save++;
		val = *save;
		*save = 0;

		if ((avs) && (at->oa_syntax == acl_sntx)) {
			(*merge_acl)(avs,SkipSpace(str));
			*save = val;
			str = ptr;
			continue;
		}

		newavs = avs_comp_alloc();
		newavs->avseq_next = NULLAV;

		if (str_at2AttrV_aux (str,at,&newavs->avseq_av) == NOTOK)
			return (NULLAV);

		*save = val;
		str = ptr;
		avs = avs_merge (avs,newavs);
	}

	if ((avs) && (at->oa_syntax == acl_sntx)) {
		(*merge_acl)(avs,SkipSpace(str));
		return (avs);
	}

	newavs = avs_comp_alloc();
	newavs->avseq_next = NULLAV;

	if (str_at2AttrV_aux (str,at,&newavs->avseq_av) == NOTOK)
		return (NULLAV);

	return (avs_merge (avs,newavs));

}

AV_Sequence fast_str2avs (char *str, AttributeType at)
{
	char * ptr;
	char * save,val;
	AV_Sequence avs = NULLAV;
	AV_Sequence newavs;
	AV_Sequence fast_avs = NULLAV;
	AV_Sequence fast_tail = NULLAV;
	extern AV_Sequence avs_fast_merge (AV_Sequence a, AV_Sequence b, AV_Sequence c, AV_Sequence d);

	if (str == NULLCP)
		return (NULLAV);

	if ((at) && (at->oa_syntax == oc_sntx))
		return ((AV_Sequence)(*oc_hier)(str));

	while ((ptr = index (str,'&')) != 0) {
		save = ptr++;
		save--;
		if (! isspace (*save))
			save++;
		val = *save;
		*save = 0;

		if ((avs) && (at->oa_syntax == acl_sntx)) {
			(*merge_acl)(avs,SkipSpace(str));
			*save = val;
			str = ptr;
			continue;
		}

		newavs = avs_comp_alloc();
		newavs->avseq_next = NULLAV;

		if (str_at2AttrV_aux (str,at,&newavs->avseq_av) == NOTOK)
			return (NULLAV);

		*save = val;
		str = ptr;
		avs = avs_fast_merge (avs,newavs,fast_avs, fast_tail);
		fast_avs = avs;
		fast_tail = newavs;
	}

	if ((avs) && (at->oa_syntax == acl_sntx)) {
		(*merge_acl)(avs,SkipSpace(str));
		return (avs);
	}

	newavs = avs_comp_alloc();
	newavs->avseq_next = NULLAV;

	if (str_at2AttrV_aux (str,at,&newavs->avseq_av) == NOTOK)
		return (NULLAV);

	avs = avs_fast_merge (avs,newavs,fast_avs, fast_tail);

	return (avs);

}
