/* template.c - your comments here */

#include "quipu/util.h"
#include "quipu/ds_search.h"
#include "util.h"
#include "filter.h"

extern struct ds_search_arg sarg;
extern struct ds_search_result sresult;
extern struct DSError serror;

struct s_filter *
	andfilter () {
	struct s_filter * fp;

	fp = filter_alloc();
	fp->flt_type = FILTER_AND;
	fp->flt_next = NULLFILTER;
	return fp;
}

struct s_filter *
	orfilter () {
	struct s_filter * fp;

	fp = filter_alloc();
	fp->flt_type = FILTER_OR;
	fp->flt_next = NULLFILTER;
	return fp;
}

struct s_filter *
eqfilter (int matchtype, char *type, char *value) {
	struct s_filter * fp;
	AttributeType at;

	fp = filter_alloc();
	fp->flt_type = FILTER_ITEM;
	fp->FUITEM.fi_type = matchtype;
	at = fp->FUITEM.fi_un.fi_un_ava.ava_type =
			 str2AttrT(type);
	fp->FUITEM.fi_un.fi_un_ava.ava_value =
		str2AttrV(value, at->oa_syntax);
	fp->flt_next = NULLFILTER;
	return fp;
}

struct s_filter *
subsfilter (int substrtype, char *type, char *value) {
	struct s_filter * fp;
	AttributeType at;

	fp = filter_alloc();
	fp->flt_type = FILTER_ITEM;
	fp->FUITEM.fi_type = FILTERITEM_SUBSTRINGS;
	at = fp->FUITEM.fi_un.fi_un_ava.ava_type =
			 str2AttrT(type);
	switch (substrtype) {
	case LEADSUBSTR:
		fp->FUITEM.UNSUB.fi_sub_initial =
			avs_comp_new(str2AttrV(value, at->oa_syntax));
		fp->FUITEM.UNSUB.fi_sub_any = NULLAV;
		fp->FUITEM.UNSUB.fi_sub_final = NULLAV;
		break;
	case TRAILSUBSTR:
		fp->FUITEM.UNSUB.fi_sub_final =
			avs_comp_new(str2AttrV(value, at->oa_syntax));
		fp->FUITEM.UNSUB.fi_sub_any = NULLAV;
		fp->FUITEM.UNSUB.fi_sub_initial = NULLAV;
		break;
	case ANYSUBSTR:
		fp->FUITEM.UNSUB.fi_sub_any =
			avs_comp_new(str2AttrV(value, at->oa_syntax));
		fp->FUITEM.UNSUB.fi_sub_initial = NULLAV;
		fp->FUITEM.UNSUB.fi_sub_final = NULLAV;
		break;
	}
	fp->flt_next = NULLFILTER;
	return fp;
}

struct s_filter *
presfilter (char *type) {
	struct s_filter * fp;

	fp = filter_alloc();
	fp->flt_type = FILTER_ITEM;
	fp->FUITEM.fi_type = FILTERITEM_PRESENT;
	fp->FUITEM.UNTYPE = str2AttrT(type);
	fp->flt_next = NULLFILTER;
	return fp;
}
